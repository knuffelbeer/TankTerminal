cmake_minimum_required(VERSION 4.2)
project(jamama)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CURSES_NEED_NCURSES TRUE)
find_package(Curses)
if(Curses_FOUND AND NOT TARGET Curses::Curses)
  add_library(Curses::Curses INTERFACE IMPORTED)
  set_target_properties(
    Curses::Curses
    PROPERTIES
      INTERFACE_LINK_LIBRARIES "${CURSES_LIBRARIES}"
      INTERFACE_INCLUDE_DIRECTORIES "${CURSES_INCLUDE_DIRS}"
  )
endif()

add_executable(server src/server/app.cpp)
add_executable(client src/client/client.cpp)
add_subdirectory(src)
target_link_libraries(client PRIVATE Curses::Curses)
target_compile_options(client PRIVATE -g -O0)
target_link_libraries(server PRIVATE Curses::Curses)
target_compile_options(server PRIVATE -g -O0)

