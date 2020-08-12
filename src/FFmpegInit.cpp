#include "FFmpegInit.hpp"

#include <ffmpegcpp/ffmpegcpp.h>
#include <spdlog/spdlog.h>

void strip_newlines(std::string& s)
{
  std::string::size_type i = 0;
  while (i < s.length())
  {
    i = s.find('\n', i);
    if (i == std::string::npos)
    {
      break;
    }
    s.erase(i);
  }
}

void log_callback(void* ptr, int level, const char* fmt, va_list args)
{
  (void)ptr;

  const size_t MAX_CHAR = 256;
  char rawMessage[MAX_CHAR];
  vsnprintf(rawMessage, MAX_CHAR, fmt, args);
  std::string message(rawMessage);
  strip_newlines(message);

  switch(level)
  {
  case AV_LOG_VERBOSE:
    spdlog::get("ffmpeg")->trace(message);
    break;
  case AV_LOG_INFO:
    spdlog::get("ffmpeg")->info(message);
    break;
  case AV_LOG_WARNING:
    spdlog::get("ffmpeg")->warn(message);
    break;
  case AV_LOG_ERROR:
    spdlog::get("ffmpeg")->error(message);
    break;
  case AV_LOG_FATAL:
  case AV_LOG_PANIC:
    spdlog::get("ffmpeg")->critical(message);
    break;
  }
}

void SetupFFmpegLogging()
{
  av_log_set_level(AV_LOG_INFO);
  av_log_set_callback(log_callback);
}