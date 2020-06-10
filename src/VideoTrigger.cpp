/* lightning-camera
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

#include "VideoTrigger.hpp"

#include <opencv2/core.hpp>
#include <numeric>
#include <spdlog/spdlog.h>

#include "OpenCVUtils.hpp"

VideoTrigger::VideoTrigger(double fps, double edgeDetectionSeconds, double debounceSeconds, double triggerDelay, unsigned char triggerThreshold)
  : THRESHOLD_WINDOW(edgeDetectionSeconds * fps),
    DEBOUNCE_COUNT(debounceSeconds * fps),
    TRIP_THRESHOLD(triggerThreshold),
    POST_TRIGGER_COUNT(triggerDelay * fps),
    thresholds(THRESHOLD_WINDOW, 0),
    currentDebounce(0),
    delayCount(0),
    isDelayed(false),
    thresholdFilled(false),
    initialFrameCount(THRESHOLD_WINDOW)
{
}

size_t VideoTrigger::GetSeekForThumbnail() const
{
  return POST_TRIGGER_COUNT;
}

bool VideoTrigger::ShouldCapture(const cv::Mat& frame)
{
  if (initialFrameCount != 0)
    --initialFrameCount;
  else
    thresholdFilled = true;
  
  unsigned char threshold = MeanIntensity(frame);
  thresholds.Push(threshold);
  unsigned char mean = thresholds.Mean();

  if (currentDebounce != 0)
    --currentDebounce;

  if (!isDelayed && currentDebounce == 0 && threshold > mean && (threshold - mean) > TRIP_THRESHOLD)
  {
    currentDebounce = DEBOUNCE_COUNT;
    delayCount = POST_TRIGGER_COUNT;
    isDelayed = true;
    spdlog::get("camera")->info("Threshold event ({} > {})", threshold, mean);
  }

  if (delayCount != 0)
    --delayCount;
  else if (isDelayed)
  {
    isDelayed = false;
    return thresholdFilled; // don't trigger unless we have a full set of samples
  }

  return false;
}