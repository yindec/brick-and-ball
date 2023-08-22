#include "game.h"

// Game-related State data
GameObject* Player;
BallObject* Ball;
ParticleGenerator* Particles;
Renderer* renderer;
Texture* awesomeface;
Texture* background;
Texture* paddle;

Collision CheckCollision(BallObject& one, GameObject& two);
Direction VectorDirection(glm::vec2 target);

Game::Game(GLuint width, GLuint height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
    delete renderer;
    delete Player;
    delete Ball;
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

    Particles = new ParticleGenerator(150);
}

void Game::Update(GLfloat dt)
{
    Ball->Move(dt, this->Width);

    this->DoCollisions();

    Particles->Update(dt, *Ball, 5, glm::vec2(Ball->Radius / 2));

    if (Ball->Position.y >= this->Height)
    {
        this->ResetLevel();
        this->ResetPlayer();
    }
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

        if (Ball->Stuck)
            Ball->Position.x = Player->Position.x + Player->Size.x * 0.5 - Ball->Radius;
    }
}

void Game::Render()
{
    renderer->Draw(*background, glm::vec2(0, 0), glm::vec2(800, 600), 0.0f);
    this->Levels[this->level].Draw(*renderer);
    // Draw player
    Player->Draw(*renderer);
    // Draw particles   
    Particles->Draw();
    Ball->Draw(*renderer);
}

void Game::DoCollisions()
{
    for (GameObject& brick : this->Levels[this->level].Bricks)
    {
        if (!brick.Destroyed)
        {
            Collision collision = CheckCollision(*Ball, brick);
            if (std::get<0>(collision))// 如果collision 是 true
            {
                if (!brick.IsSolid)
                    brick.Destroyed = GL_TRUE;
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT)
                {
                    Ball->Velocity.x *= -1;
                    GLfloat penetration = Ball->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT)
                        Ball->Position.x += penetration;
                    else
                        Ball->Position.x -= penetration; 
                }
                else // 垂直方向碰撞
                {
                    Ball->Velocity.y = -Ball->Velocity.y; // 反转垂直速度
                    // 重定位
                    GLfloat penetration = Ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP)
                        Ball->Position.y -= penetration; // 将球上移
                    else
                        Ball->Position.y += penetration; // 将球下移
                }
            }
        }
    }

    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result))
    {
        GLfloat centerBoard = Player->Position.x + Player->Size.x * 0.5;
        GLfloat distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        GLfloat percentage = distance / (Player->Size.x * 0.5);

        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * 2.0f;
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
    }
}

void Game::ResetLevel()
{
    if (this->level == 0)
        this->Levels[0].Load("levels/one.lvl", this->Width, this->Height * 0.5f);
    else if (this->level == 1)
        this->Levels[1].Load("levels/two.lvl", this->Width, this->Height * 0.5f);
    else if (this->level == 2)
        this->Levels[2].Load("levels/three.lvl", this->Width, this->Height * 0.5f);
    else if (this->level == 3)
        this->Levels[3].Load("levels/four.lvl", this->Width, this->Height * 0.5f);
}

void Game::ResetPlayer()
{
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
}


Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),  // 上
        glm::vec2(1.0f, 0.0f),  // 右
        glm::vec2(0.0f, -1.0f), // 下
        glm::vec2(-1.0f, 0.0f)  // 左
    };
    GLfloat max = 0.0f;
    GLuint best_match = -1;
    for (GLuint i = 0; i < 4; i++)
    {
        GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

Collision CheckCollision(BallObject& one, GameObject& two)
{
    glm::vec2 center(one.Position + one.Radius);
    glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
    glm::vec2 aabb_center(
        two.Position.x + aabb_half_extents.x,
        two.Position.y + aabb_half_extents.y
    );

    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

    glm::vec2 closet = aabb_center + clamped;
    difference = closet - center;

    if (glm::length(difference) < one.Radius)
        return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
    else
        return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
}