#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <filesystem>

std::filesystem::path GetDataPath();
std::filesystem::path GetConfigPath();

#endif