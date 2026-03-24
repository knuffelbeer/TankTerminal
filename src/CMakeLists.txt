target_sources(server PRIVATE
	window.cpp
)
target_sources(client PRIVATE
	window.cpp
)
add_subdirectory(client)
add_subdirectory(server)
