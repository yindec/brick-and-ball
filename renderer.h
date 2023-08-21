#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "texture.h"

class Renderer
{
public:
	Renderer(Shader& shader);
	~Renderer();
	void Draw(Texture& texture, glm::vec2 position,
		glm::vec2 size, GLfloat rotate, glm::vec3 color = glm::vec3(1.0f));
private:
	Shader shader;
	unsigned int VBO, VAO;
	void initRenderData();
};