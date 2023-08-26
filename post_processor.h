#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "renderer.h"
#include "shader.h"

class PostProcessor
{
public:
	// State
	Shader shader;
	Texture texture;
	GLuint Width, Height;
	// Options
	GLboolean Confuse, Chaos, Shake;
	PostProcessor(Shader shader, GLuint width, GLuint height);
	//
	void BeginRender();
	void EndRender();
	void Render(GLfloat time);
private:
	GLuint FBO;
	GLuint RBO;
	GLuint VAO;
};