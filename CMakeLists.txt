cmake_minimum_required(VERSION 3.21)

project(RatanaOSDesktop LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(RATANA_ENABLE_GTK_BRIDGE "Build GTK4 bridge demo" ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets)

if(RATANA_ENABLE_GTK_BRIDGE)
  find_package(PkgConfig QUIET)
  if(PkgConfig_FOUND)
    pkg_check_modules(GTK4 QUIET IMPORTED_TARGET gtk4)
  endif()
endif()

add_subdirectory(apps)
add_subdirectory(desktop)

enable_testing()
add_test(
  NAME desktop-smoke
  COMMAND ${CMAKE_SOURCE_DIR}/tests/smoke_desktop.sh ${CMAKE_SOURCE_DIR}
)
