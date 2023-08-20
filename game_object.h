#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
class GameObject {
public:
    // Object state
    glm::vec2   Position, Size, Velocity;
    glm::vec3   Color;
    GLfloat     Rotation;
    GLboolean   IsSolid;
    GLboolean   Destroyed;
    // Render state
    Texture   Sprite;
    // Constructor(s)
    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    // Draw sprite
   // virtual void Draw(SpriteRenderer& renderer);
};