#pragma once
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <zlib.h>
#include <ft2build.h>
#include FT_FREETYPE_H

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
