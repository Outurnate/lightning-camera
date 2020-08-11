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

#include "Server.hpp"
#include "OpenCVInit.hpp"
#include "FFmpegInit.hpp"

#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  std::cout <<
    "stormwatch  Copyright (C) 2020  Joe Dillon" << std::endl <<
    "This program comes with ABSOLUTELY NO WARRANTY" << std::endl <<
    "This is free software, and you are welcome to redistribute it" << std::endl <<
    "under certain conditions" << std::endl << std::endl;
  
  uint16_t port;
  std::string address;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "show help message")
    ("port", po::value(&port)->default_value(8080), "port number to listen on")
    ("address", po::value(&address)->default_value("localhost"), "address to bind to")
  ;

  po::variables_map v;
  po::store(po::parse_command_line(argc, argv, desc), v);
  po::notify(v);

  if (v.count("help"))
  {
    std::cout << desc << std::endl;
    return 1;
  }

  spdlog::set_level(spdlog::level::info);

  auto opencv  = spdlog::stdout_color_mt("opencv");
  auto web     = spdlog::stdout_color_mt("web");
  auto camera  = spdlog::stdout_color_mt("camera");
  auto library = spdlog::stdout_color_mt("library");
  auto ffmpeg  = spdlog::stdout_color_mt("ffmpeg");

  SetupOpenCVLogging();
  SetupFFmpegLogging();
  Server().Run(address, port);

  return 0;
}