#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#ifdef WINDOWS
#include <shlobj.h>
#else
#include <xdg.h>
#endif

namespace fs = std::filesystem;

#ifdef WINDOWS
fs::path GetAppData()
{
  TCHAR path[MAX_PATH];
  if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, path)))
    return fs::path(std::string(path));
  else
    return fs::current_path();
}
#endif

fs::path GetDataPath()
{
  fs::path path;
#ifdef WINDOWS
  path = GetAppData();
#else
  path = xdg::data().home();
#endif
  return path / "stormwatch";
}

fs::path GetConfigPath()
{
  fs::path path;
#ifdef WINDOWS
  path = GetAppData();
#else
  path = xdg::config().home();
#endif
  return path / "stormwatch";
}

#endif