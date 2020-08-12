#
add_executable(stormwatch
               ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/VideoLibrary.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/VideoTrigger.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/FPSCounter.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/Camera.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/Server.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/OpenCVInit.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/FFmpegInit.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/VideoID.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/VideoSaveJob.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/Platform.cpp
               $<$<PLATFORM_ID:Windows>:${CMAKE_SOURCE_DIR}/platform/stormwatch.rc>)

# Configure source compiler definitions
target_compile_definitions(stormwatch PUBLIC
                           RESTINIO_USE_BOOST_ASIO
                           $<$<PLATFORM_ID:Windows>:WINDOWS>)

# Link deps
target_link_libraries(stormwatch ${CONAN_LIBS} web_resources xdg ffmpeg-cpp cpp-base64)

# PCH for faster rebuilds
target_precompile_headers(stormwatch PRIVATE
  <restinio/all.hpp>
  <opencv2/core.hpp>
  <opencv2/core/mat.hpp>
  <nlohmann/json.hpp>)

# Extra warnings
target_compile_options(stormwatch PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall -Wextra -pedantic>
  $<$<CXX_COMPILER_ID:MSVC>:
    /W4>
)

# Enable LTO/IPO if supported
set_property(TARGET stormwatch PROPERTY INTERPROCEDURAL_OPTIMIZATION ${IPO_SUPPORTED})