#include "game.h"
#include <algorithm>

#include <irrklang/irrKlang.h>


void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);


using namespace irrklang;

// Game-related State data
GameObject* Player;
BallObject* Ball;
ParticleGenerator* Particles;
Renderer* renderer;
Texture* awesomeface;
Texture* background;
Texture* paddle;
Texture* tex_speed;
Texture* tex_sticky;
Texture* tex_pass;
Texture* tex_size;
Texture* tex_confuse;
Texture* tex_chaos;
TextRenderer* Text;

PostProcessor* Post;

ISoundEngine* SoundEngine = createIrrKlangDevice();
float ShakeTime = 0.0f;

GLboolean CheckCollision(GameObject& one, GameObject& two);
Collision CheckCollision(BallObject& one, GameObject& two);
Direction VectorDirection(glm::vec2 target);

Game::Game(GLuint width, GLuint height)
    : State(GAME_MENU), Keys(), Width(width), Height(height)
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

    SoundEngine->drop();
}

void Game::Init()
{
    Shader ourShader("shaders/sprite.vs", "shaders/sprite.frag");
    Shader textshader("shaders/text.vs", "shaders/text.frag");
    renderer = new Renderer(ourShader);

    awesomeface = new Texture("textures/awesomeface.png", false);
    background = new Texture("textures/background.jpg", true);
    paddle = new Texture("textures/paddle.png", false);
    tex_speed = new Texture("textures/powerup_speed.png", false);
    tex_sticky = new Texture("textures/powerup_sticky.png", false);
    tex_pass = new Texture("textures/powerup_passthrough.png", false);
    tex_size = new Texture("textures/powerup_increase.png", false);
    tex_confuse = new Texture("textures/powerup_confuse.png", false);
    tex_chaos = new Texture("textures/powerup_chaos.png", false);

    this->Lives = 3;
    this->level = 0;
    GameLevel one;
    GameLevel two;
    GameLevel three;
    GameLevel four;
    one.Load("levels/one.lvl", 800, 600 * 0.5);
    two.Load("levels/two.lvl", 800, 600 * 0.5);
    three.Load("levels/three.lvl", 800, 600 * 0.5);
    four.Load("levels/four.lvl", 800, 600 * 0.5);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    glm::vec2 playerPos = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
    Player = new GameObject(playerPos, PLAYER_SIZE, *paddle);

    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, *awesomeface);

    Particles = new ParticleGenerator(150);


    Shader screenShader("shaders/effects.vs", "shaders/effects.frag");
    
    Post = new PostProcessor(screenShader, this->Width, this->Height);

    // Audio
    SoundEngine->play2D("audio/breakout.mp3", GL_TRUE);
    Text = new TextRenderer(textshader, this->Width, this->Height);
}

void Game::Update(GLfloat dt)
{
    Ball->Move(dt, this->Width);

    this->DoCollisions();

    Particles->Update(dt, *Ball, 5, glm::vec2(Ball->Radius / 2));
    this->UpdatePowerUps(dt);
    if (ShakeTime > 0.0f)
    {
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f)
            Post->Shake = false;
    }

    if (Ball->Position.y >= this->Height)
    {
        --this->Lives;
        if (this->Lives == 0)
        {
            this->ResetLevel();
            this->State = GAME_MENU;
        }
        this->ResetPlayer();
    }

    if (this->State == GAME_ACTIVE && this->Levels[this->level].IsCompleted())
    {
        this->ResetLevel();
        this->ResetPlayer();
        Post->Chaos = GL_TRUE;
        this->State = GAME_WIN;
    }
 }
void Game::ProcessInput(GLFWwindow* window, float dt)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (this->State == GAME_ACTIVE)
    {
        GLfloat velocity = PLAYER_VELOCITY * dt;
        // Move playerboard
        if (Post->Confuse)
        {
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                if (Player->Position.x >= 0)
                    Player->Position.x += velocity;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                if (Player->Position.x <= this->Width - Player->Size.x)
                    Player->Position.x -= velocity;

            }
        }
        else {
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
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            Ball->Stuck = false;

        if (Ball->Stuck)
            Ball->Position.x = Player->Position.x + Player->Size.x * 0.5 - Ball->Radius;
    }
    if (this->State == GAME_MENU)
    {
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && !this->KeysProcessed[GLFW_KEY_ENTER])
        {
            this->State = GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !this->KeysProcessed[GLFW_KEY_W])
        {
            this->level = (this->level + 1) % 4;
            this->KeysProcessed[GLFW_KEY_W] = GL_TRUE;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !this->KeysProcessed[GLFW_KEY_S])
        {
            if (this->level > 0)
                --this->level;
            else
                this->level = 3;
            this->KeysProcessed[GLFW_KEY_S] = GL_TRUE;
        }
    }

    if (this->State == GAME_WIN)
    {
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
            Post->Chaos = GL_FALSE;
            this->State = GAME_MENU;
        }
    }
}

void Game::Render()
{
    if (this->State == GAME_ACTIVE || this->State == GAME_MENU)
    {
        Post->BeginRender();

        renderer->Draw(*background, glm::vec3(0, 0, -1), glm::vec2(800, 600), 0.0f);
        this->Levels[this->level].Draw(*renderer);
        // Draw player
        Player->Draw(*renderer);
        // Draw particles   
        Particles->Draw();
        Ball->Draw(*renderer);

        for (PowerUp& powerUp : this->PowerUps)
            if (!powerUp.Destroyed)
                powerUp.Draw(*renderer);

        Post->EndRender();
        Post->Render(glfwGetTime());

        std::stringstream ss; ss << this->Lives;
        Text->RenderText("Lives:" + ss.str(), 5.0f, 575.0f, 0.5f, glm::vec3(1.0, 1.0, 1.0));
    }
    if (this->State == GAME_MENU)
    {
        Text->RenderText("Press ENTER to start", 250.0f, this->Height / 2, 0.5f, glm::vec3(1.0, 1.0, 1.0));
        Text->RenderText("Press W or S to select level", 
            245.0f, this->Height / 2 - 20 , 0.4f, glm::vec3(1.0, 1.0, 1.0));
    }

    if (this->State == GAME_WIN)
    {
        Text->RenderText("You WON!!!", 320.0, Height / 2 - 20.0, 0.5f, glm::vec3(1.0, 1.0, 1.0));
        Text->RenderText("Press ENTER to retry or ESC to quit", 130.0, Height / 2, 0.5f, glm::vec3(1.0, 1.0, 1.0));
    }
}

void ActivatePowerUp(PowerUp& powerUp);

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
                {
                    brick.Destroyed = GL_TRUE;
                    this->SpawnPowerUps(brick);
                    SoundEngine->play2D("audio/bleep.mp3", GL_FALSE);
                }                    
                else
                {
                    ShakeTime = 0.05f;
                    Post->Shake = true;
                    SoundEngine->play2D("audio/solid.wav", GL_FALSE);
                }
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (!(Ball->PassThrough && !brick.IsSolid))
                {
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
        for (PowerUp& powerUp : this->PowerUps)
        {
            if (!powerUp.Destroyed)
            {
                if (powerUp.Position.y >= this->Height)
                    powerUp.Destroyed = GL_TRUE;
                if (CheckCollision(*Player, powerUp))
                {   // 道具与挡板接触，激活它！
                    ActivatePowerUp(powerUp);
                    powerUp.Destroyed = GL_TRUE;
                    powerUp.Activated = GL_TRUE;
                    SoundEngine->play2D("audio/powerup.wav", GL_FALSE);
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
        Ball->Stuck = Ball->Sticky;

        SoundEngine->play2D("audio/bleep.wav", GL_FALSE);
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
    this->Lives = 3;
}

void Game::ResetPlayer()
{
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
    // Also disable all active powerups
    
    Post->Chaos = Post->Confuse = GL_FALSE;
    Ball->PassThrough = Ball->Sticky = GL_FALSE;
    Player->Color = glm::vec3(1.0f);
    Ball->Color = glm::vec3(1.0f);
}


// PowerUps
GLboolean IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type);

void Game::UpdatePowerUps(GLfloat dt)
{
    for (PowerUp& powerUp : this->PowerUps)
    {
        powerUp.Position += powerUp.Velocity * dt;
        if (powerUp.Activated)
        {
            powerUp.Duration -= dt;

            if (powerUp.Duration <= 0.0f)
            {
                // 之后会将这个道具移除
                powerUp.Activated = GL_FALSE;
                // 停用效果
                if (powerUp.Type == "sticky")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "sticky"))
                    {   // 仅当没有其他sticky效果处于激活状态时重置，以下同理
                        Ball->Sticky = GL_FALSE;
                        Player->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "pass-through")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "pass-through"))
                    {
                        Ball->PassThrough = GL_FALSE;
                        Ball->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "confuse")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
                    {
                        Post->Confuse = GL_FALSE;
                    }
                }
                else if (powerUp.Type == "chaos")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
                    {
                        Post->Chaos = GL_FALSE;
                    }
                }
            }
        }
    }
    this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
        [](const PowerUp& powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
    ), this->PowerUps.end());
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

GLboolean CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
    // Collision x-axis?
    GLboolean collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // Collision y-axis?
    GLboolean collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // Collision only if on both axes
    return collisionX && collisionY;
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

void ActivatePowerUp(PowerUp& powerUp)
{
    // 根据道具类型发动道具
    if (powerUp.Type == "speed")
    {
        Ball->Velocity *= 1.2;
    }
    else if (powerUp.Type == "sticky")
    {
        Ball->Sticky = GL_TRUE;
        Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
    }
    else if (powerUp.Type == "pass-through")
    {
        Ball->PassThrough = GL_TRUE;
        Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
    }
    else if (powerUp.Type == "pad-size-increase")
    {
        Player->Size.x += 50;
    }
    else if (powerUp.Type == "confuse")
    {
        if (!Post->Chaos)
            Post->Confuse = GL_TRUE; // 只在chaos未激活时生效，chaos同理
    }
    else if (powerUp.Type == "chaos")
    {
        if (!Post->Confuse)
            Post->Chaos = GL_TRUE;
    }
}

GLboolean ShouldSpawn(GLuint chance)
{
    GLuint random = rand() % chance;
    return random == 0;
}

void Game::SpawnPowerUps(GameObject& block)
{
    if (ShouldSpawn(75)) // 1/75的几率
        this->PowerUps.push_back(
            PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, *tex_speed
            ));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(
            PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, *tex_sticky
            ));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(
            PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, *tex_pass
            ));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(
            PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, *tex_size
            ));
    if (ShouldSpawn(15)) // 负面道具被更频繁地生成
        this->PowerUps.push_back(
            PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 5.0f, block.Position, *tex_confuse
            ));
    if (ShouldSpawn(15))
        this->PowerUps.push_back(
            PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 5.0f, block.Position, *tex_chaos
            ));
}

GLboolean IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type)
{
    // Check if another PowerUp of the same type is still active
    // in which case we don't disable its effect (yet)
    for (const PowerUp& powerUp : powerUps)
    {
        if (powerUp.Activated)
            if (powerUp.Type == type)
                return GL_TRUE;
    }
    return GL_FALSE;
}

