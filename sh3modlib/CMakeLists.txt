project(sh3modlib)

add_library(sh3modlib 
	mem/impl/front.cpp
	arc/impl/front.cpp
	crk/memstream.cpp
	crk/tabula.cpp
)
target_include_directories(sh3modlib PUBLIC ./)
target_link_libraries(sh3modlib ZLIB::ZLIB)
if(WIN32)
	target_compile_definitions(sh3modlib PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()