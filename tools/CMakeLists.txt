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

set(CMAKE_EXE_LINKER_FLAGS " -static -mwindows")
add_executable(sh3viewer sh3viewer.cpp)
target_link_libraries(sh3viewer PRIVATE sh3modlib)
target_link_libraries(sh3viewer PRIVATE -static-libgcc -static-libstdc++)
target_link_libraries(sh3viewer PRIVATE wx::core wx::base)
target_sources(sh3viewer PRIVATE ../sh3viewer.rc)
if(WIN32)
	target_compile_definitions(sh3viewer PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()