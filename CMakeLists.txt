cmake_minimum_required(VERSION 3.15...4.2)
project(nkutj)
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

add_executable(TankTerminal app.cpp tank.cpp game.cpp managegame.cpp)
target_link_libraries(TankTerminal PRIVATE Curses::Curses)
target_compile_options(TankTerminal PRIVATE
)
