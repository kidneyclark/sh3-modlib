
# wxWidgets
add_subdirectory(wxWidgets)

# zlib
add_subdirectory(zlib)

# glad
add_library(glad glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)