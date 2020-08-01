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
#include <opencv2/imgproc.hpp>
#include <spdlog/spdlog.h>

#ifdef WINDOWS
#define ATOMIC_FLAG_INIT
#endif

Camera::Camera(VideoLibrary& videoLibrary)
  : library(videoLibrary),
    abort(ATOMIC_FLAG_INIT),
    applySettings(ATOMIC_FLAG_INIT)
{
  abort.clear();
  applySettings.clear();

  properties = std::map<CameraProperty, double>(
  {
    { CameraProperty::EdgeDetectionSeconds, 2.0 },
    { CameraProperty::DebounceSeconds, 1.0 },
    { CameraProperty::TriggerDelay, 5.0 },
    { CameraProperty::TriggerThreshold, 15.0 },
    { CameraProperty::ClipLengthSeconds, 30.0 },
    { CameraProperty::BayerMode, 0.0 },
    { CameraProperty::Width, 0.0 },
    { CameraProperty::Height, 0.0 }
  });

  ApplyPropertyChange();
}

Camera::~Camera()
{
  Stop();
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

std::vector<uchar> GetDefaultImage()
{
  std::vector<uchar> image;
  cv::imencode(".jpg", cv::Mat(cv::Size(32, 32), CV_8UC3, cv::Scalar(0, 0, 0)), image);
  return image;
}

std::vector<uchar> Camera::GetPreview()
{
  static std::vector<uchar> defaultImage = GetDefaultImage();

  std::vector<uchar> image;
  if (IsRunning())
  {
    std::shared_lock lock(preview.mutex);
    if (!preview.object.empty())
      cv::imencode(".jpg", preview.object, image);
    else
      image = defaultImage;
  }
  else
    image = defaultImage;
  return image;
}

CameraStatus Camera::GetStatus()
{
  std::shared_lock lock(status.mutex);
  return CameraStatus(status.object);
}

void Camera::Start()
{
  double clipLengthSeconds = properties.at(CameraProperty::ClipLengthSeconds);
  std::optional<BayerMode> bayerMode = magic_enum::enum_cast<BayerMode>(properties.at(CameraProperty::BayerMode));
  auto width  = properties.at(CameraProperty::Width);
  auto height = properties.at(CameraProperty::Height);
  std::optional<cv::Size> requestedDimensions = (width > 0 && height > 0) ?
    std::optional<cv::Size>(cv::Size(width, height)) :
    std::nullopt;

  std::unique_lock(cameraThread.mutex);
  if (cameraThread.object.get_id() == std::thread::id())
  {
    abort.test_and_set();
    cameraThread.object = std::thread(&Camera::Run, this, clipLengthSeconds, bayerMode, requestedDimensions);
    spdlog::get("camera")->info("Started camera");
  }
  else
    spdlog::get("camera")->warn("Start request received but camera already started");
  
}

void Camera::Stop()
{
  std::unique_lock(cameraThread.mutex);
  if (cameraThread.object.joinable())
  {
    abort.clear();
    cameraThread.object.join();
    cameraThread.object = std::thread();
    spdlog::get("camera")->info("Stopped camera");
  }
  else
    spdlog::get("camera")->warn("Stop request received but camera already stopped");
}

bool Camera::IsRunning()
{
  std::unique_lock(cameraThread.mutex);
  return cameraThread.object.get_id() != std::thread::id();
}

void Camera::Run(double clipLengthSeconds, std::optional<BayerMode> bayerMode, std::optional<cv::Size> requestedDimensions)
{
  cv::VideoCapture cap;
  std::vector<cv::Mat> frames;
  size_t frameIndex = 0;
  
  cap.open(0);
  if (!cap.isOpened())
  {
    spdlog::get("camera")->critical("ERROR! Unable to open camera");
    return;
  }

  if (requestedDimensions)
  {
    cap.set(cv::CAP_PROP_FRAME_WIDTH,  requestedDimensions.value().width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, requestedDimensions.value().height);
  }
  if (bayerMode)
    cap.set(cv::CAP_PROP_CONVERT_RGB, 0);

  auto propFPS = cap.get(cv::CAP_PROP_FPS);
  status.object.resolution = cv::Size(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT));
  status.object.nominalFPS = propFPS == 0 ? 30 : propFPS;

  size_t bufferSize = clipLengthSeconds * status.object.nominalFPS;
  frames.reserve(bufferSize);
  for (size_t i = 0; i < bufferSize; ++i)
    frames.push_back(cv::Mat(status.object.resolution, CV_8UC3, cv::Scalar(0, 0, 0)));

  while(abort.test_and_set())
  {
    cv::Mat frame;
    cap.read(frame);
    
    if (frame.empty())
    {
      spdlog::get("camera")->critical("ERROR! blank frame grabbed");
      continue;
    }

    if (bayerMode)
    {
      cv::Mat bayer = frame.reshape(0, status.object.resolution.height);
      switch (bayerMode.value())
      {
      default:
      case BayerMode::GB:
        cv::cvtColor(bayer, frame, cv::COLOR_BayerGB2BGR);
        break;
      case BayerMode::BG:
        cv::cvtColor(bayer, frame, cv::COLOR_BayerBG2BGR);
        break;
      case BayerMode::RG:
        cv::cvtColor(bayer, frame, cv::COLOR_BayerRG2BGR);
        break;
      case BayerMode::GR:
        cv::cvtColor(bayer, frame, cv::COLOR_BayerGR2BGR);
        break;
      }
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
      library.SaveClip(clip, status.object.resolution, status.object.nominalFPS, trigger->GetSeekForThumbnail());
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