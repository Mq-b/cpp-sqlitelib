cmake_minimum_required(VERSION 3.14)
project(test)

# Find sqlite3mc
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3MC REQUIRED sqlite3mc)

add_executable(test-main test.cc)

target_link_libraries(test-main PRIVATE ${SQLITE3MC_LIBRARIES})
target_include_directories(test-main PRIVATE ${SQLITE3MC_INCLUDE_DIRS} .. .)

