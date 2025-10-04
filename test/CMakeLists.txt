cmake_minimum_required(VERSION 3.14)
project(test)

if(WIN32)
  set(SQLite3MC_DIR "D:/App/sqlite3mc-2.1.1-sqlite-3.49.2-icu-win64")
  set(SQLite3MC_LIB "${SQLite3MC_DIR}/dll/sqlite3mc_icu_x64.lib")
  include_directories(${SQLite3MC_DIR}/include)
elseif(UNIX)
  set(SQLite3MC_LIB "/usr/local/lib/libsqlite3mc.a")
  include_directories("/usr/local/include/sqlite3mc")
endif()

add_executable(test-main test.cc)

target_link_libraries(test-main PRIVATE
    ${SQLite3MC_LIB}
)
target_include_directories(test-main PRIVATE .. .)

