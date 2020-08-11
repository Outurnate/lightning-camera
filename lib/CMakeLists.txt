add_library(xdg STATIC ${CMAKE_CURRENT_SOURCE_DIR}/xdg/xdg.cpp)
target_include_directories(xdg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/xdg)

add_library(cpp-base64 STATIC ${CMAKE_CURRENT_SOURCE_DIR}/cpp-base64/base64.cpp)
target_include_directories(cpp-base64 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/cpp-base64)

add_library(ffmpeg-cpp STATIC
            ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-cpp/src/OpenCodec.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-cpp/src/FFmpegException.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-cpp/src/CodecDeducer.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-cpp/src/VideoFormatConverter.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-cpp/src/Codecs/Codec.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-cpp/src/Codecs/VideoCodec.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-cpp/src/Codecs/VP9Codec.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-cpp/src/Muxing/Muxer.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-cpp/src/Muxing/OutputStream.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-cpp/src/Muxing/VideoOutputStream.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-cpp/src/FrameSinks/VideoEncoder.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-cpp/src/Sources/RawVideoDataSource.cpp)
target_include_directories(ffmpeg-cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-cpp/include ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-cpp/include/ffmpegcpp)