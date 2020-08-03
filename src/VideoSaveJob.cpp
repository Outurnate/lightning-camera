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

#include "VideoSaveJob.hpp"

#include <spdlog/spdlog.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
}

// Translated to constexpr from
// https://rosettacode.org/wiki/Convert_decimal_number_to_rational#C
// I have pretty much no idea how this works...but it do
constexpr AVRational NearestRational(double f)
{
  int h[3] = { 0, 1, 0 };
  int k[3] = { 1, 0, 0 };
  int n = 1;
  int neg = 0;
  int md = 16;

  if (f < 0)
  {
    neg = 1;
    f = -f;
  }

  while (f != floor(f))
  {
    n <<= 1;
    f *= 2;
  }
  int d = f;
  
  for (int i = 0; i < 64; ++i)
  {
    int a = n ? d / n : 0;
    if (i && !a)
      break;

    int x = d;
    d = n;
    n = x % n;

    x = a;
    if (k[1] * a + k[0] >= md)
    {
      x = (md - k[0]) / k[1];
      if (x * 2 >= a || k[1] >= md)
        i = 65;
      else
        break;
    }

    h[2] = x * h[1] + h[0]; h[0] = h[1]; h[1] = h[2];
    k[2] = x * k[1] + k[0]; k[0] = k[1]; k[1] = k[2];
  }

  return AVRational { neg ? -h[1] : h[1], k[1] };
}

constexpr AVRational Inverse(const AVRational& v)
{
  return AVRational { v.den, v.num };
}

VideoSaveJob::VideoSaveJob(
  std::shared_ptr<std::vector<cv::Mat>> data,
  cv::Size dimensions,
  double fps,
  size_t seekBackThumbnail,
  std::filesystem::path videoPath,
  std::filesystem::path thumbPath)
  : data(data), dimensions(dimensions), fps(fps),
    seekBackThumbnail(seekBackThumbnail), videoPath(videoPath),
    thumbPath(thumbPath)
{}

void VideoSaveJob::operator()()
{
  spdlog::get("library")->info("Started save for clip {}", videoPath.string());
  auto rationalFPS = NearestRational(fps);
  spdlog::get("library")->info("Estimated FPS at {}/{}", rationalFPS.num, rationalFPS.den);

  unsigned i = 0;
  for (const cv::Mat& srcFrame : *data)
  {
    if (srcFrame.empty())
    {
      ++i;
      continue;
    }
    spdlog::get("library")->trace("Wrote frame {}/{}", i++, data->size());
  }

  cv::Mat originalThumbnail = (*data)[data->size() - seekBackThumbnail];
  cv::Mat thumbnail;
  if (originalThumbnail.empty())
    originalThumbnail = data->back();
  cv::resize(originalThumbnail, thumbnail, cv::Size(128, 96));
  cv::imwrite(thumbPath.string(), thumbnail);
  spdlog::get("library")->info("Clip saved as {}", videoPath.string());
}