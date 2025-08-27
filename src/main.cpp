#include "config.h"

#include <iostream>
#include "graphics/window.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

int main() {
    std::cout << "we are all alone on life's journey, held captive by the limitations of human consciousness.\n";
    std::cout << "c++ version: " << __cplusplus << "\n\n";

    try {
        window _window;
        _window.create("reif", DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui_ImplGlfw_InitForOpenGL(_window.get_window(), true);
        ImGui_ImplOpenGL3_Init("#version 330");

        while (!_window.should_close()) {
            _window.poll_events();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();

            ImGui::NewFrame();
                ImGui::Begin("Hello");
                ImGui::Text("asdasd");
            ImGui::End();
            ImGui::Render();

            glViewport(0, 0, 800, 600);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            _window.swap_buffers();
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        _window.destroy();
    } catch(const std::exception& ex) {
        std::cerr << "fatal error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
