#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

class spritesheet {
    private:
        GLuint _texture_id;
        GLuint _vao, _vbo;
        GLuint _shader;

        int _sprite_width, _sprite_height;
        int _sheet_width, _sheet_height;
        int _cols, _rows;

        void load_texture(const std::string& filename);
        std::vector<float> get_tex_coords(int index) const;

        void create_shader();
        void create_buffers();

    public:
        spritesheet(const std::string& filename, int width, int height);
        ~spritesheet();

        void render(int index, float x, float y, float scale = 1.0f, int width = 800, int height = 600);
        void bind_texture() const;

        void cleanup();
};
