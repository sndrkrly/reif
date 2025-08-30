#include "config.h"

#include <iostream>

#include "graphics/window.h"
#include "assets/font.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

int main() {
    std::cout << "we are all alone on life's journey, held captive by the limitations of human consciousness.\n";
    std::cout << "c++ version: " << __cplusplus << "\n\n";

    try {
        window _window;
        _window.create("reif", DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

        font _font;
        _font.load("../res/fonts/CozetteVector.ttf", 48);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(800), 0.0f, static_cast<float>(600));
        _font.set_projection(projection);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui_ImplGlfw_InitForOpenGL(_window.get_window(), true);
        ImGui_ImplOpenGL3_Init("#version 330");

        while (!_window.should_close()) {
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            _font.render_text("Hello, World!", 25.0f, 550.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            _font.render_text("This is a sample text", 25.0f, 500.0f, 0.8f, glm::vec3(0.5f, 0.8f, 0.2f));
            _font.render_text("FontManager in C++ with FreeType", 25.0f, 450.0f, 0.7f, glm::vec3(0.8f, 0.3f, 0.7f));

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();

            ImGui::NewFrame();
                ImGui::Begin("Hello");
                ImGui::Text("asdasd");
            ImGui::End();
            ImGui::Render();

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            _window.swap_buffers();
            _window.poll_events();
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
