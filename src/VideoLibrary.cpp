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
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <regex>

namespace fs = std::filesystem;

VideoLibrary::VideoLibrary()
  : pool(5),
    videoPath(GetDataPath() / "videolib")
{
  if (!fs::exists(videoPath))
    fs::create_directories(videoPath);
  
  spdlog::get("library")->info("Using \"{}\" as library path", videoPath.string());
}

void VideoLibrary::SaveClip(std::shared_ptr<std::vector<cv::Mat>> clip, cv::Size clipSize, double fps, size_t seekBackThumbnail)
{
  auto encoded = VideoID().GetID();
  auto videoName = videoPath / fmt::format("{}.webm", encoded);
  auto thumbName = videoPath / fmt::format("{}.jpeg", encoded);
  
  spdlog::get("library")->info("Requested save for clip {} ({} MB)",
    videoName.string(),
    double(clip->back().total() * clip->back().elemSize() * clip->size()) / 1024.0 / 1024.0);

  boost::asio::post(pool, VideoSaveJob(clip, clipSize, fps, seekBackThumbnail, videoName, thumbName));
}

std::vector<VideoID> VideoLibrary::GetClips() const
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

std::optional<fs::path> VideoLibrary::GetClipPath(const VideoID& name) const
{
  std::regex base64("[A-Za-z0-9\\+/=]+\\.(jpeg|webm)");
  return std::regex_match(name.GetID(), base64) && fs::exists(videoPath) ?
    std::optional(videoPath / name.GetID()) :
    std::nullopt;
}

bool VideoLibrary::DeleteClip(const VideoID& name)
{
  auto path = GetClipPath(name);
  if (path)
  {
    auto thumb = fs::path(path.value()).replace_extension("jpeg");
    if (fs::exists(path.value()) && fs::exists(thumb))
    {
      fs::remove(path.value());
      fs::remove(thumb);

      return true;
    }
  }

  return false;
}