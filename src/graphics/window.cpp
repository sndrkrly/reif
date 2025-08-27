#include <iostream>
#include "window.h"

void window::create(const char* name, int width, int height) {
    if (!glfwInit()) {
        throw std::runtime_error("glfw init failed\n");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    _window = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (!_window)
    {
        glfwTerminate();
        throw std::runtime_error("window creation failed\n");
    }

    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;
    const GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "glew init failed: " << glewGetErrorString(err) << "\n" << std::endl;
    } else {
        std::cout << "glew init\n";
    }

    std::cout << "opengl init\nversion: " << glGetString(GL_VERSION) << std::endl;

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "couldn't init freetype!\n";
    } else {
        std::cout << "freetype init\n";
        FT_Done_FreeType(ft);
    }

    int fb_width = 0, fb_height = 0;
    glfwGetFramebufferSize(_window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);
}

void window::destroy() {
    if (_window) {
        glfwDestroyWindow(_window);
        _window = nullptr;
    }
}

void window::poll_events() {
    glfwPollEvents();
}

void window::swap_buffers() {
    if (_window) {
        glfwSwapBuffers(_window);
    }
}

bool window::should_close() const {
    return _window ? glfwWindowShouldClose(_window) : true;
}
