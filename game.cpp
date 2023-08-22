#include "game.h"

// 初始化球的速度
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// 球的半径
const GLfloat BALL_RADIUS = 12.5f;

// Game-related State data
GameObject* Player;
BallObject* Ball;
Renderer* renderer;
Texture* awesomeface;
Texture* background;
Texture* paddle;

GLboolean CheckCollision(GameObject& one, GameObject& two);

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
    delete paddle;
}

void Game::Init()
{
    Shader ourShader("shaders/sprite.vs", "shaders/sprite.frag");
    renderer = new Renderer(ourShader);

    awesomeface = new Texture("textures/awesomeface.png", false);
    background = new Texture("textures/background.jpg", true);
    paddle = new Texture("textures/paddle.png", false);

    this->level = 0;
    GameLevel one;
    one.Load("levels/one.lvl", 800, 600 * 0.5);
    GameLevel two;
    two.Load("levels/two.lvl", 800, 600 * 0.5);
    this->Levels.push_back(one);
    this->Levels.push_back(two);

    glm::vec2 playerPos = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
    Player = new GameObject(playerPos, PLAYER_SIZE, *paddle);

    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, *awesomeface);
}

void Game::Update(GLfloat dt)
{
    Ball->Move(dt, this->Width);

    this->DoCollisions();
}
void Game::ProcessInput(GLFWwindow* window, float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        GLfloat velocity = PLAYER_VELOCITY * dt;
        // Move playerboard
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            if (Player->Position.x >= 0)
                Player->Position.x -= velocity;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
                Player->Position.x += velocity;
        }
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            Ball->Stuck = false;
    }
}

void Game::Render()
{
    renderer->Draw(*background, glm::vec2(0, 0), glm::vec2(800, 600), 0.0f);
    this->Levels[this->level].Draw(*renderer);
    // Draw player
    Player->Draw(*renderer);
    Ball->Draw(*renderer);
}

void Game::DoCollisions()
{
    for (GameObject& brick : this->Levels[this->level].Bricks)
    {
        if (!brick.Destroyed && CheckCollision(brick, *Ball) && !brick.IsSolid)
        {
            brick.Destroyed = GL_TRUE;
        }
    }
}

GLboolean CheckCollision(GameObject& one, GameObject& two)
{
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;

    return collisionX && collisionY;
}