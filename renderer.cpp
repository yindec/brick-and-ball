#include "renderer.h"

Renderer::Renderer(Shader& shader)
{
	this->shader = shader;
	this->initRenderData();
}

Renderer::~Renderer()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}
void Renderer::initRenderData() {

    GLfloat vertices[] = {
        // Î»ÖÃ     // ÎÆÀí
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::Draw(Texture& texture, glm::vec3 position,
    glm::vec2 size, GLfloat rotate, glm::vec3 color)
{
    // create transformations
    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    model = glm::translate(model, position);

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));
    //render container
    shader.use();
    this->shader.setMatrix4("model", model);
    this->shader.setVector3f("spriteColor", color);

    glm::mat4 projection = glm::ortho(0.0f, 800.f, 600.f, 0.0f, -1.0f, 1.0f);
    this->shader.setMatrix4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.ID);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::Draw(Texture& texture, glm::vec2 position,
    glm::vec2 size, GLfloat rotate, glm::vec3 color)
{
    // create transformations
    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    model = glm::translate(model, glm::vec3(position, 0.0));

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));
    //render container
    shader.use();
    this->shader.setMatrix4("model", model);
    this->shader.setVector3f("spriteColor", color);

    glm::mat4 projection = glm::ortho(0.0f, 800.f, 600.f, 0.0f, -1.0f, 1.0f);
    this->shader.setMatrix4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.ID);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}