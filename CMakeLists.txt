cmake_minimum_required(VERSION 3.10)

project(stormwatch)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
externalproject_add(ffmpeg-cpp-external
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/ffmpeg-cpp
  CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg-cpp
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)
if(WIN32)
  set(prefix "")
  set(suffix ".lib")
elseif(APPLE)
  set(prefix "lib")
  set(suffix ".a")
else()
  set(prefix "lib")
  set(suffix ".a")
endif()
add_library(ffmpeg-cpp SHARED IMPORTED ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg-cpp/src/ffmpeg-cpp-build/lib/${prefix}ffmpegcpp${suffix})

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
                          web/svg/trash.svg
                          web/png/loader.png
                          web/png/favicon.png)

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS date_time system)
find_package(unofficial-http-parser CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(restinio CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)

find_package(FFMPEG REQUIRED)
find_package(libffi CONFIG REQUIRED)
find_package(ZLIB REQUIRED)

if(WIN32)
  set(PLATFORM_FILES platform/stormwatch.rc)
endif()

add_executable(stormwatch
               src/main.cpp
               src/VideoLibrary.cpp
               src/VideoTrigger.cpp
               src/FPSCounter.cpp
               src/Camera.cpp
               src/Server.cpp
               src/OpenCVInit.cpp
               src/FFmpegInit.cpp
               src/VideoID.cpp
               src/VideoSaveJob.cpp
               lib/xdg/xdg.cpp
               lib/ffmpeg-cpp/src/OpenCodec.cpp
               lib/ffmpeg-cpp/src/FFmpegException.cpp
               lib/ffmpeg-cpp/src/CodecDeducer.cpp
               lib/ffmpeg-cpp/src/VideoFormatConverter.cpp
               lib/ffmpeg-cpp/src/Codecs/Codec.cpp
               lib/ffmpeg-cpp/src/Codecs/VideoCodec.cpp
               lib/ffmpeg-cpp/src/Codecs/VP9Codec.cpp
               lib/ffmpeg-cpp/src/Muxing/Muxer.cpp
               lib/ffmpeg-cpp/src/Muxing/OutputStream.cpp
               lib/ffmpeg-cpp/src/Muxing/VideoOutputStream.cpp
               lib/ffmpeg-cpp/src/FrameSinks/VideoEncoder.cpp
               lib/ffmpeg-cpp/src/Sources/RawVideoDataSource.cpp
               ${PLATFORM_FILES})
target_include_directories(stormwatch PUBLIC ${CMAKE_SOURCE_DIR}/lib/xdg ${CMAKE_SOURCE_DIR}/lib/ffmpeg-cpp/include ${CMAKE_SOURCE_DIR}/lib/ffmpeg-cpp/include/ffmpegcpp)
target_compile_definitions(stormwatch PUBLIC RESTINIO_USE_BOOST_ASIO)
list(REMOVE_ITEM FFMPEG_LIBRARIES "NOTFOUND")
target_link_libraries(stormwatch web_resources ${OpenCV_LIBS} Threads::Threads Boost::system Boost::date_time restinio::restinio nlohmann_json nlohmann_json::nlohmann_json spdlog::spdlog_header_only magic_enum::magic_enum ZLIB::ZLIB libffi pcre ${FFMPEG_LIBRARIES} va-drm va va-x11 vdpau /usr/share/vcpkg/installed/x64-linux/lib/libx264.a /usr/share/vcpkg/installed/x64-linux/lib/libvpx.a)
target_compile_options(stormwatch PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
)
add_dependencies(stormwatch ffmpeg-cpp-external)

if(WIN32)
  target_compile_definitions(stormwatch PUBLIC WINDOWS)
endif()
if(supported)
  message(STATUS "IPO / LTO enabled")
  set_property(TARGET stormwatch PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(STATUS "IPO / LTO not supported: <${error}>")
endif()

install(TARGETS stormwatch DESTINATION .)
install(FILES LICENSE DESTINATION .)
if(WIN32)
  install(DIRECTORY ${CMAKE_BINARY_DIR}/Release/ DESTINATION . FILES_MATCHING PATTERN "*.dll")
endif()

if(WIN32)
  set(CPACK_GENERATOR "WIX;ZIP")
else()
  set(CPACK_GENERATOR "TGZ")
endif()
set(CPACK_CREATE_DESKTOP_LINKS stormwatch)
set(CPACK_PACKAGE_EXECUTABLES stormwatch "Stormwatch")
set(CPACK_PACKAGE_NAME "Stormwatch")
set(CPACK_PACKAGE_VENDOR "Stormwatch")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Joe Dillon")
set(CPACK_WIX_LICENSE_RTF "${CMAKE_SOURCE_DIR}/platform/LICENSE.rtf")
set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/platform/icon.ico")
set(CPACK_WIX_PRODUCT_GUID "69084271-6E41-4008-9853-040FFF94DE99") 
set(CPACK_WIX_UPGRADE_GUID "69084271-6E41-4008-9853-040FFF94DE99")

include(CPack)
