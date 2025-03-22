project(sh3modlib C)

add_library(sh3modlib 
	arc/arc_table.c
	utils.c
)
target_include_directories(sh3modlib PUBLIC ./)
target_link_libraries(sh3modlib ZLIB::ZLIB)
if(WIN32)
	target_compile_definitions(sh3modlib PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()