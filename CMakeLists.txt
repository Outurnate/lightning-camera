cmake_minimum_required(VERSION 3.10)

project(stormwatch)

include(cmake/CMakeRC.cmake)
cmrc_add_resource_library(web_resources
                          WHENCE web
                          web/index.html
                          web/css/dashboard.css
                          web/css/bootstrap.min.css
                          web/js/dashboard.js
                          web/js/bootstrap.min.js
                          web/js/jquery-3.5.1.min.js
                          web/svg/data-transfer-download.svg
                          web/png/loader.png
                          web/png/favicon.png)

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost CONFIG REQUIRED COMPONENTS date_time system)
find_package(unofficial-http-parser CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(restinio CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)

add_executable(stormwatch
               src/main.cpp
               src/VideoLibrary.cpp
               src/VideoTrigger.cpp
               src/FPSCounter.cpp
               src/Camera.cpp
               src/Server.cpp
               src/OpenCVInit.cpp)
target_compile_definitions(stormwatch PUBLIC RESTINIO_USE_BOOST_ASIO)
target_link_libraries(stormwatch web_resources ${OpenCV_LIBS} Threads::Threads Boost::system Boost::date_time restinio::restinio nlohmann_json nlohmann_json::nlohmann_json spdlog::spdlog magic_enum::magic_enum)
target_compile_options(stormwatch PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
)
if(supported)
  message(STATUS "IPO / LTO enabled")
  set_property(TARGET stormwatch PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(STATUS "IPO / LTO not supported: <${error}>")
endif()