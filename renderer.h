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
	void Draw();
private:
	Shader shader;
	Texture texture1;
	Texture texture2;
	unsigned int VBO, VAO, EBO;
	void initRenderData();
};