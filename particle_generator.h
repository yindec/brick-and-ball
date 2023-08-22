#pragma once

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "texture.h"
#include "renderer.h"
#include "game_level.h"
#include "renderer.h"
#include "ball_object.h"
#include "particle_generator.h"

struct Particle
{
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	GLfloat Life;

	Particle()
		: Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
};

class ParticleGenerator
{
public:
	ParticleGenerator(GLuint mount);
	void Update(GLfloat dt, BallObject& Ball, GLuint newParticles, glm::vec2 offset);
	void Draw();
private:
	std::vector<Particle> Particles;
	GLuint mount;
	GLuint VAO;
	Shader shader;
	Texture texture;
	void init();
	int FirstUnusedParticle();
	void RespawnParticle(Particle &particle, BallObject &Ball, glm::vec2 offset);
};