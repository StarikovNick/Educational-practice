cmake_minimum_required(VERSION 3.10)
project(Educational-practice)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Исполняемый файл
add_executable(${PROJECT_NAME} main.cpp)

# --- ImGui (исходники) ---
file(GLOB IMGUI_SOURCES "pack_for_gui/imgui_lib/*.cpp")
file(GLOB IMGUI_BACKEND_SOURCES "pack_for_gui/imgui_lib/backends/imgui_impl_glfw.cpp" "pack_for_gui/imgui_lib/backends/imgui_impl_opengl3.cpp")
target_sources(${PROJECT_NAME} PRIVATE ${IMGUI_SOURCES} ${IMGUI_BACKEND_SOURCES})


# --- Пути к заголовкам ImGui и GLFW ---
target_include_directories(${PROJECT_NAME} PRIVATE
    "pack_for_gui/imgui_lib"
    "pack_for_gui/imgui_lib/backends"
<<<<<<< HEAD
    "pack_for_gui/glfw/include"         
=======
    "pack_for_gui/glfw/include"          # здесь заголовки GLFW
>>>>>>> 1a3da97909f8362877d4eb042cf321ee1d71c966
)

# --- Путь к библиотекам GLFW ---
target_link_directories(${PROJECT_NAME} PRIVATE
    "pack_for_gui/glfw/lib-mingw-w64"  
)

# --- Линковка ---
# Удалите или закомментируйте старые строки с target_link_directories и target_link_libraries для GLFW

# Добавьте это:
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