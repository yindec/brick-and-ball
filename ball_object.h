#pragma once
#include "game_object.h"

class BallObject : public GameObject
{
public:
	// propertity of ball
	GLfloat Radius;
	GLboolean Stuck;

	BallObject();
	BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture texture);

	glm::vec2 Move(GLfloat dt, GLuint window_width);
	void      Reset(glm::vec2 position, glm::vec2 velocity);
};