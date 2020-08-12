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

#ifndef VIDEOLIBRARY_HPP
#define VIDEOLIBRARY_HPP

#include "VideoID.hpp"
#include "VideoSaveJob.hpp"

#include <vector>
#include <optional>
#include <boost/asio/thread_pool.hpp>

class VideoLibrary
{
public:
  VideoLibrary();

  void SaveClip(std::shared_ptr<std::vector<cv::Mat>> clip, cv::Size clipSize, double fps, size_t seekBackThumbnail);
  std::vector<VideoID> GetClips() const;
  std::optional<std::filesystem::path> GetClipThumbnailPath(const VideoID& name) const;
  std::optional<std::filesystem::path> GetClipVideoPath(const VideoID& name) const;
  bool DeleteClip(const VideoID& name);
private:
  boost::asio::thread_pool pool;
  double fps;
  std::filesystem::path videoPath;
};

#endif