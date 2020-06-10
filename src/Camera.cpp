/* stormwatch
 * Copyright (C) 2020 Joe Dillon
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "Camera.hpp"

#include <opencv2/imgcodecs.hpp>
#include <spdlog/spdlog.h>

Camera::Camera(std::shared_ptr<VideoLibrary> videoLibrary, double clipLengthSeconds)
  : library(videoLibrary),
    frames(),
    frameIndex(0),
    abort(ATOMIC_FLAG_INIT),
    applySettings(ATOMIC_FLAG_INIT)
{
  cap.open(0);
  if (!cap.isOpened())
  {
    spdlog::get("camera")->critical("ERROR! Unable to open camera");
    throw 9; // TODO
  }

  status.object.resolution = cv::Size(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT));
  status.object.nominalFPS = cap.get(cv::CAP_PROP_FPS);

  size_t bufferSize = clipLengthSeconds * status.object.nominalFPS;
  frames.reserve(bufferSize);
  for (size_t i = 0; i < bufferSize; ++i)
    frames.push_back(cv::Mat(status.object.resolution, CV_8UC3, cv::Scalar(0, 0, 0)));
  
  properties = std::map<CameraProperty, double>(
  {
    { CameraProperty::EdgeDetectionSeconds, 2.0 },
    { CameraProperty::DebounceSeconds, 1.0 },
    { CameraProperty::TriggerDelay, 5.0 },
    { CameraProperty::TriggerThreshold, 15.0 }
  });

  ApplyPropertyChange();

  abort.test_and_set();
  cameraThread = std::thread(&Camera::Run, this);
}

Camera::~Camera()
{
  abort.clear();
  cameraThread.join();
}

double Camera::GetProperty(CameraProperty property) const
{
  return properties.at(property);
}

void Camera::SetProperty(CameraProperty property, double value)
{
  properties[property] = value;
}

void Camera::ApplyPropertyChange()
{
  applySettings.clear();
}

std::vector<uchar> Camera::GetPreview()
{
  std::shared_lock lock(preview.mutex);
  std::vector<uchar> image;
  cv::imencode(".jpg", preview.object, image);
  return image;
}

CameraStatus Camera::GetStatus()
{
  std::shared_lock lock(status.mutex);
  return CameraStatus(status.object);
}

void Camera::Run()
{
  while(abort.test_and_set())
  {
    cv::Mat frame;
    cap.read(frame);
    
    if (frame.empty())
    {
      spdlog::get("camera")->critical("ERROR! blank frame grabbed");
      continue;
    }
    
    frames[frameIndex] = frame.clone();
    frameIndex = (frameIndex + 1) % frames.size();

    // If something cleared this flag, set it again, but reset the trigger
    if (!applySettings.test_and_set())
    {
      spdlog::get("camera")->info("VideoTrigger settings changed; state cleared");
      trigger = std::make_unique<VideoTrigger>(
        status.object.nominalFPS,
        GetProperty(CameraProperty::EdgeDetectionSeconds),
        GetProperty(CameraProperty::DebounceSeconds),
        GetProperty(CameraProperty::TriggerDelay),
        GetProperty(CameraProperty::TriggerThreshold));
    }

    // Check if there was an event
    if (trigger->ShouldCapture(frame))
    {
      std::vector<cv::Mat> clip(frames.size() + 1);
      for (size_t i = 0; i < (frames.size() + 1); ++i)
        clip.push_back(frames[(frameIndex + i) % frames.size()].clone());
      library->SaveClip(clip, status.object.resolution, status.object.nominalFPS, trigger->GetSeekForThumbnail());
      spdlog::get("camera")->info("Clip saved");
    }
    
    // Update the FPS counter
    counter.Update();

    // If no one is looking, let's update the preview
    if (std::unique_lock lock(preview.mutex, std::try_to_lock); lock)
      preview.object = frame.clone();

    // We didn't acquire a r/o lock above, because the only writes are below
    // The code below is not going to run async to the code above
    if (std::unique_lock lock(status.mutex, std::try_to_lock); lock)
      status.object.measuredFPS = counter.GetFPSAveraged();
  }
}