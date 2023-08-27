#include "ball_object.h"

BallObject::BallObject()
	: GameObject(), Radius(12.5f), Stuck(true), Sticky(GL_FALSE), PassThrough(GL_FALSE) { }

BallObject::BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture texture)
	: GameObject(pos, glm::vec2(radius * 2, radius * 2), texture, glm::vec3(1.0f), velocity), 
	Radius(radius), Stuck(true), Sticky(GL_FALSE), PassThrough(GL_FALSE) { }

glm::vec2 BallObject::Move(GLfloat dt, GLuint window_width)
{
	// if ball is not stucked in paddle
	if (!this->Stuck)
	{
		// move ball
		this->Position += this->Velocity * dt;
		// check ball move to the edge of window, then reverse the velocity
		if (this->Position.x < 0.0f)
		{
			this->Velocity.x *= -1;
			this->Position.x = 0.0f;
		}
		else if (this->Position.x + this->Size.x >= window_width)
		{
			this->Velocity.x *= -1;
			this->Position.x = window_width - this->Size.x;
		}
		if (this->Position.y < 0.0f)
		{
			this->Velocity.y *= -1;
			this->Position.y = 0.0f;
		}
	}

	return this->Position;
}

// Resets the ball to initial Stuck Position (if ball is outside window bounds)
void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
	this->Position = position;
	this->Velocity = velocity;
	this->Stuck = true;
	this->Sticky = GL_FALSE;
	this->PassThrough = GL_FALSE;
}