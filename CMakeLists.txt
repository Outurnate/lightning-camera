cmake_minimum_required(VERSION 3.10)

project(stormwatch)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(lib)
add_subdirectory(web)

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
               $<$<PLATFORM_ID:Windows>:platform/stormwatch.rc>)
target_compile_definitions(stormwatch PUBLIC
                           RESTINIO_USE_BOOST_ASIO
                           $<$<PLATFORM_ID:Windows>:WINDOWS>)
target_link_libraries(stormwatch ${CONAN_LIBS} web_resources xdg ffmpeg-cpp cpp-base64)
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
