target_sources(server PRIVATE
	window.cpp
	reader.cpp
)
target_sources(client PRIVATE
	window.cpp
	reader.cpp
)
add_subdirectory(client)
add_subdirectory(server)
