#include "particle_generator.h"

ParticleGenerator::ParticleGenerator(GLuint mount)
{
	this->mount = mount;
	this->init();
}

void ParticleGenerator::init()
{
	for (GLuint i = 0; i < this->mount; ++i)
		this->Particles.push_back(Particle());

	GLuint VBO;
	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->VAO);  
	glGenBuffers(1, &VBO);

	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	this->shader = Shader("shaders/particle.vs", "shaders/particle.frag");
	this->texture = Texture("textures/particle.png", false);
}
void ParticleGenerator::Update(GLfloat dt, BallObject& Ball, GLuint newParticles, glm::vec2 offset)
{
	// Add new partciles
	for (GLuint i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->FirstUnusedParticle();
		this->RespawnParticle(this->Particles[unusedParticle], Ball, offset);
	}

	// Update all particles
	for (GLuint i = 0; i < this->mount; ++i)
	{
		Particle& p = this->Particles[i];
		p.Life -= dt;
		if (p.Life > 0.0f)
		{
			p.Position -= p.Velocity * dt;
			p.Color.a -= dt * 2.5;
		}
	}
}

void ParticleGenerator::Draw()
{
	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	this->shader.use();
	for (Particle& particle : this->Particles)
	{
		if (particle.Life > 0.0f)
		{
			glm::mat4 projection = glm::ortho(0.0f, 800.f, 600.f, 0.0f, -1.0f, 1.0f);
			this->shader.setMatrix4("projection", projection);
			this->shader.setVector2f("offset", particle.Position);
			this->shader.setVector4f("color", particle.Color);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->texture.ID);

			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

GLuint lastUsedParticle = 0;

int ParticleGenerator::FirstUnusedParticle()
{
	// Search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < this->mount; ++i)
	{
		if (this->Particles[i].Life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (GLuint i = 0; i < lastUsedParticle; ++i)
	{
		if (this->Particles[i].Life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	// Override first particle if all others are alive
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::RespawnParticle(Particle& particle, BallObject& Ball, glm::vec2 offset)
{
	GLfloat random = ((rand() % 100) - 50) / 10.0f;
	GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
	particle.Position = Ball.Position + random + offset;
	particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.Life = 1.0f;
	particle.Velocity = Ball.Velocity * 0.1f;
}