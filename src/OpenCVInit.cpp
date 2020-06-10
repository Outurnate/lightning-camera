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

#include "OpenCVInit.hpp"

#include <opencv2/core.hpp>
#include <spdlog/spdlog.h>

int handleError(int status, const char* func_name,
                const char* err_msg, const char* file_name,
                int line, void* userdata)
{
  (void)userdata;
  spdlog::get("opencv")->error("({} {}) {} at {}:{}", status, func_name, err_msg, file_name, line);
  return 0;
}

void SetupOpenCVLogging()
{
  cv::redirectError(handleError);
}