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

#ifndef VIDEOTRIGGER_HPP
#define VIDEOTRIGGER_HPP

#include <opencv2/core/mat.hpp>

#include "MovingAverage.hpp"

class VideoTrigger
{
public:
  VideoTrigger(double fps, double edgeDetectionSeconds = 2, double debounceSeconds = 1, double triggerDelay = 5, unsigned char triggerThreshold = 15);

  bool ShouldCapture(const cv::Mat& frame);
  size_t GetSeekForThumbnail() const;
private:
  const size_t THRESHOLD_WINDOW;
  const size_t DEBOUNCE_COUNT;
  const unsigned char TRIP_THRESHOLD;
  const size_t POST_TRIGGER_COUNT;

  MovingAverage<int> thresholds;
  size_t currentDebounce;
  size_t delayCount;
  bool isDelayed;
  bool thresholdFilled;
  size_t initialFrameCount;
};

#endif