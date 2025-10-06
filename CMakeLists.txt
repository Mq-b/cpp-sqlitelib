cmake_minimum_required(VERSION 3.14)
project(cpp-sqlitelib)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /utf-8")
endif()

# Find sqlite3mc
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3MC REQUIRED sqlite3mc)

# Include directories
include_directories(${SQLITE3MC_INCLUDE_DIRS})

# Link directories
link_directories(${SQLITE3MC_LIBRARY_DIRS})

add_subdirectory(test)

install(FILES sqlitelib.h DESTINATION include)
