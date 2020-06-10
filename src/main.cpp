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

#include "Server.hpp"
#include "OpenCVInit.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

int main(int, char**)
{
  spdlog::set_level(spdlog::level::info);

  auto opencv = spdlog::stdout_color_mt("opencv");
  auto web = spdlog::stdout_color_mt("web");
  auto camera = spdlog::stdout_color_mt("camera");

  SetupOpenCVLogging();
  RunCameraServer();

  return 0;
}