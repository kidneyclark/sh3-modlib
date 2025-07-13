project(tools)

add_executable(newmod newmod.cpp)
target_link_libraries(newmod sh3modlib)
if(WIN32)
	target_compile_definitions(newmod PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

add_executable(sh3tools sh3tools.cpp)
target_link_libraries(sh3tools sh3modlib)
if(WIN32)
	target_compile_definitions(sh3tools PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()