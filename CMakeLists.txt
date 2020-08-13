cmake_minimum_required(VERSION 3.16)

project(stormwatch)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Setup Compiler Options

# Enable LTO/IPO
include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_ERROR)
if(IPO_SUPPORTED)
  message(STATUS "IPO / LTO enabled")
else()
  message(STATUS "IPO / LTO not supported: <${IPO_ERROR}>")
endif()

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Force colored output under Ninja
add_compile_options(
  $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>
  $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>
)

add_subdirectory(lib)
add_subdirectory(web)
add_subdirectory(src)

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
set(CPACK_PACKAGE_VERSION "1.1.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Joe Dillon")
set(CPACK_WIX_LICENSE_RTF "${CMAKE_SOURCE_DIR}/platform/LICENSE.rtf")
set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/platform/icon.ico")
set(CPACK_WIX_PRODUCT_GUID "69084271-6E41-4008-9853-040FFF94DE98") 
set(CPACK_WIX_UPGRADE_GUID "69084271-6E41-4008-9853-040FFF94DE99")

include(CPack)
