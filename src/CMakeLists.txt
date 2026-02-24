target_sources(TankTerminal PRIVATE
	wall.cpp
	window.cpp
	tank.cpp
	game.cpp
	managegame.cpp
)
add_subdirectory(elements)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
