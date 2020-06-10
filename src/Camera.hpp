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

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "VideoLibrary.hpp"
#include "VideoTrigger.hpp"
#include "FPSCounter.hpp"

#include <opencv2/videoio.hpp>
#include <atomic>
#include <memory>
#include <thread>
#include <shared_mutex>
#include <map>
#include <magic_enum.hpp>

enum class CameraProperty
{
  EdgeDetectionSeconds,
  DebounceSeconds,
  TriggerDelay,
  TriggerThreshold
};
constexpr auto CameraPropertyEntries = magic_enum::enum_entries<CameraProperty>();

template<typename T>
struct SharedLockable
{
  T object;
  std::shared_mutex mutex;
};

struct CameraStatus
{
  cv::Size resolution;
  double nominalFPS;
  double measuredFPS;
};

class Camera
{
public:
  Camera(std::shared_ptr<VideoLibrary> videoLibrary, double clipLengthSeconds = 30);
  virtual ~Camera();

  std::vector<uchar> GetPreview();
  double GetProperty(CameraProperty property) const;
  void SetProperty(CameraProperty property, double value);
  void ApplyPropertyChange();
  CameraStatus GetStatus();
private:
  void Run();

  std::unique_ptr<VideoTrigger> trigger;
  std::shared_ptr<VideoLibrary> library;
  FPSCounter counter;
  std::map<CameraProperty, double> properties;
  SharedLockable<cv::Mat> preview;
  SharedLockable<CameraStatus> status;

  cv::VideoCapture cap;
  std::vector<cv::Mat> frames;
  size_t frameIndex;
  std::atomic_flag abort;
  std::atomic_flag applySettings;
  std::thread cameraThread;
};

#endif