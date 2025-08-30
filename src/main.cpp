#include "config.h"
#include <exception>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "graphics/window.h"

#include "assets/font.h"
#include "assets/spritesheet.h"

int main() {
    std::cout << "we are all alone on life's journey, held captive by the limitations of human consciousness.\n";
    std::cout << "c++ version: " << __cplusplus << "\n\n";

    try {
        window _window;
        font _font;

        _window.create("reif", DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
        _font.load("../res/fonts/Ticketing.ttf", 42);

        spritesheet* _sprite = nullptr;
        _sprite = new spritesheet("../res/images/tilemap_2_packed.png", 16, 16);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glm::mat4 font_projection = glm::ortho(0.0f, static_cast<float>(800), 0.0f, static_cast<float>(600));
        _font.set_projection(font_projection);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui_ImplGlfw_InitForOpenGL(_window.get_window(), true);
        ImGui_ImplOpenGL3_Init("#version 330");

        int width, height;
        glfwGetFramebufferSize(_window.get_window(), &width, &height);

        while (!_window.should_close()) {
            glClear(GL_COLOR_BUFFER_BIT);

            _font.render_text("asdasdasd", 10.0f, 10.0f, 0.8f, glm::vec3(0.7f, 0.7f, 0.7f));
            _sprite->render(16, width / 2, height / 2, 2.0f, width, height);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();

            ImGui::NewFrame();
                ImGui::Begin("Hello");
                ImGui::Text("asdasd");
            ImGui::End();
            ImGui::Render();

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            _window.poll_events();
            _window.swap_buffers();
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        _window.cleanup();
        _font.cleanup();

        delete _sprite;
    } catch(const std::exception& ex) {
        std::cerr << "fatal error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
