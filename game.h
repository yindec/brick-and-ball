#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "texture.h"
#include "renderer.h"
#include "game_level.h"
#include "renderer.h"
#include "ball_object.h"

enum GameState { 
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100, 20);
// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.0f);

class Game
{
public:
    // 游戏状态
    GameState  State;
    GLboolean  Keys[1024];
    GLuint     Width, Height;
    std::vector<GameLevel> Levels;
    GLuint     level;
    
    // 构造函数/析构函数
    Game(GLuint width, GLuint height);
    ~Game();
    // 初始化游戏状态（加载所有的着色器/纹理/关卡）
    void Init();
    // 游戏循环
    void ProcessInput(GLFWwindow* window, float dt);
    void Update(GLfloat dt);
    void Render();
    void DoCollisions();
};