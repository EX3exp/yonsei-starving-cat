#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <iostream>
#include <map>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <learnopengl/shader.h>

struct Character {
    GLuint TextureID;    // ID handle of the glyph texture
    glm::ivec2 Size;     // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;      // Offset to advance to next glyph
};


class Text {
public:
    Text(const std::string& vertexShaderPath, const std::string& fragShaderPath, const std::string& fontPath, const glm::mat4& projection, const std::u32string& text, glm::vec3 &color)
        : shader(vertexShaderPath.c_str(), fragShaderPath.c_str()), projectionMatrix(projection), text(text), color(color)
    {
        lineNum = 0; // 줄의 개수, 줄이 바꿔질 때마다 1씩 더해짐 - 최소값은 1
        shader.use();
        shader.setMat4("projection", projectionMatrix);

        if (FT_Init_FreeType(&ft)) {
            std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            exit(-1);
        }

        if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
            std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
            exit(-1);
        }

        FT_Set_Pixel_Sizes(face, 0, 48);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Ensure an unicode character map is loaded
        FT_Select_Charmap(face, FT_ENCODING_UNICODE);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ~Text() 
    {
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    void draw()
    {
        shader.use();
        shader.setVec3("textColor", color);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        std::vector<std::u32string> lines;
        std::u32string currentLine;

        // Split text into lines
        for (auto c : text) {
            if (c == U'\n') {
                lines.push_back(currentLine);
                currentLine.clear();
            }
            else {
                currentLine += c;
            }
        }
        lines.push_back(currentLine); // Add the last line

        GLfloat yTemp = y;

        for (const auto& line : lines) {
            // Calculate the width of the current line
            GLfloat lineWidth = 0.0f;
            for (auto c : line) {
                if (Characters.find(c) == Characters.end()) {
                    Characters[c] = LoadCharacter(face, c);
                }
                Character ch = Characters[c];
                lineWidth += (ch.Advance >> 6) * scale;
            }

            // Calculate the starting x position for the current line to center it
            GLfloat xTemp = x - lineWidth / 2.0f;

            for (auto c : line) {
                Character ch = Characters[c];

                GLfloat xpos = xTemp + ch.Bearing.x * scale;
                GLfloat ypos = yTemp - (ch.Size.y - ch.Bearing.y) * scale;

                GLfloat w = ch.Size.x * scale;
                GLfloat h = ch.Size.y * scale;

                GLfloat vertices[6][4] =
                {
                    { xpos,     ypos + h,   0.0f, 0.0f },
                    { xpos,     ypos,       0.0f, 1.0f },
                    { xpos + w, ypos,       1.0f, 1.0f },

                    { xpos,     ypos + h,   0.0f, 0.0f },
                    { xpos + w, ypos,       1.0f, 1.0f },
                    { xpos + w, ypos + h,   1.0f, 0.0f }
                };

                glBindTexture(GL_TEXTURE_2D, ch.TextureID);

                // Bind the buffer and map its memory
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
                if (ptr) {
                    memcpy(ptr, vertices, sizeof(vertices));
                    glUnmapBuffer(GL_ARRAY_BUFFER);
                }
                else {
                    // Handle error
                    std::cerr << "Failed to map buffer" << std::endl;
                }
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                xTemp += (ch.Advance >> 6) * scale;
            }

            yTemp -= getHeightPerLine() * 1.5; // Move y position down by line height for the next line
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }


    // 표시되는 텍스트를 변경합니다.
    void setText(const std::u32string& text) 
    {
        this->text = text;
    }

    // 텍스트의 x좌표를 바꿉니다.
    void setX(GLfloat x) 
    {
        this->x = x;
    }

    // 텍스트의 x, y좌표를 바꿉니다.
    void setPos(GLfloat x, GLfloat y) {
        this->x = x;
        this->y = y;
    }

    // 텍스트의 x, y 좌표, scale을 바꿉니다.
    void setPos(GLfloat x, GLfloat y, GLfloat scale) {
        this->x = x;
        this->y = y;
        this->scale = scale;
    }

    void setColor(glm::vec3 color) {
		this->color = color;
	}
    
    GLfloat getTextWidth() {
		GLfloat textWidth = 0.f;
        for (auto c : text) {
            if (Characters.find(c) == Characters.end()) {
				Characters[c] = LoadCharacter(face, c);
			}
			Character ch = Characters[c];
			textWidth += (ch.Advance >> 6) * scale;
		}
        return textWidth;
	}

    GLfloat getHeightPerLine() {
        GLfloat textHeight = 0.f;
        for (auto c : text) {
            if (Characters.find(c) == Characters.end()) {
				Characters[c] = LoadCharacter(face, c);
			}
			Character ch = Characters[c];
			textHeight = std::max(textHeight, ch.Size.y * scale);
		}
        return textHeight;
    }

    // 텍스트의 y좌표를 바꿉니다.
    void setY(GLfloat y) {
        this->y = y;
    }

    // 텍스트의 scale을 바꿉니다.
    void setScale(GLfloat scale) {
        this->scale = scale;
    }

    // 텍스트를 빈 문자열로 바꿉니다.
    void clearText() {
        this->text = U"";
    }

private:
    Character LoadCharacter(FT_Face face, char32_t c) 
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "Failed to load Glyph: " << c << std::endl;
            return Character();
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };

        return character;
    }

    std::map<char32_t, Character> Characters;
    unsigned int VAO, VBO;
    glm::mat4 projectionMatrix;
    Shader shader;
    FT_Library ft;
    FT_Face face;

    std::u32string text;
    GLfloat x = 0.f;
    GLfloat y = 0.f;
    GLfloat scale = 1.f;
    glm::vec3 color;

    int lineNum;
};

#endif // TEXTRENDERER_H
