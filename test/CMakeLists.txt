cmake_minimum_required(VERSION 3.14)
project(test)

set(SQLite3MC_DIR "D:/App/sqlite3mc-2.1.1-sqlite-3.49.2-icu-win64")

add_executable(test-main test.cc)
include_directories(${SQLite3MC_DIR}/include)

target_link_libraries(test-main PRIVATE 
    ${SQLite3MC_DIR}/dll/sqlite3mc_icu_x64.lib
)
target_include_directories(test-main PRIVATE .. .)

enable_testing()

add_test(
  NAME TestMain
  COMMAND test-main
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
