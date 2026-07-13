cmake_minimum_required(VERSION 3.10)
project(Educational-practice)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} main.cpp data/GraphData.cpp gui/gui.cpp gui/globals.cpp core/genetic_algorithm.cpp core/disjoint_set.cpp)

file(GLOB IMGUI_SOURCES "lib_for_gui/imgui_lib/*.cpp")
file(GLOB IMGUI_BACKEND_SOURCES "lib_for_gui/imgui_lib/backends/imgui_impl_glfw.cpp" "lib_for_gui/imgui_lib/backends/imgui_impl_opengl3.cpp")
target_sources(${PROJECT_NAME} PRIVATE ${IMGUI_SOURCES} ${IMGUI_BACKEND_SOURCES})


file(GLOB IMPLOT_SOURCES "lib_for_gui/implot/implot.cpp" "lib_for_gui/implot/implot_items.cpp")
target_sources(${PROJECT_NAME} PRIVATE ${IMPLOT_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    "lib_for_gui/imgui_lib"
    "lib_for_gui/imgui_lib/backends"
    "lib_for_gui/glfw/include"
    "lib_for_gui/implot"
    "data"
    "core"
    "gui"       
)

target_link_directories(${PROJECT_NAME} PRIVATE
    "lib_for_gui/glfw/lib-mingw-w64"  
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    libglfw3.a
    opengl32
    gdi32
    user32
    kernel32
)

target_compile_options(${PROJECT_NAME} PRIVATE -fexec-charset=UTF-8 -finput-charset=UTF-8)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    _CRT_SECURE_NO_WARNINGS
    __STDC_SECURE_LIB__=0
)