#include <iostream>
#include "font.h"

font::font() : _ft(nullptr), _face(nullptr), _vao(0), _vbo(0), _shader(0) {
    _vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
        out vec2 TexCoords;

        uniform mat4 projection;

        void main() {
            gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
            TexCoords = vertex.zw;
        }
    )";

    _fragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoords;
        out vec4 color;

        uniform sampler2D text;
        uniform vec3 textColor;

        void main() {    
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
            color = vec4(textColor, 1.0) * sampled;
        }
    )";
    
    if (FT_Init_FreeType(&_ft)) {
        std::cerr << "couldn't init freetype\n" << std::endl;
    } else {
        std::cout << "freetype init\n";
    }
}

font::~font() {
    cleanup();
}

void font::cleanup() {
    if (_vao != 0) {
        glDeleteVertexArrays(1, &_vao);
        _vao = 0;
    }

    if (_vbo != 0) {
        glDeleteBuffers(1, &_vbo);
        _vbo = 0;
    }

    if (_shader != 0) {
        glDeleteProgram(_shader);
        _shader = 0;
    }
    
    for (auto& pair : _characters) {
        glDeleteTextures(1, &pair.second._texture_id);
    }

    _characters.clear();
    
    if (_face != nullptr) {
        FT_Done_Face(_face);
        _face = nullptr;
    }

    if (_ft != nullptr) {
        FT_Done_FreeType(_ft);
        _ft = nullptr;
    }
}

bool font::load(const std::string& path, unsigned int pixel_size) {
    _characters.clear();

    std::cout << "attempting to load font: " << path << std::endl;

    FILE* file = fopen(path.c_str(), "rb");
    if (!file) {
        std::cerr << "font file does not exist or cannot be opened: " << path << std::endl;
        std::cerr << "current working directory might not be correct." << std::endl;

        return false;
    }

    fclose(file);
    
    FT_Error error = FT_New_Face(_ft, path.c_str(), 0, &_face);
    if (error) {
        std::cerr << "failed to load font: " << path << std::endl;
        std::cerr << "freetype error code: " << error << std::endl;
        
        switch (error) {
            case FT_Err_Unknown_File_Format:
                std::cerr << "the font file format is unsupported or corrupted." << std::endl;
                break;
            case FT_Err_Cannot_Open_Resource:
                std::cerr << "cannot open the font file (permission or path issue)." << std::endl;
                break;
            case FT_Err_Invalid_File_Format:
                std::cerr << "invalid font file format." << std::endl;
                break;
            default:
                std::cerr << "unknown FreeType error occurred." << std::endl;
                break;
        }

        return false;
    }

    FT_Set_Pixel_Sizes(_face, 0, pixel_size);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(_face, c, FT_LOAD_RENDER)) {
            std::cerr << "failed to load Glyph: " << c << std::endl;
            continue;
        }
        
        GLuint texture;
        glGenTextures(1, &texture);
        
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            _face->glyph->bitmap.width,
            _face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            _face->glyph->bitmap.buffer
        );
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        Character character = {
            texture,
            glm::ivec2(_face->glyph->bitmap.width, _face->glyph->bitmap.rows),
            glm::ivec2(_face->glyph->bitmap_left, _face->glyph->bitmap_top),
            static_cast<GLuint>(_face->glyph->advance.x)
        };

        _characters.insert(std::pair<char, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(_face);
    _face = nullptr;

    _shader = create_shader();
    if (_shader == 0) {
        std::cerr << "failed to create shader";
        return false;
    }

    _projectionLoc = glGetUniformLocation(_shader, "projection");
    _textColorLoc = glGetUniformLocation(_shader, "textColor");
    
    setup_render();
    
    std::cout << "font loaded successfully: " << path << " (size: " << pixel_size << ")" << std::endl;
    return true;
}

void font::render_text(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {
    glUseProgram(_shader);
    glUniform3f(_textColorLoc, color.x, color.y, color.z);
    
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(_vao);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = _characters[*c];

        GLfloat xpos = x + ch._bearing.x * scale;
        GLfloat ypos = y - (ch._size.y - ch._bearing.y) * scale;

        GLfloat w = ch._size.x * scale;
        GLfloat h = ch._size.y * scale;
        
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        
        glBindTexture(GL_TEXTURE_2D, ch._texture_id);
        
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (ch._advance >> 6) * scale;
    }
    
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_BLEND);
}

void font::set_projection(const glm::mat4& projection) {
    glUseProgram(_shader);

    glUniformMatrix4fv(_projectionLoc, 1, GL_FALSE, &projection[0][0]);
    glUseProgram(0);
}

GLuint font::compile_shader(const char* source, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint success;
    GLchar infoLog[1024];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cerr << "SHADER_COMPILATION_ERROR of type: " 
                  << (shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") 
                  << "\n" << infoLog << std::endl;

        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

GLuint font::create_shader() {
    GLuint vertexShader = font::compile_shader(_vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = font::compile_shader(_fragmentShaderSource, GL_FRAGMENT_SHADER);
    
    if (vertexShader == 0 || fragmentShader == 0) {
        return 0;
    }
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    
    GLint success;
    GLchar infoLog[1024];

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cerr << "PROGRAM_LINKING_ERROR\n" << infoLog << std::endl;

        glDeleteProgram(program);
        program = 0;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

void font::setup_render() {
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
