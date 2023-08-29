#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <map>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "texture.h"
#include "shader.h"

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
    GLuint Advance;     // Horizontal offset to advance to next glyph
};


// A renderer class for rendering text displayed by a font loaded using the 
// FreeType library. A single font is loaded, processed into a list of Character
// items for later rendering.
class TextRenderer
{
public:
    // Holds a list of pre-compiled Characters
    std::map<GLchar, Character> Characters;
    // Shader used for text rendering
    Shader TextShader;
    // Constructor
    TextRenderer(Shader shader, GLuint width, GLuint height);

    // Renders a string of text using the precompiled list of characters
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));
private:
    // Render state
    GLuint VAO, VBO;
};