#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream> // для вывода ошибок

int main() {
    // --- Инициализация GLFW ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Мое приложение с ImGui", nullptr, nullptr);
    if (!window) {
        std::cerr << "Ошибка создания окна GLFW!\n";
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // --- Инициализация ImGui ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // ---------- ЗАГРУЗКА ШРИФТА С КИРИЛЛИЦЕЙ ----------
    // Попробуем загрузить Arial из системной папки Windows
    // Если у вас другая ОС, укажите путь к вашему шрифту с поддержкой кириллицы
    const char* fontPath = "C:/Windows/Fonts/arial.ttf"; // или "arial.ttf", если файл рядом с exe
    ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath, 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    
    if (!font) {
        std::cerr << "Не удалось загрузить шрифт " << fontPath << ". Использую стандартный (без кириллицы).\n";
        // В качестве запасного варианта можно загрузить стандартный шрифт (без кириллицы)
        io.Fonts->AddFontDefault();
    } else {
        std::cout << "Шрифт с кириллицей загружен.\n";
    }

    // После добавления шрифтов нужно пересобрать текстуру
    // Это происходит автоматически при вызове ImGui_ImplOpenGL3_Init,
    // но лучше сделать это вручную, если мы добавляем шрифт до инициализации бэкенда
    // (в данном порядке это необязательно, но оставим для ясности)
    // io.Fonts->Build(); // вызывается внутри ImGui_ImplOpenGL3_Init

    // Настройка бэкендов ImGui (после загрузки шрифтов)
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // --- Главный цикл ---
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- Ваш интерфейс ---
        ImGui::Begin("Привет, ImGui!");
        ImGui::Text("Это моё первое окно.");
        ImGui::Text("Русский текст теперь отображается!");
        ImGui::End();

        // --- Отрисовка ---
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // --- Очистка ---
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}