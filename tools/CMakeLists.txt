project(tools C)

add_executable(sh3tools sh3tools.c)
target_link_libraries(sh3tools sh3modlib)
if(WIN32)
	target_compile_definitions(sh3tools PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()