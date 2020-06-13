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

#include "VideoLibrary.hpp"

#include <boost/asio/post.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <fmt/format.h>
#include <iostream>
#include <regex>

namespace fs = std::filesystem;

VideoLibrary::VideoLibrary()
  : pool(1),
    videoPath(fs::current_path() / "videolib")
{
  if (!fs::exists(videoPath))
    fs::create_directory(videoPath);
}

void VideoLibrary::SaveClip(std::vector<cv::Mat> clip, cv::Size clipSize, double fps, size_t seekBackThumbnail)
{
  auto timestamp = boost::posix_time::to_iso_extended_string(boost::posix_time::microsec_clock::local_time());
  auto videoName = videoPath / fmt::format("{}.mp4", timestamp);
  auto thumbName = videoPath / fmt::format("{}.jpeg", timestamp);
  
  boost::asio::post(pool, [clip, clipSize, fps, videoName, thumbName, seekBackThumbnail]()
  {
    cv::Mat thumbnail;
    cv::resize(clip[clip.size() - seekBackThumbnail], thumbnail, cv::Size(128, 96));
    cv::imwrite(thumbName.string(), thumbnail);

#ifdef WINDOWS
    cv::VideoWriter output(videoName.string(), cv::CAP_DSHOW, cv::VideoWriter::fourcc('v', 'p', '0', '9'), fps, clipSize);
#else
    cv::VideoWriter output(videoName.string(), cv::VideoWriter::fourcc('v', 'p', '0', '9'), fps, clipSize);
#endif
    output.set(cv::VIDEOWRITER_PROP_QUALITY, 100);
    for (const cv::Mat& frame : clip)
      output.write(frame);
  });
}

std::vector<std::string> VideoLibrary::GetClips()
{
  std::vector<std::string> clips;
  for (auto& clip : fs::directory_iterator(videoPath))
  {
    auto clipName = clip.path().filename();
    if (clipName.extension() == ".mp4")
      clips.push_back(clipName.stem().string());
  }
  return clips;
}

std::optional<fs::path> VideoLibrary::GetClipPath(const std::string& name)
{
  return fs::exists(videoPath) ? std::optional(videoPath / name) : std::nullopt;
}