project(sh3modlib C)

add_library(sh3modlib 
	arc/arc_table.h
	arc/arc_table.c
)
target_include_directories(sh3modlib PUBLIC sh3modlib)