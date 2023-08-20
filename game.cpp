#include "game.h"

Game::Game(GLuint width, GLuint height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{

}

void Game::Init()
{

}

void Game::Update(GLfloat dt)
{

}
void Game::ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Game::Render()
{

}