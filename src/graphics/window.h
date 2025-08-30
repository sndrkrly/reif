#pragma once
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class window {
    private:
        GLFWwindow* _window = nullptr;

    public:
        void create(const char* name, int width, int height);
        void destroy();

        void poll_events();
        void swap_buffers();
  
        bool should_close() const;

        GLFWwindow* get_window() const { return _window; }
};
