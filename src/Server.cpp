/* lightning-camera
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

#include <restinio/all.hpp>
#include <nlohmann/json.hpp>
#include <cmrc/cmrc.hpp>
#include <spdlog/spdlog.h>

#include "Camera.hpp"

CMRC_DECLARE(web_resources);

using json = nlohmann::json;
using router_t = restinio::router::express_router_t<restinio::router::std_regex_engine_t>;

template <typename RESP>
inline RESP init(RESP resp)
{
	resp.append_header("Server", "lightning-camera");
	resp.append_header_date_field();

	return resp;
}

inline std::string ReadFile(const cmrc::file& file)
{
  std::string contents;
  for (const char& i : file)
    contents += i;
  return contents;
}

inline void StaticDirectoryMapper(std::unique_ptr<router_t>& router, cmrc::embedded_filesystem& fs, std::string path, std::string mime)
{
  router->http_get(
    fmt::format("/{}/([a-zA-Z0-9\\.:-]+)", path),
    [fs, path, mime](auto req, auto params)
    {
      auto requestedFile = fmt::format("{}/{}", path, std::string(params[0]));
      if (fs.exists(requestedFile))
      {
        return init(req->create_response())
          .append_header(restinio::http_field::content_type, mime)
          .set_body(ReadFile(fs.open(requestedFile)))
          .done();
      }
      else
        return restinio::request_rejected();
    });
}

inline auto CreateHandler(cmrc::embedded_filesystem fs, std::shared_ptr<VideoLibrary> library, std::shared_ptr<Camera> camera)
{
  auto router = std::make_unique<router_t>();

  router->http_get(
    "/live.jpeg",
    [camera](auto req, auto)
    {
      return init(req->create_response())
        .append_header(restinio::http_field::content_type, "image/jpeg")
        .set_body(camera->GetPreview())
        .done();
    });

  router->http_get(
    "/stats",
    [camera](auto req, auto)
    {
      auto cameraStatus = camera->GetStatus();
      json stats;
      stats["width"]       = cameraStatus.resolution.width;
      stats["height"]      = cameraStatus.resolution.height;
      stats["nominalFPS"]  = cameraStatus.nominalFPS;
      stats["measuredFPS"] = cameraStatus.measuredFPS;

      return init(req->create_response())
        .append_header(restinio::http_field::content_type, "text/json; charset=utf-8")
        .set_body(stats.dump())
        .done();
    });

  router->http_get(
    "/clips",
    [library](auto req, auto)
    {
      json clips;
      for (const std::string& clip : library->GetClips())
        clips.push_back(json::object(
          {
            { "title", clip },
            { "video", fmt::format("/clips/{}.mp4", clip) },
            { "thumbnail", fmt::format("/clips/{}.jpeg", clip) }
          }));

      return init(req->create_response())
        .append_header(restinio::http_field::content_type, "text/json; charset=utf-8")
        .set_body(clips.dump())
        .done();
    });
  
  router->http_get(
    "/clips/([a-zA-Z0-9\\.:-]+)",
    [library](auto req, auto params)
    {
      auto clipPath = library->GetClipPath(std::string(params[0]));
      if (clipPath)
      {
        return init(req->create_response())
          .append_header(restinio::http_field::content_type, "video/mp4")
          .set_body(restinio::sendfile(clipPath.value().string()))
          .done();
      }
      else
        return restinio::request_rejected();
    });

  router->http_get(
    "/settings",
    [camera](auto req, auto)
    {
      json settings;
      for (auto property : CameraPropertyEntries)
        settings[std::string(property.second)] = camera->GetProperty(property.first);
      return init(req->create_response())
        .append_header(restinio::http_field::content_type, "text/json; charset=utf-8")
        .set_body(settings.dump())
        .done();
    });

  router->http_post(
    "/settings",
    [camera](auto req, auto)
    {
      const auto parameters = restinio::parse_query(req->header().query());
      for (auto property : CameraPropertyEntries)
        camera->SetProperty(property.first, restinio::value_or(parameters, property.second, camera->GetProperty(property.first)));
      camera->ApplyPropertyChange();
      return init(req->create_response())
        .append_header(restinio::http_field::content_type, "text/json; charset=utf-8")
        .set_body("{}")
        .done();
    });

  StaticDirectoryMapper(router, fs, "js", "text/javascript; charset=utf-8");
  StaticDirectoryMapper(router, fs, "css", "text/css; charset=utf-8");
  StaticDirectoryMapper(router, fs, "svg", "image/svg+xml");
  StaticDirectoryMapper(router, fs, "png", "image/png");

  router->http_get(
    "/",
    [fs](auto req, auto)
    {
      return init(req->create_response())
        .append_header(restinio::http_field::content_type, "text/html; charset=utf-8")
        .set_body(ReadFile(fs.open("index.html")))
        .done();
    });

  router->non_matched_request_handler(
    [](auto req)
    {
      return req->create_response(restinio::status_not_found())
        .connection_close()
        .done();
    });

  return router;
}

class spdlog_logger_t
{
public:
  template<typename Msg_Builder>
  void trace(Msg_Builder&& mb)
  {
    spdlog::get("web")->trace(mb());
  }

  template<typename Msg_Builder>
  void info(Msg_Builder&& mb)
  {
    spdlog::get("web")->info(mb());
  }

  template<typename Msg_Builder>
  void warn(Msg_Builder&& mb)
  {
    spdlog::get("web")->warn(mb());
  }

  template<typename Msg_Builder>
  void error(Msg_Builder&& mb)
  {
    spdlog::get("web")->error(mb());
  }
};

void RunCameraServer()
{
  std::shared_ptr<VideoLibrary> library = std::make_shared<VideoLibrary>();
  std::shared_ptr<Camera> camera = std::make_shared<Camera>(library);

  using traits_t =
    restinio::traits_t<
      restinio::asio_timer_manager_t,
      spdlog_logger_t,
      router_t>;

  restinio::run(
    restinio::on_this_thread<traits_t>()
      .port(8080)
      .address("localhost")
      .request_handler(CreateHandler(cmrc::web_resources::get_filesystem(), library, camera)));
}