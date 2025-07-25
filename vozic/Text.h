#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <map>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Character {
    unsigned int TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    unsigned int Advance;
};

extern std::map<char, Character> Characters;
extern unsigned int textVAO, textVBO;

bool InitFreeType(const char* fontPath);
void RenderText(unsigned int shaderProgram, const std::string& text, float x, float y, float scale, glm::vec3 color, unsigned int screenWidth, unsigned int screenHeight);
