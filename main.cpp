#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <chrono>
#include <thread>
#include "gui.h"
#include "globals.h"
#include "genetic_algorithm.h"


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(1400, 900, "Генетический алгоритм для поиска МОД", nullptr, nullptr);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    loadErrorTexture(); 

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    ImFont* font = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/arial.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    if (!font) io.Fonts->AddFontDefault();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool windowFocused = true;
    glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int focused) {
        if (!focused) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    });

    auto lastRenderTime = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastRenderTime).count();

        int targetFPS = windowFocused ? 60 : 10;
        int targetIntervalMs = 1000 / targetFPS;

        if (elapsed < targetIntervalMs) {
            std::this_thread::sleep_for(std::chrono::milliseconds(targetIntervalMs - elapsed));
        }

        lastRenderTime = std::chrono::steady_clock::now();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        DrawControlPanel();
        DrawInfoPanel();
        DrawVisualizationWindow();
        DrawEvolutionPlot();
        DrawManualInputWindow();
        DrawErrorPopup();
        DrawRandomInputWindow();

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}