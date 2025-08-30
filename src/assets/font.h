#pragma once

#include <map>
#include <string>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

struct Character {
    GLuint _texture_id;   // ID handle of the glyph texture
    glm::ivec2 _size;    // Size of glyph
    glm::ivec2 _bearing; // Offset from baseline to left/top of glyph
    GLuint _advance;     // Offset to advance to next glyph
};

class font {
    private:
        FT_Library _ft;
        FT_Face _face;

        std::map<char, Character> _characters;

        GLuint _vao, _vbo;
        GLuint _shader;
        GLuint _projectionLoc, _textColorLoc;
    
        const char* _vertexShaderSource;
        const char* _fragmentShaderSource;
    
        GLuint compile_shader(const char* source, GLenum shaderType);
        GLuint create_shader();
        
        void setup_render();

    public:
        font();
        ~font();

        bool load(const std::string& path, unsigned int pixel_size);
        void render_text(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
        void set_projection(const glm::mat4& projection);
};
