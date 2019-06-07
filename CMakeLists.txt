cmake_minimum_required(VERSION 2.8)

project(spiderdisco C)

SET(LIBSUPERDERPY_GAMENAME "spiderdisco" CACHE INTERNAL "")
SET(LIBSUPERDERPY_GAMENAME_PRETTY "Spider Disco" CACHE INTERNAL "")

SET(LIBSUPERDERPY_APPID "com.holypangolin.SpiderDisco" CACHE INTERNAL "")
SET(LIBSUPERDERPY_ORIENTATION "LANDSCAPE" CACHE INTERNAL "")
SET(LIBSUPERDERPY_VERSION "1.0" CACHE INTERNAL "")

set(EMSCRIPTEN_TOTAL_MEMORY "1024" CACHE INTERNAL "")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/libsuperderpy/cmake")

include(libsuperderpy)

add_subdirectory(libsuperderpy)
add_subdirectory(src)
add_subdirectory(data)
