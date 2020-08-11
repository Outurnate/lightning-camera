include(${CMAKE_SOURCE_DIR}/cmake/CMakeRC.cmake)
cmrc_add_resource_library(web_resources
                          WHENCE ${CMAKE_CURRENT_SOURCE_DIR}
                          ${CMAKE_CURRENT_SOURCE_DIR}/index.html
                          ${CMAKE_CURRENT_SOURCE_DIR}/css/dashboard.css
                          ${CMAKE_CURRENT_SOURCE_DIR}/css/bootstrap.min.css
                          ${CMAKE_CURRENT_SOURCE_DIR}/js/dashboard.js
                          ${CMAKE_CURRENT_SOURCE_DIR}/js/bootstrap.min.js
                          ${CMAKE_CURRENT_SOURCE_DIR}/js/jquery-3.5.1.min.js
                          ${CMAKE_CURRENT_SOURCE_DIR}/svg/data-transfer-download.svg
                          ${CMAKE_CURRENT_SOURCE_DIR}/svg/trash.svg
                          ${CMAKE_CURRENT_SOURCE_DIR}/png/loader.png
                          ${CMAKE_CURRENT_SOURCE_DIR}/png/favicon.png)