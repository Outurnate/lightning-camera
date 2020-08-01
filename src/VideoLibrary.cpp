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

#include "Platform.hpp"

#include <boost/asio/post.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <regex>

namespace fs = std::filesystem;

VideoLibrary::VideoLibrary()
  : pool(1),
    videoPath(GetDataPath() / "videolib")
{
  if (!fs::exists(videoPath))
    fs::create_directories(videoPath);
}

void VideoLibrary::SaveClip(std::vector<cv::Mat> clip, cv::Size clipSize, double fps, size_t seekBackThumbnail)
{
  auto encoded = VideoID().GetID();
  auto videoName = videoPath / fmt::format("{}.mp4", encoded);
  auto thumbName = videoPath / fmt::format("{}.jpeg", encoded);
  
  boost::asio::post(pool, [clip, clipSize, fps, videoName, thumbName, seekBackThumbnail]()
  {
#ifdef WINDOWS
    cv::VideoWriter output(videoName.string(), cv::CAP_FFMPEG, cv::VideoWriter::fourcc('m', 'p', '4', 'v'), fps, clipSize);
#else
    cv::VideoWriter output(videoName.string(), cv::VideoWriter::fourcc('v', 'p', '0', '9'), fps, clipSize);
#endif
    output.set(cv::VIDEOWRITER_PROP_QUALITY, 100);
    for (const cv::Mat& frame : clip)
      output.write(frame);

    cv::Mat thumbnail;
    cv::resize(clip[clip.size() - seekBackThumbnail], thumbnail, cv::Size(128, 96));
    cv::imwrite(thumbName.string(), thumbnail);
    spdlog::get("camera")->info("Clip saved");
  });
}

std::vector<VideoID> VideoLibrary::GetClips()
{
  std::vector<VideoID> clips;
  for (auto& clip : fs::directory_iterator(videoPath))
  {
    auto clipName = clip.path().filename();
    if (clipName.extension() == ".jpeg")
      clips.push_back(VideoID(clipName.stem().string()));
  }
  return clips;
}

std::optional<fs::path> VideoLibrary::GetClipPath(const VideoID& name)
{
  std::regex base64("[A-Za-z0-9\\+/=]+\\.(jpeg|mp4)");
  return std::regex_match(name.GetID(), base64) && fs::exists(videoPath) ?
    std::optional(videoPath / name.GetID()) :
    std::nullopt;
}