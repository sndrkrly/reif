#include "spritesheet.h"

#include <iostream>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include "../../vendor/stb_image/stb_image.h"

const char* _vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
uniform mat4 projection;
uniform vec2 position;
uniform float scale;
void main() {
    gl_Position = projection * vec4(aPos * scale + position, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* _fragmentShaderSource = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D texture1;
void main() {
    vec4 color = texture(texture1, TexCoord);
    if (color.a < 0.1) discard; // Discard fully transparent pixels
    FragColor = color;
    if (TexCoord.x < 0.0 || TexCoord.x > 1.0 || TexCoord.y < 0.0 || TexCoord.y > 1.0)
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Debug: Red if out of bounds
}
)";

spritesheet::spritesheet(const std::string& filename, int width, int height) : _sprite_width(width), _sprite_height(height), _texture_id(0), _vao(0), _vbo(0), _shader(0) {
    load_texture(filename);

    _cols = _sheet_width / _sprite_width;
    _rows = _sheet_height / _sprite_height;

    std::cout << "spritesheet loaded: " << _sheet_width << "x" << _sheet_height << ", sprites: " << _cols << "x" << _rows << std::endl;

    if (_cols * _sprite_width > _sheet_width || _rows * _sprite_height > _sheet_height) {
        std::cout << "warning: sprite grid exceeds spritesheet dimensions. clamping coordinates." << std::endl;
    }

    create_shader();
    create_buffers();
}

spritesheet::~spritesheet() {
    cleanup();
}

void spritesheet::cleanup() {
    glDeleteTextures(1, &_texture_id);
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteProgram(_shader);
}

void spritesheet::load_texture(const std::string& filename) {
    int channels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename.c_str(), &_sheet_width, &_sheet_height, &channels, 0);

    if (!data) {
        throw std::runtime_error("failed to load texture: " + filename);
    }

    glGenTextures(1, &_texture_id);
    glBindTexture(GL_TEXTURE_2D, _texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, _sheet_width, _sheet_height, 0, format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}

std::vector<float> spritesheet::get_tex_coords(int index) const {
    if (index < 0 || index >= _cols * _rows) {
        throw std::runtime_error("sprite index out of bounds: " + std::to_string(index) + " (max: " + std::to_string(_cols * _rows - 1) + ")");
    }

    int col = index % _cols;
    int row = index / _cols;

    float u0 = (float)(col * _sprite_width) / _sheet_width;
    float v0 = (float)(row * _sprite_height) / _sheet_height;
    float u1 = (float)std::min((col + 1) * _sprite_width, _sheet_width) / _sheet_width;
    float v1 = (float)std::min((row + 1) * _sprite_height, _sheet_height) / _sheet_height;

    return {u0, v0, u1, v0, u1, v1, u0, v1};
}

void spritesheet::render(int index, float x, float y, float scale, int width, int height) {
    if (scale > 10.0f) {
        std::cout << "warning: scale " << scale << " is very large, capping to 10.0f for visibility." << std::endl;
        scale = 10.0f;
    }

    glUseProgram(_shader);
    bind_texture();

    float ortho[16] = {
        2.0f / width, 0.0f, 0.0f, 0.0f,
        0.0f, -2.0f / height, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f
    };

    GLint projLoc = glGetUniformLocation(_shader, "projection");
    if (projLoc == -1) std::cout << "projection uniform not found" << std::endl;
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, ortho);

    GLint posLoc = glGetUniformLocation(_shader, "position");
    if (posLoc == -1) std::cout << "position uniform not found" << std::endl;
    glUniform2f(posLoc, x, height - y);

    GLint scaleLoc = glGetUniformLocation(_shader, "scale");
    if (scaleLoc == -1) std::cout << "scale uniform not found" << std::endl;
    glUniform1f(scaleLoc, scale * _sprite_width);

    std::vector<float> texCoords = get_tex_coords(index);
    float vertices[] = {
        -0.5f, -0.5f, texCoords[0], texCoords[1],
         0.5f, -0.5f, texCoords[2], texCoords[3],
         0.5f,  0.5f, texCoords[4], texCoords[5],
        -0.5f,  0.5f, texCoords[6], texCoords[7]
    };

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Check for OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "opengl error: " << err << std::endl;
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void spritesheet::bind_texture() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    GLint texLoc = glGetUniformLocation(_shader, "texture1");
    if (texLoc == -1) std::cout << "texture uniform not found" << std::endl;
    glUniform1i(texLoc, 0);
}

void spritesheet::create_shader() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &_vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        throw std::runtime_error("vertex shader compilation failed: " + std::string(infoLog));
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &_fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        throw std::runtime_error("fragment shader compilation failed: " + std::string(infoLog));
    }

    _shader = glCreateProgram();
    glAttachShader(_shader, vertexShader);
    glAttachShader(_shader, fragmentShader);
    glLinkProgram(_shader);

    glGetProgramiv(_shader, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(_shader, 512, nullptr, infoLog);
        throw std::runtime_error("shader program linking failed: " + std::string(infoLog));
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void spritesheet::create_buffers() {
    float vertices[] = {
        -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
         0.5f, -0.5f,  1.0f, 0.0f, // bottom-right
         0.5f,  0.5f,  1.0f, 1.0f, // top-right
        -0.5f,  0.5f,  0.0f, 1.0f  // top-left
    };

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
