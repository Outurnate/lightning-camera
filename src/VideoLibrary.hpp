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

#include <opencv2/core/mat.hpp>
#include <filesystem>
#include <vector>
#include <mutex>
#include <optional>
#include <boost/asio/thread_pool.hpp>

class VideoLibrary
{
public:
  VideoLibrary();

  void SaveClip(std::vector<cv::Mat> clip, cv::Size clipSize, double fps, size_t seekBackThumbnail);
  std::vector<std::string> GetClips();
  std::optional<std::filesystem::path> GetClipPath(const std::string& name);
private:
  boost::asio::thread_pool pool;
  double fps;
  std::filesystem::path videoPath;
};

#endif