#include "game.h"

// Game-related State data
GameObject* Player;
Renderer* renderer;
Texture* awesomeface;
Texture* background;

Game::Game(GLuint width, GLuint height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
    delete renderer;
    delete Player;
    delete background;
    delete awesomeface;
}

void Game::Init()
{
    Shader ourShader("shaders/sprite.vs", "shaders/sprite.frag");
    renderer = new Renderer(ourShader);

    awesomeface = new Texture("textures/awesomeface.png", false);
    background = new Texture("textures/background.jpg", true);

    this->level = 0;
    GameLevel one;
    one.Load("levels/one.lvl", 800, 600 * 0.5);
    GameLevel two;
    two.Load("levels/two.lvl", 800, 600 * 0.5);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
}

void Game::Update(GLfloat dt)
{

}
void Game::ProcessInput(GLFWwindow* window, float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        GLfloat velocity = PLAYER_VELOCITY * dt;
        // Move playerboard
        if (this->Keys[GLFW_KEY_A])
        {
            if (Player->Position.x >= 0)
                Player->Position.x -= velocity;
        }
        if (this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
                Player->Position.x += velocity;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Game::Render()
{
    
    renderer->Draw(*background, glm::vec2(0, 0), glm::vec2(800, 600), 0.0f);
    this->Levels[this->level].Draw(*renderer);
    // Draw player
    //Player->Draw(*renderer);
}