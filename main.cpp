#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "sound.h"
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

int ard_3ard = 1280, tol_3ard = 720;
int locModel = -1, locTint = -1, locOffset = -1, locHighlight = -1, locGray = -1;
int gizmo = -1, wizzo = -1, fizzo = -1, hilo = -1;
unsigned int uiVAO = 0, biko = 0, ziro_vao = 0, menuVAO = 0;
int meshmesh = 0, falafel_indices = 0;
int menuIconStart = 24, menuIconCount = 42;
float uiRed = 0.0f;

struct Character {
    unsigned int TextureID;
    glm::ivec2   Size;
    glm::ivec2   Bearing;
    unsigned int Advance;
};

std::map<char, Character> Characters;
unsigned int textVAO = 0, textVBO = 0, textProgram = 0;
bool textReady = false;
unsigned int mainProgram = 0;

struct Mahshy { int id; float yStart, yEnd, centerY; };

struct Hawawshy {
    int laneId;
    float xPos;
    float spd;
    float yOffset;
    bool isJumping;
    bool isDucking;
    float jumpTimer;
    int health;
    float damageTint;
    int kind;
    float attackTimer;
};

class Koshary {
public:
    std::vector<Mahshy> tababa;
    float asfar = 0.5f, akhdar = 3.5f;
    Koshary() {
        tababa.push_back({ 0, 6.2f, 8.2f, 7.2f });
        tababa.push_back({ 1, 3.5f, 5.5f, 4.5f });
        tababa.push_back({ 2, 0.8f, 2.8f, 1.8f });
    }
    int getLaneFromY(float y) {
        for (auto& l : tababa) if (y >= l.yStart && y <= l.yEnd) return l.id;
        return -1;
    }
};

bool wPressed = false;
bool sPressed = false;
float playerSpeed = 1.5f;

Koshary soso;
std::vector<Hawawshy> basal;
std::vector<bool> tamtam(3, false);
int ziz = -1;
bool tot = true;

enum EnemyType { EASY_UPPER, MEDIUM_LOWER, HARD_MELEE };

struct Enemy {
    int id;
    EnemyType type;
    int laneId;
    float xPos;
    float yPos;
    int health;
    float speed;
    int damage;
    bool isAlive;
    float damageTint;
    bool blocked;
    float attackTimer;

Enemy(int enemyId, EnemyType t, int lane, float startX, float laneY) {
    id = enemyId;
    type = t;
    laneId = lane;
    xPos = startX;
    yPos = laneY;
    isAlive = true;
    damageTint = 0.0f;
    blocked = false;
    attackTimer = 0.0f;

    if (type == EASY_UPPER) {
        health = 5;
        speed = 0.8f;
        damage = 1;
    } else if (type == MEDIUM_LOWER) {
        health = 5;
        speed = 0.6f;
        damage = 1;
    } else if (type == HARD_MELEE) {
        health = 7;
        speed = 1.4f;
        damage = 2;
    }
}

    void update(float dt) {
        if (isAlive && !blocked) {
            xPos -= speed * dt;
        }
    }
};

struct Coin {
    float xPos;
    float yPos;
    bool collected;
};

std::vector<Enemy> enemies;
std::vector<Coin> coins;

struct Projectile {
    int laneId;
    float xPos;
    float height;
    float spd;
    bool fromPlayer;
    int attackType;
    float ignoreMinX;
    float ignoreMaxX;
};

std::vector<Projectile> projectiles;

std::vector<EnemyType> spawnQueue;
float spawnTimer = 0.0f;
float spawnInterval = 3.0f;
int nextEnemyId = 0;
int currentWave = 1;
bool waveInProgress = false;
int playerCoins = 30;
int cardCost[3] = { 10, 20, 30 };

bool isPaused = false;
bool inMainMenu = true;
bool isWin = false;
bool gameOver = false;
bool inCutscene = false;
float cutsceneTime = 0.0f;
int menuHover = -1;
int cardHover = -1;
float animTime = 0.0f;

int menuButtonAt(float x, float y);
int cardAt(float x, float y);
bool handleMenuClick(GLFWwindow* window, float wX, float wY);
void hurtEnemy(Enemy& e, int dmg);
void drawHealthBar(float cx, float y, int health, int maxHealth);

const char* kalmita ="#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 vertexColor;\n"
"uniform mat4 uProjection;\n"
"uniform mat4 model;\n"
"uniform vec2 uOffset;\n"
"void main(){\n"
"   gl_Position = uProjection * (model * vec4(aPos, 1.0) + vec4(uOffset, 0.0, 0.0));\n"
"   vertexColor = aColor;\n"
"}\n";

const char* shabash = "#version 330 core\n"
"in vec3 vertexColor;\n"
"out vec4 FragColor;\n"
"uniform vec3 uTint;\n"
"uniform float u_Highlight;\n"
"uniform float u_Gray;\n"
"void main(){\n"
"   vec3 baseColor = vertexColor * uTint;\n"
"   float grayValue = dot(baseColor, vec3(0.299, 0.587, 0.114));\n"
"   baseColor = mix(baseColor, vec3(grayValue), u_Gray);\n"
"   vec3 finalColor = mix(baseColor, vec3(1.0, 1.0, 0.0), u_Highlight);\n"
"   FragColor = vec4(finalColor, 1.0);\n"
"}\n";

void ftesh(unsigned int bombo, std::string no3) {
    int tamam_ya_fndm;
    char faza3[1024];
    if (no3 != "PROGRAM") {
        glGetShaderiv(bombo, GL_COMPILE_STATUS, &tamam_ya_fndm);
        if (!tamam_ya_fndm) {
            glGetShaderInfoLog(bombo, 1024, NULL, faza3);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << no3 << "\n" << faza3 << "\n";
        }
    }
    else {
        glGetProgramiv(bombo, GL_LINK_STATUS, &tamam_ya_fndm);
        if (!tamam_ya_fndm) {
            glGetProgramInfoLog(bombo, 1024, NULL, faza3);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR\n" << faza3 << "\n";
        }
    }
}

void el_shasha_betrg3(GLFWwindow* bobo, int ard, int tool) {
    if (tool == 0) tool = 1;
    ard_3ard = ard;
    tol_3ard = tool;
    glViewport(0, 0, ard, tool);
}

void uiBegin() {
    glBindVertexArray(uiVAO);
    glUniform2f(locOffset, 0.0f, 0.0f);
    glUniform1f(locHighlight, 0.0f);
    glUniform1f(locGray, 0.0f);
}

void uiEnd() {
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 1.0f, 1.0f, 1.0f);
    glUniform1f(locHighlight, 0.0f);
    glUniform1f(locGray, 0.0f);
}

GLFWwindow* initWindow() {
    if (!glfwInit()) return nullptr;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* hamada = glfwCreateWindow(ard_3ard, tol_3ard, "", NULL, NULL);
    if (!hamada) { glfwTerminate(); return nullptr; }
    glfwMakeContextCurrent(hamada);
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return nullptr;
}
    glfwGetFramebufferSize(hamada, &ard_3ard, &tol_3ard);
    glViewport(0, 0, ard_3ard, tol_3ard);
    return hamada;
}

void buildEnvironment() {
    std::vector<float> vertices = {
        0.5f, 0.5f, 0.0f,   0.32f, 0.35f, 0.42f,
        3.5f, 0.5f, 0.0f,   0.32f, 0.35f, 0.42f,
        3.5f, 8.5f, 0.0f,   0.32f, 0.35f, 0.42f,
        0.5f, 8.5f, 0.0f,   0.32f, 0.35f, 0.42f,
        1.6f, 0.5f, 0.0f,   0.15f, 0.15f, 0.18f,
        2.4f, 0.5f, 0.0f,   0.15f, 0.15f, 0.18f,
        2.4f, 2.7f, 0.0f,   0.15f, 0.15f, 0.18f,
        1.6f, 2.7f, 0.0f,   0.15f, 0.15f, 0.18f,
        1.2f, 6.5f, 0.0f,   0.2f, 0.22f, 0.28f,
        1.6f, 6.5f, 0.0f,   0.2f, 0.22f, 0.28f,
        1.6f, 7.3f, 0.0f,   0.2f, 0.22f, 0.28f,
        1.2f, 7.3f, 0.0f,   0.2f, 0.22f, 0.28f,
        2.4f, 6.5f, 0.0f,   0.2f, 0.22f, 0.28f,
        2.8000002f, 6.5f, 0.0f,   0.2f, 0.22f, 0.28f,
        2.8000002f, 7.3f, 0.0f,   0.2f, 0.22f, 0.28f,
        2.4f, 7.3f, 0.0f,   0.2f, 0.22f, 0.28f,
        0.5f, 8.5f, 0.0f,   0.22f, 0.25f, 0.3f,
        1.1f, 8.5f, 0.0f,   0.22f, 0.25f, 0.3f,
        1.1f, 8.9f, 0.0f,   0.22f, 0.25f, 0.3f,
        0.5f, 8.9f, 0.0f,   0.22f, 0.25f, 0.3f,
        1.3f, 8.5f, 0.0f,   0.22f, 0.25f, 0.3f,
        1.9f, 8.5f, 0.0f,   0.22f, 0.25f, 0.3f,
        1.9f, 8.9f, 0.0f,   0.22f, 0.25f, 0.3f,
        1.3f, 8.9f, 0.0f,   0.22f, 0.25f, 0.3f,
        2.1f, 8.5f, 0.0f,   0.22f, 0.25f, 0.3f,
        2.6999998f, 8.5f, 0.0f,   0.22f, 0.25f, 0.3f,
        2.6999998f, 8.9f, 0.0f,   0.22f, 0.25f, 0.3f,
        2.1f, 8.9f, 0.0f,   0.22f, 0.25f, 0.3f,
        2.9f, 8.5f, 0.0f,   0.22f, 0.25f, 0.3f,
        3.5f, 8.5f, 0.0f,   0.22f, 0.25f, 0.3f,
        3.5f, 8.9f, 0.0f,   0.22f, 0.25f, 0.3f,
        2.9f, 8.9f, 0.0f,   0.22f, 0.25f, 0.3f,
        3.5f, 6.2f, 0.0f,   0.2f, 0.6f, 0.9f,
        15.5f, 6.2f, 0.0f,   0.2f, 0.6f, 0.9f,
        15.5f, 8.2f, 0.0f,   0.2f, 0.6f, 0.9f,
        3.5f, 8.2f, 0.0f,   0.2f, 0.6f, 0.9f,
        3.5f, 3.5f, 0.0f,   0.2f, 0.6f, 0.9f,
        15.5f, 3.5f, 0.0f,   0.2f, 0.6f, 0.9f,
        15.5f, 5.5f, 0.0f,   0.2f, 0.6f, 0.9f,
        3.5f, 5.5f, 0.0f,   0.2f, 0.6f, 0.9f,
        3.5f, 0.8f, 0.0f,   0.2f, 0.6f, 0.9f,
        15.5f, 0.8f, 0.0f,   0.2f, 0.6f, 0.9f,
        15.5f, 2.8f, 0.0f,   0.2f, 0.6f, 0.9f,
        3.5f, 2.8f, 0.0f,   0.2f, 0.6f, 0.9f,
        5.5f, 8.2f, 0.0f,   0.45f, 0.3f, 0.15f,
        5.9f, 8.2f, 0.0f,   0.45f, 0.3f, 0.15f,
        5.9f, 9.4f, 0.0f,   0.45f, 0.3f, 0.15f,
        5.5f, 9.4f, 0.0f,   0.45f, 0.3f, 0.15f,
        5.0f, 9.0f, 0.0f,   0.15f, 0.55f, 0.2f,
        6.4f, 9.0f, 0.0f,   0.15f, 0.55f, 0.2f,
        6.4f, 9.8f, 0.0f,   0.15f, 0.55f, 0.2f,
        5.0f, 9.8f, 0.0f,   0.15f, 0.55f, 0.2f,
        5.2f, 9.6f, 0.0f,   0.18f, 0.65f, 0.22f,
        6.2f, 9.6f, 0.0f,   0.18f, 0.65f, 0.22f,
        6.2f, 10.200001f, 0.0f,   0.18f, 0.65f, 0.22f,
        5.2f, 10.200001f, 0.0f,   0.18f, 0.65f, 0.22f,
        8.0f, 8.2f, 0.0f,   0.3f, 0.3f, 0.3f,
        8.2f, 8.2f, 0.0f,   0.3f, 0.3f, 0.3f,
        8.2f, 9.8f, 0.0f,   0.3f, 0.3f, 0.3f,
        8.0f, 9.8f, 0.0f,   0.3f, 0.3f, 0.3f,
        7.0f, 9.0f, 0.0f,   0.75f, 0.15f, 0.15f,
        8.0f, 9.0f, 0.0f,   0.75f, 0.15f, 0.15f,
        8.0f, 9.7f, 0.0f,   0.75f, 0.15f, 0.15f,
        7.0f, 9.7f, 0.0f,   0.75f, 0.15f, 0.15f,
        7.3f, 9.2f, 0.0f,   0.95f, 0.85f, 0.2f,
        7.6000004f, 9.2f, 0.0f,   0.95f, 0.85f, 0.2f,
        7.6000004f, 9.5f, 0.0f,   0.95f, 0.85f, 0.2f,
        7.3f, 9.5f, 0.0f,   0.95f, 0.85f, 0.2f,
        11.0f, 8.2f, 0.0f,   0.35f, 0.25f, 0.15f,
        11.2f, 8.2f, 0.0f,   0.35f, 0.25f, 0.15f,
        11.2f, 9.2f, 0.0f,   0.35f, 0.25f, 0.15f,
        11.0f, 9.2f, 0.0f,   0.35f, 0.25f, 0.15f,
        10.8f, 9.1f, 0.0f,   0.25f, 0.25f, 0.28f,
        11.400001f, 9.1f, 0.0f,   0.25f, 0.25f, 0.28f,
        11.400001f, 9.400001f, 0.0f,   0.25f, 0.25f, 0.28f,
        10.8f, 9.400001f, 0.0f,   0.25f, 0.25f, 0.28f,
        10.95f, 9.4f, 0.0f,   0.95f, 0.5f, 0.1f,
        11.25f, 9.4f, 0.0f,   0.95f, 0.5f, 0.1f,
        11.25f, 9.9f, 0.0f,   0.95f, 0.5f, 0.1f,
        10.95f, 9.9f, 0.0f,   0.95f, 0.5f, 0.1f,
        11.05f, 9.5f, 0.0f,   0.95f, 0.9f, 0.2f,
        11.2f, 9.5f, 0.0f,   0.95f, 0.9f, 0.2f,
        11.2f, 9.8f, 0.0f,   0.95f, 0.9f, 0.2f,
        11.05f, 9.8f, 0.0f,   0.95f, 0.9f, 0.2f,
        13.5f, 8.2f, 0.0f,   0.4f, 0.43f, 0.52f,
        15.3f, 8.2f, 0.0f,   0.4f, 0.43f, 0.52f,
        15.3f, 9.7f, 0.0f,   0.4f, 0.43f, 0.52f,
        13.5f, 9.7f, 0.0f,   0.4f, 0.43f, 0.52f,
        13.5f, 9.7f, 0.0f,   0.28f, 0.3f, 0.38f,
        13.9f, 9.7f, 0.0f,   0.28f, 0.3f, 0.38f,
        13.9f, 10.0f, 0.0f,   0.28f, 0.3f, 0.38f,
        13.5f, 10.0f, 0.0f,   0.28f, 0.3f, 0.38f,
        14.2f, 9.7f, 0.0f,   0.28f, 0.3f, 0.38f,
        14.599999f, 9.7f, 0.0f,   0.28f, 0.3f, 0.38f,
        14.599999f, 10.0f, 0.0f,   0.28f, 0.3f, 0.38f,
        14.2f, 10.0f, 0.0f,   0.28f, 0.3f, 0.38f,
        14.9f, 9.7f, 0.0f,   0.28f, 0.3f, 0.38f,
        15.299999f, 9.7f, 0.0f,   0.28f, 0.3f, 0.38f,
        15.299999f, 10.0f, 0.0f,   0.28f, 0.3f, 0.38f,
        14.9f, 10.0f, 0.0f,   0.28f, 0.3f, 0.38f,
        14.2f, 8.8f, 0.0f,   0.18f, 0.2f, 0.25f,
        14.599999f, 8.8f, 0.0f,   0.18f, 0.2f, 0.25f,
        14.599999f, 9.400001f, 0.0f,   0.18f, 0.2f, 0.25f,
        14.2f, 9.400001f, 0.0f,   0.18f, 0.2f, 0.25f,
        3.5f, 0.0f, 0.0f,   0.35f, 0.38f, 0.45f,
        15.5f, 0.0f, 0.0f,   0.35f, 0.38f, 0.45f,
        15.5f, 0.8f, 0.0f,   0.35f, 0.38f, 0.45f,
        3.5f, 0.8f, 0.0f,   0.35f, 0.38f, 0.45f,
        4.0f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        4.6f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        4.6f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        4.0f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        5.2f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        5.7999997f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        5.7999997f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        5.2f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        6.3999996f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        6.9999995f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        6.9999995f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        6.3999996f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        7.5999994f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        8.2f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        8.2f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        7.5999994f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        8.799999f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        9.4f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        9.4f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        8.799999f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        9.999999f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        10.599999f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        10.599999f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        9.999999f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        11.199999f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        11.799999f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        11.799999f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        11.199999f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        12.399999f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        12.999999f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        12.999999f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        12.399999f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        13.599998f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        14.199999f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        14.199999f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        13.599998f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        14.799998f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        15.399999f, 0.8f, 0.0f,   0.28f, 0.3f, 0.38f,
        15.399999f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f,
        14.799998f, 1.2f, 0.0f,   0.28f, 0.3f, 0.38f
    };
    GLuint indices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20,
        24, 25, 26, 26, 27, 24,
        28, 29, 30, 30, 31, 28,
        32, 33, 34, 34, 35, 32,
        36, 37, 38, 38, 39, 36,
        40, 41, 42, 42, 43, 40,
        44, 45, 46, 46, 47, 44,
        48, 49, 50, 50, 51, 48,
        52, 53, 54, 54, 55, 52,
        56, 57, 58, 58, 59, 56,
        60, 61, 62, 62, 63, 60,
        64, 65, 66, 66, 67, 64,
        68, 69, 70, 70, 71, 68,
        72, 73, 74, 74, 75, 72,
        76, 77, 78, 78, 79, 76,
        80, 81, 82, 82, 83, 80,
        84, 85, 86, 86, 87, 84,
        88, 89, 90, 90, 91, 88,
        92, 93, 94, 94, 95, 92,
        96, 97, 98, 98, 99, 96,
        100, 101, 102, 102, 103, 100,
        104, 105, 106, 106, 107, 104,
        108, 109, 110, 110, 111, 108,
        112, 113, 114, 114, 115, 112,
        116, 117, 118, 118, 119, 116,
        120, 121, 122, 122, 123, 120,
        124, 125, 126, 126, 127, 124,
        128, 129, 130, 130, 131, 128,
        132, 133, 134, 134, 135, 132,
        136, 137, 138, 138, 139, 136,
        140, 141, 142, 142, 143, 140,
        144, 145, 146, 146, 147, 144
    };
    meshmesh = 48;
    int foul_indices = 18;
    falafel_indices = (int)(sizeof(indices) / sizeof(indices[0])) - meshmesh - foul_indices;

    unsigned int shiko, fiko;
    glGenVertexArrays(1, &biko);
    glGenBuffers(1, &shiko);
    glGenBuffers(1, &fiko);
    glBindVertexArray(biko);
    glBindBuffer(GL_ARRAY_BUFFER, shiko);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fiko);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

unsigned int buildShaderProgram() {
    unsigned int tigers = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(tigers, 1, &kalmita, NULL);
    glCompileShader(tigers);
    ftesh(tigers, "VERTEX");

    unsigned int chipsy = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(chipsy, 1, &shabash, NULL);
    glCompileShader(chipsy);
    ftesh(chipsy, "FRAGMENT");
    unsigned int dodo = glCreateProgram();
    glAttachShader(dodo, tigers);
    glAttachShader(dodo, chipsy);
    glLinkProgram(dodo);
    ftesh(dodo, "PROGRAM");
    glDeleteShader(tigers);
    glDeleteShader(chipsy);
    gizmo = glGetUniformLocation(dodo, "uProjection");
    wizzo = glGetUniformLocation(dodo, "uOffset");
    fizzo = glGetUniformLocation(dodo, "uTint");
    hilo = glGetUniformLocation(dodo, "u_Highlight");
    locModel = glGetUniformLocation(dodo, "model");
    locTint = fizzo;
    locOffset = wizzo;
    locHighlight = hilo;
    locGray = glGetUniformLocation(dodo, "u_Gray");
    return dodo;
}

void beginFrame(unsigned int program) {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    float left = 0.0f, right = 16.0f, bottom = 0.0f, top = 10.0f;
    float dolly[16] = {
        2.0f / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        -(right + left) / (right - left), -(top + bottom) / (top - bottom), 0.0f, 1.0f
    };
    glUniformMatrix4fv(gizmo, 1, GL_FALSE, dolly);
    glUniform2f(wizzo, 0.0f, 0.0f);
    uiEnd();
}

void drawEnvironment() {
    glBindVertexArray(biko);
    glUniform3f(fizzo, 1.0f, 1.0f, 1.0f);
    glDrawElements(GL_TRIANGLES, meshmesh, GL_UNSIGNED_INT, (void*)0);
    
    int laneIndexOffset = meshmesh;
    int indicesPerLane = 6;
    for (int i = 0; i < 3; i++) {
        if (tamtam[i]) {
            glUniform3f(fizzo, 0.4f, 1.2f, 0.5f);
        }
        else if (i == ziz) {
            glUniform3f(fizzo, 1.2f, 1.2f, 0.7f);
        }
        else {
            glUniform3f(fizzo, 1.0f, 1.0f, 1.0f);
        }
        glDrawElements(GL_TRIANGLES, indicesPerLane, GL_UNSIGNED_INT, (void*)((laneIndexOffset + i * indicesPerLane) * sizeof(unsigned int)));
    }

    glUniform3f(fizzo, 1.0f, 1.0f, 1.0f);
    int envIndexOffset = meshmesh + (3 * indicesPerLane);
    glDrawElements(GL_TRIANGLES, falafel_indices, GL_UNSIGNED_INT, (void*)(envIndexOffset * sizeof(unsigned int)));
}

const char* textVertexSrc = "#version 330 core\n"
"layout (location = 0) in vec4 vertex;\n"
"out vec2 TexCoords;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
"    TexCoords = vertex.zw;\n"
"}\n";

const char* textFragmentSrc = "#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"uniform sampler2D text;\n"
"uniform vec3 textColor;\n"
"void main()\n"
"{\n"
"    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
"    color = vec4(textColor, 1.0) * sampled;\n"
"}\n";

bool initText() {
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return false;
    }

    const char* fontPaths[] = {
        "fonts/DejaVuSans.ttf",
        "../fonts/DejaVuSans.ttf",
        "../Template/fonts/DejaVuSans.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
    };
    FT_Face face;
    bool loaded = false;
    for (const char* path : fontPaths) {
        if (!FT_New_Face(ft, path, 0, &face)) { loaded = true; break; }
    }
    if (!loaded)
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        FT_Done_FreeType(ft);
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (unsigned int)face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    unsigned int tv = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(tv, 1, &textVertexSrc, NULL);
    glCompileShader(tv);
    ftesh(tv, "VERTEX");
    unsigned int tf = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(tf, 1, &textFragmentSrc, NULL);
    glCompileShader(tf);
    ftesh(tf, "FRAGMENT");
    textProgram = glCreateProgram();
    glAttachShader(textProgram, tv);
    glAttachShader(textProgram, tf);
    glLinkProgram(textProgram);
    ftesh(textProgram, "PROGRAM");
    glDeleteShader(tv);
    glDeleteShader(tf);

    glm::mat4 projection = glm::ortho(0.0f, 16.0f, 0.0f, 10.0f);
    glUseProgram(textProgram);
    glUniformMatrix4fv(glGetUniformLocation(textProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return true;
}

void RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
    glUseProgram(textProgram);
    glUniform3f(glGetUniformLocation(textProgram, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(mainProgram);
}

float textWidth(const std::string& text, float scale) {
    float w = 0.0f;
    for (size_t i = 0; i < text.size(); i++) w += (Characters[text[i]].Advance >> 6) * scale;
    return w;
}

void drawText(const std::string& text, float cx, float y, float em, glm::vec3 color) {
    float scale = em / 48.0f;
    RenderText(text, cx - textWidth(text, scale) / 2.0f, y, scale, color);
}

float triSign(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool pointInTriangle(glm::vec2 pt, glm::vec2 a, glm::vec2 b, glm::vec2 c) {
    float d1 = triSign(pt, a, b);
    float d2 = triSign(pt, b, c);
    float d3 = triSign(pt, c, a);

    bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(hasNeg && hasPos);
}

void processInput(GLFWwindow* window) {
    wPressed = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
    sPressed = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
}

void el_mouse_dakhal(GLFWwindow* bobo, int dakhal) {
    tot = dakhal;
    if (!dakhal) ziz = -1;

}

void el_mouse_beteeth(GLFWwindow* tabl, double mash, double mosh) {
    if (!tot || tol_3ard == 0) return;
    int winW, winH;
    glfwGetWindowSize(tabl, &winW, &winH);
    if (winW == 0 || winH == 0) return;
    float wX = (float)mash / winW * 16.0f;
    float wY = (1.0f - (float)mosh / winH) * 10.0f;
    if (inCutscene) return;
    if (inMainMenu || isPaused || gameOver || isWin) {
        menuHover = menuButtonAt(wX, wY);
        return;
    }
    cardHover = cardAt(wX, wY);
    if (wX > soso.akhdar) {
        ziz = soso.getLaneFromY(wY);
    }
    else {
        ziz = -1;
    }
}

void el_mouse_dos(GLFWwindow* tabl, int b, int a, int m) {
    if (b != GLFW_MOUSE_BUTTON_LEFT || a != GLFW_PRESS || tol_3ard == 0) return;
    double mash, mosh;
    glfwGetCursorPos(tabl, &mash, &mosh);
    int winW, winH;
    glfwGetWindowSize(tabl, &winW, &winH);
    if (winW == 0 || winH == 0) return;
    float wX = (float)mash / winW * 16.0f;
    float wY = (1.0f - (float)mosh / winH) * 10.0f;
    if (handleMenuClick(tabl, wX, wY)) return;
    int card = cardAt(wX, wY);
    if (card != -1) {
        bool placed = false;
        for (size_t i = 0; i < tamtam.size(); ++i) {
            if (tamtam[i] && playerCoins >= cardCost[card]) {
                basal.push_back({ (int)i, soso.akhdar, card == 2 ? 0.0f : playerSpeed, 0.0f, false, false, 0.0f, card == 2 ? 7 : 5, 0.0f, card, 0.0f });
                playerCoins -= cardCost[card];
                placed = true;
            }
        }
        if (placed) playSound(SND_PLACE);
        return;
    }
    if (!(wX >= soso.asfar && wX <= soso.akhdar)) {
        int nawwar = soso.getLaneFromY(wY);
        if (nawwar != -1) {
            tamtam[nawwar] = !tamtam[nawwar];
            playSound(SND_CLICK);
        }
    }
}

void updatePlayerActions(GLFWwindow* window, float dt) {
    processInput(window);
    for (auto& p : basal) {
        bool laneSelected = tamtam[p.laneId] && p.kind != 2;
        if (laneSelected && wPressed && !p.isJumping) {
            p.isJumping = true;
            p.jumpTimer = 0.0f;
            playSound(SND_JUMP);
        }
        p.isDucking = laneSelected && sPressed;
        if (p.isJumping) {
            p.jumpTimer += dt * 5.0f;
            p.yOffset = 0.6f * sin(p.jumpTimer);
            if (p.jumpTimer >= 3.14159f) {
                p.isJumping = false;
                p.yOffset = 0.0f;
            }
        }
    }
}

void advancePlayers(float dt) {
    for (auto it = basal.begin(); it != basal.end();) {
        it->xPos += it->spd * dt;
        if (it->xPos > 16.5f) it = basal.erase(it);
        else ++it;
    }
}

void updateCombat(float dt) {
    for (auto& e : enemies) {
        e.blocked = false;
        if (e.damageTint > 0.0f) e.damageTint -= dt * 3.0f;
        if (e.attackTimer > 0.0f) e.attackTimer -= dt;
    }
    for (auto& p : basal) {
        if (p.damageTint > 0.0f) p.damageTint -= dt * 3.0f;
        if (p.attackTimer > 0.0f) p.attackTimer -= dt;
    }

    for (auto& p : basal) {
        if (p.kind == 1 && p.attackTimer <= 0.0f) {
            projectiles.push_back({ p.laneId, p.xPos + 0.2f, 0.42f, 6.0f, true, 2, -100.0f, -100.0f });
            p.attackTimer = 1.2f;
            playSound(SND_SHOOT);
        }
    }
    for (auto& e : enemies) {
        if (!e.isAlive || e.type == HARD_MELEE || e.xPos > 15.5f || e.attackTimer > 0.0f) continue;
        bool high = (e.type == EASY_UPPER);
        projectiles.push_back({ e.laneId, e.xPos - 0.2f, high ? 0.8f : 0.15f, -5.0f, false, high ? 0 : 1, -100.0f, -100.0f });
        e.attackTimer = high ? 2.0f : 2.4f;
        playSound(SND_ENEMY_SHOOT);
    }

    for (auto it = projectiles.begin(); it != projectiles.end();) {
        it->xPos += it->spd * dt;
        bool remove = false;
        if (it->fromPlayer) {
            for (auto& e : enemies) {
                if (!e.isAlive || e.laneId != it->laneId || e.xPos > 16.0f) continue;
                if (std::abs(it->xPos - e.xPos) < 0.35f) {
                    hurtEnemy(e, 1);
                    remove = true;
                    break;
                }
            }
            if (it->xPos > 16.5f) remove = true;
        }
        else {
            for (auto& p : basal) {
                if (p.laneId != it->laneId) continue;
                if (it->xPos >= it->ignoreMinX && it->xPos <= it->ignoreMaxX) continue;
                if (std::abs(it->xPos - p.xPos) < 0.3f) {
                    bool dodged = false;
                    if (p.kind != 2) {
                        if (it->attackType == 0 && p.isDucking) dodged = true;
                        if (it->attackType == 1 && p.isJumping) dodged = true;
                    }
                    if (dodged) {
                        it->ignoreMinX = p.xPos - 0.3f;
                        it->ignoreMaxX = p.xPos + 0.3f;
                        continue;
                    }
                    p.health -= 1;
                    p.damageTint = 0.5f;
                    playSound(SND_HURT);
                    remove = true;
                    break;
                }
            }
            if (it->xPos < soso.akhdar) remove = true;
        }
        if (remove) it = projectiles.erase(it);
        else ++it;
    }

    for (auto& p : basal) {
        bool isColliding = false;
        for (auto& e : enemies) {
            if (!e.isAlive || e.laneId != p.laneId) continue;
            if (std::abs(p.xPos - e.xPos) >= 0.6f) continue;
            isColliding = true;
            e.blocked = true;
            if (p.kind == 0 && p.attackTimer <= 0.0f) {
                hurtEnemy(e, 1);
                p.attackTimer = 0.5f;
            }
            if (e.type == HARD_MELEE && e.attackTimer <= 0.0f) {
                p.health -= e.damage;
                p.damageTint = 0.5f;
                playSound(SND_HURT);
                e.attackTimer = 0.6f;
            }
        }
        p.spd = (isColliding || p.kind == 2) ? 0.0f : playerSpeed;
    }

    for (auto it = basal.begin(); it != basal.end();) {
        if (it->health <= 0) {
            playSound(SND_DEATH);
            it = basal.erase(it);
        }
        else ++it;
    }
}

float redFor(int health, int maxHealth, float flash) {
    float frac = (float)health / (float)maxHealth;
    if (frac < 0.0f) frac = 0.0f;
    float red = (1.0f - frac) * 0.65f + (flash > 0.0f ? flash * 0.5f : 0.0f);
    return red > 0.85f ? 0.85f : red;
}

void drawGuard(float x, float y, float lift, int kind = 0) {
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x - 0.15f, y - 0.15f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f, 0.15f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.2f + (1.0f - 0.2f) * uiRed, 0.2f * (1.0f - uiRed), 0.25f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x + 0.05f, y - 0.15f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f, 0.15f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.2f + (1.0f - 0.2f) * uiRed, 0.2f * (1.0f - uiRed), 0.25f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x - 0.18f, y, 0.0f));
    model = glm::scale(model, glm::vec3(0.36f, 0.5f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.2f + (1.0f - 0.2f) * uiRed, 0.35f * (1.0f - uiRed), 0.8f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x - 0.18f, y + 0.42f, 0.0f));
    model = glm::scale(model, glm::vec3(0.36f, 0.08f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.85f + (1.0f - 0.85f) * uiRed, 0.7f * (1.0f - uiRed), 0.2f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y + 0.66f, 0.0f));
    model = glm::scale(model, glm::vec3(0.15f, 0.15f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.95f + (1.0f - 0.95f) * uiRed, 0.8f * (1.0f - uiRed), 0.65f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLE_FAN, 15, 34);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x - 0.17f, y + 0.7f, 0.0f));
    model = glm::scale(model, glm::vec3(0.34f, 0.12f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.6f + (1.0f - 0.6f) * uiRed, 0.6f * (1.0f - uiRed), 0.65f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x - 0.05f, y + 0.82f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f, 0.12f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.8f + (1.0f - 0.8f) * uiRed, 0.1f * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 6, 3);
    }
    if (kind == 0) {
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x - 0.38f, y + 0.12f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.28f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.6f + (1.0f - 0.6f) * uiRed, 0.6f * (1.0f - uiRed), 0.65f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x - 0.38f, y - 0.02f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.14f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.6f + (1.0f - 0.6f) * uiRed, 0.6f * (1.0f - uiRed), 0.65f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 12, 3);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.2f, y + 0.25f + lift, 0.0f));
        model = glm::scale(model, glm::vec3(0.06f, 0.5f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.85f + (1.0f - 0.85f) * uiRed, 0.85f * (1.0f - uiRed), 0.9f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.13f, y + 0.22f + lift, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.05f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.5f + (1.0f - 0.5f) * uiRed, 0.35f * (1.0f - uiRed), 0.15f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
    else if (kind == 1) {
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.26f, y + 0.12f + lift * 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(0.05f, 0.62f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.5f + (1.0f - 0.5f) * uiRed, 0.32f * (1.0f - uiRed), 0.12f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.2f, y + 0.12f + lift * 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.05f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.5f + (1.0f - 0.5f) * uiRed, 0.32f * (1.0f - uiRed), 0.12f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.2f, y + 0.69f + lift * 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.05f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.5f + (1.0f - 0.5f) * uiRed, 0.32f * (1.0f - uiRed), 0.12f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.2f, y + 0.15f + lift * 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(0.015f, 0.57f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.9f + (1.0f - 0.9f) * uiRed, 0.9f * (1.0f - uiRed), 0.85f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.2f, y + 0.42f + lift * 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.03f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.8f + (1.0f - 0.8f) * uiRed, 0.8f * (1.0f - uiRed), 0.75f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.7f, y + 0.38f + lift * 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(0.12f, 0.11f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.85f + (1.0f - 0.85f) * uiRed, 0.85f * (1.0f - uiRed), 0.9f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 9, 3);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x - 0.25f, y + 0.5f, 0.0f));
        model = glm::scale(model, glm::vec3(0.07f, 0.3f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.45f + (1.0f - 0.45f) * uiRed, 0.28f * (1.0f - uiRed), 0.12f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x - 0.3f, y + 0.5f, 0.0f));
        model = glm::scale(model, glm::vec3(0.17f, 0.04f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.3f + (1.0f - 0.3f) * uiRed, 0.5f * (1.0f - uiRed), 0.25f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
    else {
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.06f, y - 0.1f, 0.0f));
        model = glm::scale(model, glm::vec3(0.38f, 0.92f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.45f + (1.0f - 0.45f) * uiRed, 0.47f * (1.0f - uiRed), 0.55f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.1f, y - 0.06f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f, 0.84f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.65f + (1.0f - 0.65f) * uiRed, 0.67f * (1.0f - uiRed), 0.72f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.23f, y + 0.1f, 0.0f));
        model = glm::scale(model, glm::vec3(0.04f, 0.56f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.85f + (1.0f - 0.85f) * uiRed, 0.7f * (1.0f - uiRed), 0.2f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.14f, y + 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(0.22f, 0.04f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.85f + (1.0f - 0.85f) * uiRed, 0.7f * (1.0f - uiRed), 0.2f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.06f, y - 0.22f, 0.0f));
        model = glm::scale(model, glm::vec3(0.38f, 0.14f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.45f + (1.0f - 0.45f) * uiRed, 0.47f * (1.0f - uiRed), 0.55f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 12, 3);
        }
    }
}

void drawPlayerUnit(const Hawawshy& p) {
    int maxHp = p.kind == 2 ? 7 : 5;
    float duck = p.isDucking ? -0.12f : 0.0f;
    float base = soso.tababa[p.laneId].centerY - 0.55f + p.yOffset + duck;
    float swing = 0.0f;
    if (p.kind == 0) swing = 0.15f * sin(animTime * (p.spd == 0.0f ? 30.0f : 12.0f));
    else if (p.kind == 1 && p.spd == 0.0f) swing = 0.15f * sin(animTime * 30.0f);
    uiRed = redFor(p.health, maxHp, p.damageTint);
    drawGuard(p.xPos, base, swing, p.kind);
    uiRed = 0.0f;
    drawHealthBar(p.xPos, base + 1.05f, p.health, maxHp);
}

void spawnWave(int waveNumber, const Koshary& grid) {
    enemies.clear();
    spawnQueue.clear();

    if (waveNumber == 1) {
        for (int i = 0; i < 3; i++) spawnQueue.push_back(EASY_UPPER);
        spawnInterval = 3.5f;
    } else if (waveNumber == 2) {
        for (int i = 0; i < 5; i++) spawnQueue.push_back((i % 2 == 0) ? EASY_UPPER : MEDIUM_LOWER);
        spawnInterval = 3.0f;
    } else if (waveNumber == 3) {
        for (int i = 0; i < 10; i++) spawnQueue.push_back((i % 3 == 0) ? EASY_UPPER : ((i % 3 == 1) ? MEDIUM_LOWER : HARD_MELEE));
        spawnInterval = 2.2f;
    }
    spawnTimer = 0.0f;
    waveInProgress = true;
}

void updateSpawner(float dt) {
    if (spawnQueue.empty()) return;
    spawnTimer -= dt;
    if (spawnTimer > 0.0f) return;
    EnemyType type = spawnQueue.front();
    spawnQueue.erase(spawnQueue.begin());
    int lane = rand() % (int)soso.tababa.size();
    enemies.push_back(Enemy(nextEnemyId++, type, lane, 16.0f, soso.tababa[lane].centerY));
    spawnTimer = spawnInterval;
}

void hurtEnemy(Enemy& e, int dmg) {
    e.health -= dmg;
    e.damageTint = 0.5f;
    if (e.health <= 0 && e.isAlive) {
        e.isAlive = false;
        Coin drop;
        drop.xPos = e.xPos;
        drop.yPos = e.yPos;
        drop.collected = false;
        coins.push_back(drop);
        playSound(SND_DEATH);
    }
    else {
        playSound(SND_HIT);
    }
}

void startWaveIfNeeded() {
    if (!waveInProgress && currentWave <= 3) {
        spawnWave(currentWave, soso);
    }
}

void updateEnemiesAndWaves(float dt) {
    bool allDefeated = true;
    for (auto& enemy : enemies) {
        if (!enemy.isAlive) continue;
        enemy.update(dt);
        allDefeated = false;

        if (enemy.xPos <= 3.5f) {
            if (!gameOver) playSound(SND_LOSE);
            gameOver = true;
        }
    }

    for (auto& coin : coins) {
        if (coin.collected) continue;
        for (const auto& p : basal) {
            float py = soso.tababa[p.laneId].centerY;
            if (std::abs(coin.xPos - p.xPos) < 0.6f && std::abs(coin.yPos - py) < 0.6f) {
                coin.collected = true;
                playSound(SND_COIN);
                playerCoins += 10;
                break;
            }
        }
    }

    if (!spawnQueue.empty()) allDefeated = false;

    if (allDefeated && waveInProgress) {
        waveInProgress = false;
        currentWave++;
        if (currentWave > 3) {
            isWin = true;
            playSound(SND_WIN);
        }
    }
}

void buildCoinMesh() {
    std::vector<float> quadVertices = {
        -0.2f, -0.2f, 0.0f,   0.95f, 0.85f, 0.2f,
        0.2f, -0.2f, 0.0f,   0.95f, 0.85f, 0.2f,
        0.2f, 0.2f, 0.0f,   0.95f, 0.85f, 0.2f,
        -0.2f, 0.2f, 0.0f,   0.95f, 0.85f, 0.2f
    };
    GLuint quadIndices[] = { 0, 1, 2, 2, 3, 0 };
    unsigned int ziko, ziro_ebo;
    glGenVertexArrays(1, &ziro_vao);
    glGenBuffers(1, &ziko);
    glGenBuffers(1, &ziro_ebo);
    glBindVertexArray(ziro_vao);
    glBindBuffer(GL_ARRAY_BUFFER, ziko);
    glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), quadVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ziro_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void drawCoins() {
    glBindVertexArray(ziro_vao);

    for (const auto& coin : coins) {
        if (!coin.collected) {

            glUniform3f(fizzo, 1.0f, 0.85f, 0.0f);

            glUniform2f(wizzo, coin.xPos, coin.yPos);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        }
    }
}

void drawEnemyActor(float x, float y, float lift, int variant = 2) {
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x - 0.16f, y - 0.15f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f, 0.15f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.25f + (1.0f - 0.25f) * uiRed, 0.1f * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x + 0.06f, y - 0.15f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f, 0.15f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.25f + (1.0f - 0.25f) * uiRed, 0.1f * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x - 0.2f, y, 0.0f));
    model = glm::scale(model, glm::vec3(0.4f, 0.55f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.55f + (1.0f - 0.55f) * uiRed, 0.12f * (1.0f - uiRed), 0.12f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y + 0.72f, 0.0f));
    model = glm::scale(model, glm::vec3(0.17f, 0.17f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.35f + (1.0f - 0.35f) * uiRed, 0.55f * (1.0f - uiRed), 0.25f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLE_FAN, 15, 34);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x - 0.17f, y + 0.8f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f, 0.2f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.9f + (1.0f - 0.9f) * uiRed, 0.9f * (1.0f - uiRed), 0.8f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 6, 3);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x + 0.07f, y + 0.8f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f, 0.2f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.9f + (1.0f - 0.9f) * uiRed, 0.9f * (1.0f - uiRed), 0.8f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 6, 3);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x - 0.09f, y + 0.74f, 0.0f));
    model = glm::scale(model, glm::vec3(0.03f, 0.03f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 1.0f + (1.0f - 1.0f) * uiRed, 0.9f * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLE_FAN, 15, 34);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x + 0.01f, y + 0.74f, 0.0f));
    model = glm::scale(model, glm::vec3(0.03f, 0.03f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 1.0f + (1.0f - 1.0f) * uiRed, 0.9f * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLE_FAN, 15, 34);
    }
    if (variant == 0) {
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x - 0.34f, y + 0.55f + lift * 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(0.06f, 0.5f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.45f + (1.0f - 0.45f) * uiRed, 0.28f * (1.0f - uiRed), 0.12f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x - 0.4f, y + 0.95f + lift * 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(0.18f, 0.05f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.45f + (1.0f - 0.45f) * uiRed, 0.28f * (1.0f - uiRed), 0.12f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x - 0.4f, y + 0.55f + lift * 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(0.18f, 0.05f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.45f + (1.0f - 0.45f) * uiRed, 0.28f * (1.0f - uiRed), 0.12f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x - 0.78f, y + 0.77f + lift * 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(0.48f, 0.03f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.8f + (1.0f - 0.8f) * uiRed, 0.8f * (1.0f - uiRed), 0.75f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x - 0.78f, y + 0.725f + lift * 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(-0.12f, 0.11f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.85f + (1.0f - 0.85f) * uiRed, 0.85f * (1.0f - uiRed), 0.9f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 9, 3);
        }
    }
    else if (variant == 1) {
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x - 0.62f, y + 0.12f + lift * 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(0.46f, 0.06f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.45f + (1.0f - 0.45f) * uiRed, 0.28f * (1.0f - uiRed), 0.12f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x - 0.64f, y + 0.0f + lift * 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(0.05f, 0.3f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.35f + (1.0f - 0.35f) * uiRed, 0.22f * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x - 0.9f, y + 0.14f + lift * 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(0.28f, 0.03f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.8f + (1.0f - 0.8f) * uiRed, 0.8f * (1.0f - uiRed), 0.75f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x - 0.9f, y + 0.095f + lift * 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(-0.12f, 0.11f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.85f + (1.0f - 0.85f) * uiRed, 0.85f * (1.0f - uiRed), 0.9f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 9, 3);
        }
    }
    else {
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x - 0.36f, y + 0.15f + lift, 0.0f));
        model = glm::scale(model, glm::vec3(0.08f, 0.5f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.45f + (1.0f - 0.45f) * uiRed, 0.28f * (1.0f - uiRed), 0.12f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x - 0.3f, y + 0.5f, 0.0f));
        model = glm::scale(model, glm::vec3(0.12f, 0.16f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.75f + (1.0f - 0.75f) * uiRed, 0.75f * (1.0f - uiRed), 0.8f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 6, 3);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.1f, y + 0.5f, 0.0f));
        model = glm::scale(model, glm::vec3(0.12f, 0.16f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.75f + (1.0f - 0.75f) * uiRed, 0.75f * (1.0f - uiRed), 0.8f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 6, 3);
        }
    }
}

void drawProjectile(const Projectile& pr) {
    float base = soso.tababa[pr.laneId].centerY - 0.55f;
    float y = base + pr.height;
    if (pr.fromPlayer) {
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(pr.xPos - 0.3f, y - 0.02f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.04f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.85f + (1.0f - 0.85f) * uiRed, 0.85f * (1.0f - uiRed), 0.8f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(pr.xPos + 0.2f, y - 0.08f, 0.0f));
        model = glm::scale(model, glm::vec3(0.16f, 0.16f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.95f + (1.0f - 0.95f) * uiRed, 0.95f * (1.0f - uiRed), 1.0f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 9, 3);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(pr.xPos - 0.34f, y - 0.07f, 0.0f));
        model = glm::scale(model, glm::vec3(0.07f, 0.14f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.2f + (1.0f - 0.2f) * uiRed, 0.7f * (1.0f - uiRed), 0.3f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
    else {
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(pr.xPos - 0.2f, y - 0.02f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.04f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.55f + (1.0f - 0.55f) * uiRed, 0.3f * (1.0f - uiRed), 0.15f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(pr.xPos - 0.2f, y - 0.08f, 0.0f));
        model = glm::scale(model, glm::vec3(-0.16f, 0.16f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.95f + (1.0f - 0.95f) * uiRed, 0.15f * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 9, 3);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(pr.xPos + 0.3f, y - 0.07f, 0.0f));
        model = glm::scale(model, glm::vec3(0.07f, 0.14f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.95f + (1.0f - 0.95f) * uiRed, 0.55f * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
}

void drawEnemyUnit(const Enemy& e) {
    int maxHp = e.type == HARD_MELEE ? 7 : 5;
    float base = e.yPos - 0.55f;
    float swing = 0.0f;
    if (e.type == HARD_MELEE) swing = -0.15f * sin(animTime * (e.blocked ? 30.0f : 14.0f));
    else if (e.blocked) swing = -0.15f * sin(animTime * 30.0f);
    uiRed = redFor(e.health, maxHp, e.damageTint);
    drawEnemyActor(e.xPos, base, swing, (int)e.type);
    uiRed = 0.0f;
    drawHealthBar(e.xPos, base + 1.05f, e.health, maxHp);
}

int menuButtonAt(float x, float y) {
    glm::vec2 pt(x, y);
    if (pointInTriangle(pt, glm::vec2(5.5f, 6.0f), glm::vec2(10.5f, 6.0f), glm::vec2(10.5f, 7.4f)) ||
        pointInTriangle(pt, glm::vec2(10.5f, 7.4f), glm::vec2(5.5f, 7.4f), glm::vec2(5.5f, 6.0f))) return 0;
    if (pointInTriangle(pt, glm::vec2(5.5f, 4.1f), glm::vec2(10.5f, 4.1f), glm::vec2(10.5f, 5.5f)) ||
        pointInTriangle(pt, glm::vec2(10.5f, 5.5f), glm::vec2(5.5f, 5.5f), glm::vec2(5.5f, 4.1f))) return 1;
    if (pointInTriangle(pt, glm::vec2(5.5f, 2.2f), glm::vec2(10.5f, 2.2f), glm::vec2(10.5f, 3.6f)) ||
        pointInTriangle(pt, glm::vec2(10.5f, 3.6f), glm::vec2(5.5f, 3.6f), glm::vec2(5.5f, 2.2f))) return 2;
    if (pointInTriangle(pt, glm::vec2(0.4f, 0.4f), glm::vec2(1.4f, 0.4f), glm::vec2(1.4f, 1.4f)) ||
        pointInTriangle(pt, glm::vec2(1.4f, 1.4f), glm::vec2(0.4f, 1.4f), glm::vec2(0.4f, 0.4f))) return 3;
    return -1;
}

int cardAt(float x, float y) {
    glm::vec2 pt(x, y);
    for (int i = 0; i < 3; i++) {
        float x0 = 3.7f + i * 1.15f, y0 = 8.35f, x1 = x0 + 1.05f, y1 = y0 + 1.55f;
        if (pointInTriangle(pt, glm::vec2(x0, y0), glm::vec2(x1, y0), glm::vec2(x1, y1)) ||
            pointInTriangle(pt, glm::vec2(x1, y1), glm::vec2(x0, y1), glm::vec2(x0, y0))) return i;
    }
    return -1;
}

void toggleMute() {
    muted = !muted;
    playSound(SND_CLICK);
}
void dos_zrar(GLFWwindow* tabl, int nono, int zaza, int pika, int mizo) {
    if (nono == GLFW_KEY_M && pika == GLFW_PRESS) toggleMute();
    if (inCutscene) {
        if (nono == GLFW_KEY_ESCAPE && pika == GLFW_PRESS) inCutscene = false;
        return;
    }
    if (inMainMenu || gameOver || isWin) return;

    if (nono == GLFW_KEY_ESCAPE && pika == GLFW_PRESS) {
        isPaused = !isPaused;
        playSound(SND_CLICK);
    }
    if (isPaused) return;
}

void restartGame() {
    enemies.clear();
    coins.clear();
    projectiles.clear();
    spawnQueue.clear();
    spawnTimer = 0.0f;
    nextEnemyId = 0;
    animTime = 0.0f;
    basal.clear();
    for (size_t i = 0; i < tamtam.size(); ++i) tamtam[i] = false;
    currentWave = 1;
    playerCoins = 30;
    waveInProgress = false;
    gameOver = false;
    isWin = false;
    isPaused = false;
    inCutscene = false;
    menuHover = -1;
    cardHover = -1;
}

bool handleMenuClick(GLFWwindow* window, float wX, float wY) {
    if (inCutscene) {
        inCutscene = false;
        return true;
    }
    if (inMainMenu) {
        int hit = menuButtonAt(wX, wY);
        if (hit == 1 || hit == 3) { toggleMute(); return true; }
        if (hit != -1) playSound(SND_CLICK);
        if (hit == 0) { inMainMenu = false; inCutscene = true; cutsceneTime = 0.0f; }
        else if (hit == 2) glfwSetWindowShouldClose(window, true);
        return true;
    }
    if (isWin || gameOver) {
        int hit = menuButtonAt(wX, wY);
        if (hit == 3) { toggleMute(); return true; }
        if (hit == 0 || hit == 1) playSound(SND_CLICK);
        if (hit == 0) restartGame();
        else if (hit == 1) { restartGame(); inMainMenu = true; }
        return true;
    }
    if (isPaused) {
        int hit = menuButtonAt(wX, wY);
        if (hit == 3) { toggleMute(); return true; }
        if (hit != -1) playSound(SND_CLICK);
        if (hit == 0) isPaused = false;
        else if (hit == 1) restartGame();
        else if (hit == 2) { restartGame(); inMainMenu = true; }
        return true;
    }
    return false;
}

std::string cutsceneLine(float t) {
    if (t >= 0.5f && t < 4.5f) return "Guard: This gate is our home. We must protect it!";
    if (t >= 4.5f && t < 7.0f) return "Raider: We will take your gate!";
    if (t >= 7.0f && t < 9.5f) return "Guard: Not while I stand!";
    if (t >= 9.5f && t < 13.5f) return "The battle for the gate begins!";
    if (t >= 13.5f) return "Guard: More are coming. Hold the line!";
    return "";
}

void buildMenuMesh() {
    std::vector<float> menuVertices = {
        4.90f, 1.70f, 0.0f,   0.18f, 0.20f, 0.28f,
        11.10f, 1.70f, 0.0f,   0.18f, 0.20f, 0.28f,
        11.10f, 8.00f, 0.0f,   0.18f, 0.20f, 0.28f,
        11.10f, 8.00f, 0.0f,   0.18f, 0.20f, 0.28f,
        4.90f, 8.00f, 0.0f,   0.18f, 0.20f, 0.28f,
        4.90f, 1.70f, 0.0f,   0.18f, 0.20f, 0.28f,
        5.50f, 6.00f, 0.0f,   0.20f, 0.60f, 0.25f,
        10.50f, 6.00f, 0.0f,   0.20f, 0.60f, 0.25f,
        10.50f, 7.40f, 0.0f,   0.20f, 0.60f, 0.25f,
        10.50f, 7.40f, 0.0f,   0.20f, 0.60f, 0.25f,
        5.50f, 7.40f, 0.0f,   0.20f, 0.60f, 0.25f,
        5.50f, 6.00f, 0.0f,   0.20f, 0.60f, 0.25f,
        5.50f, 4.10f, 0.0f,   0.20f, 0.40f, 0.80f,
        10.50f, 4.10f, 0.0f,   0.20f, 0.40f, 0.80f,
        10.50f, 5.50f, 0.0f,   0.20f, 0.40f, 0.80f,
        10.50f, 5.50f, 0.0f,   0.20f, 0.40f, 0.80f,
        5.50f, 5.50f, 0.0f,   0.20f, 0.40f, 0.80f,
        5.50f, 4.10f, 0.0f,   0.20f, 0.40f, 0.80f,
        5.50f, 2.20f, 0.0f,   0.75f, 0.20f, 0.20f,
        10.50f, 2.20f, 0.0f,   0.75f, 0.20f, 0.20f,
        10.50f, 3.60f, 0.0f,   0.75f, 0.20f, 0.20f,
        10.50f, 3.60f, 0.0f,   0.75f, 0.20f, 0.20f,
        5.50f, 3.60f, 0.0f,   0.75f, 0.20f, 0.20f,
        5.50f, 2.20f, 0.0f,   0.75f, 0.20f, 0.20f,
        7.70f, 6.20f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.70f, 7.20f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.50f, 6.70f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.55f, 4.30f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.80f, 4.30f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.80f, 4.70f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.80f, 4.70f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.55f, 4.70f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.55f, 4.30f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.95f, 4.30f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.20f, 4.30f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.20f, 5.00f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.20f, 5.00f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.95f, 5.00f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.95f, 4.30f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.35f, 4.30f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.60f, 4.30f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.60f, 5.30f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.60f, 5.30f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.35f, 5.30f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.35f, 4.30f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.25f, 2.40f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.80f, 2.40f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.80f, 3.40f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.80f, 3.40f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.25f, 3.40f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.25f, 2.40f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.33f, 2.48f, 0.0f,   0.75f, 0.20f, 0.20f,
        7.72f, 2.48f, 0.0f,   0.75f, 0.20f, 0.20f,
        7.72f, 3.32f, 0.0f,   0.75f, 0.20f, 0.20f,
        7.72f, 3.32f, 0.0f,   0.75f, 0.20f, 0.20f,
        7.33f, 3.32f, 0.0f,   0.75f, 0.20f, 0.20f,
        7.33f, 2.48f, 0.0f,   0.75f, 0.20f, 0.20f,
        7.95f, 2.82f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.40f, 2.82f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.40f, 2.98f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.40f, 2.98f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.95f, 2.98f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.95f, 2.82f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.40f, 2.70f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.40f, 3.10f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.75f, 2.90f, 0.0f,   1.00f, 1.00f, 1.00f,
        4.90f, 1.70f, 0.0f,   0.18f, 0.20f, 0.28f,
        11.10f, 1.70f, 0.0f,   0.18f, 0.20f, 0.28f,
        11.10f, 8.00f, 0.0f,   0.18f, 0.20f, 0.28f,
        11.10f, 8.00f, 0.0f,   0.18f, 0.20f, 0.28f,
        4.90f, 8.00f, 0.0f,   0.18f, 0.20f, 0.28f,
        4.90f, 1.70f, 0.0f,   0.18f, 0.20f, 0.28f,
        5.50f, 6.00f, 0.0f,   0.20f, 0.60f, 0.25f,
        10.50f, 6.00f, 0.0f,   0.20f, 0.60f, 0.25f,
        10.50f, 7.40f, 0.0f,   0.20f, 0.60f, 0.25f,
        10.50f, 7.40f, 0.0f,   0.20f, 0.60f, 0.25f,
        5.50f, 7.40f, 0.0f,   0.20f, 0.60f, 0.25f,
        5.50f, 6.00f, 0.0f,   0.20f, 0.60f, 0.25f,
        5.50f, 4.10f, 0.0f,   0.20f, 0.40f, 0.80f,
        10.50f, 4.10f, 0.0f,   0.20f, 0.40f, 0.80f,
        10.50f, 5.50f, 0.0f,   0.20f, 0.40f, 0.80f,
        10.50f, 5.50f, 0.0f,   0.20f, 0.40f, 0.80f,
        5.50f, 5.50f, 0.0f,   0.20f, 0.40f, 0.80f,
        5.50f, 4.10f, 0.0f,   0.20f, 0.40f, 0.80f,
        5.50f, 2.20f, 0.0f,   0.80f, 0.50f, 0.15f,
        10.50f, 2.20f, 0.0f,   0.80f, 0.50f, 0.15f,
        10.50f, 3.60f, 0.0f,   0.80f, 0.50f, 0.15f,
        10.50f, 3.60f, 0.0f,   0.80f, 0.50f, 0.15f,
        5.50f, 3.60f, 0.0f,   0.80f, 0.50f, 0.15f,
        5.50f, 2.20f, 0.0f,   0.80f, 0.50f, 0.15f,
        7.70f, 6.20f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.70f, 7.20f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.50f, 6.70f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.95f, 4.30f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.95f, 5.30f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.45f, 4.80f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.55f, 4.30f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.55f, 5.30f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.05f, 4.80f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.60f, 2.40f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.40f, 2.40f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.40f, 3.00f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.40f, 3.00f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.60f, 3.00f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.60f, 2.40f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.45f, 3.00f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.55f, 3.00f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.00f, 3.45f, 0.0f,   1.00f, 1.00f, 1.00f,
        4.90f, 3.80f, 0.0f,   0.12f, 0.45f, 0.20f,
        11.10f, 3.80f, 0.0f,   0.12f, 0.45f, 0.20f,
        11.10f, 8.00f, 0.0f,   0.12f, 0.45f, 0.20f,
        11.10f, 8.00f, 0.0f,   0.12f, 0.45f, 0.20f,
        4.90f, 8.00f, 0.0f,   0.12f, 0.45f, 0.20f,
        4.90f, 3.80f, 0.0f,   0.12f, 0.45f, 0.20f,
        4.90f, 3.80f, 0.0f,   0.50f, 0.12f, 0.12f,
        11.10f, 3.80f, 0.0f,   0.50f, 0.12f, 0.12f,
        11.10f, 8.00f, 0.0f,   0.50f, 0.12f, 0.12f,
        11.10f, 8.00f, 0.0f,   0.50f, 0.12f, 0.12f,
        4.90f, 8.00f, 0.0f,   0.50f, 0.12f, 0.12f,
        4.90f, 3.80f, 0.0f,   0.50f, 0.12f, 0.12f,
        5.50f, 6.00f, 0.0f,   0.20f, 0.40f, 0.80f,
        10.50f, 6.00f, 0.0f,   0.20f, 0.40f, 0.80f,
        10.50f, 7.40f, 0.0f,   0.20f, 0.40f, 0.80f,
        10.50f, 7.40f, 0.0f,   0.20f, 0.40f, 0.80f,
        5.50f, 7.40f, 0.0f,   0.20f, 0.40f, 0.80f,
        5.50f, 6.00f, 0.0f,   0.20f, 0.40f, 0.80f,
        5.50f, 4.10f, 0.0f,   0.80f, 0.50f, 0.15f,
        10.50f, 4.10f, 0.0f,   0.80f, 0.50f, 0.15f,
        10.50f, 5.50f, 0.0f,   0.80f, 0.50f, 0.15f,
        10.50f, 5.50f, 0.0f,   0.80f, 0.50f, 0.15f,
        5.50f, 5.50f, 0.0f,   0.80f, 0.50f, 0.15f,
        5.50f, 4.10f, 0.0f,   0.80f, 0.50f, 0.15f,
        7.95f, 6.20f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.95f, 7.20f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.45f, 6.70f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.55f, 6.20f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.55f, 7.20f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.05f, 6.70f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.60f, 4.20f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.40f, 4.20f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.40f, 4.80f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.40f, 4.80f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.60f, 4.80f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.60f, 4.20f, 0.0f,   1.00f, 1.00f, 1.00f,
        7.45f, 4.80f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.55f, 4.80f, 0.0f,   1.00f, 1.00f, 1.00f,
        8.00f, 5.25f, 0.0f,   1.00f, 1.00f, 1.00f
    };

    
    

    unsigned int menuVBO;
    glGenVertexArrays(1, &menuVAO);
    glGenBuffers(1, &menuVBO);
    glBindVertexArray(menuVAO);
    glBindBuffer(GL_ARRAY_BUFFER, menuVBO);
    glBufferData(GL_ARRAY_BUFFER, menuVertices.size() * sizeof(float), menuVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void buildUiMesh() {
    std::vector<float> uiVertices = {
        0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        0.5f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        1.0f, 0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f
    };
    for (int i = 0; i <= 32; i++) {
        float angle = i * 2.0f * 3.14159265f / 32.0f;
        uiVertices.push_back(cos(angle));
        uiVertices.push_back(sin(angle));
        uiVertices.push_back(0.0f);
        uiVertices.push_back(1.0f);
        uiVertices.push_back(1.0f);
        uiVertices.push_back(1.0f);
    }

    unsigned int uiVBO;
    glGenVertexArrays(1, &uiVAO);
    glGenBuffers(1, &uiVBO);
    glBindVertexArray(uiVAO);
    glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
    glBufferData(GL_ARRAY_BUFFER, uiVertices.size() * sizeof(float), uiVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void drawMenuLabels(const char* title, const char* a, const char* b, const char* c) {
    glm::vec3 white(1.0f, 1.0f, 1.0f);
    drawText(title, 8.0f, 7.58f, 0.5f, glm::vec3(1.0f, 0.85f, 0.3f));
    drawText(a, 8.0f, 6.5f, 0.55f, white);
    drawText(b, 8.0f, 4.6f, 0.55f, white);
    if (c) drawText(c, 8.0f, 2.7f, 0.55f, white);
    drawText(muted ? "SOUND OFF (M)" : "SOUND ON (M)", 3.1f, 0.78f, 0.32f, white);
}

void drawMainMenuText() {
    if (!textReady) return;
    drawMenuLabels("TOWER DEFENSE", "START", muted ? "SOUND: OFF" : "SOUND: ON", "END");
}

void drawEndOrPauseText() {
    if (!textReady) return;
    if (isWin) drawMenuLabels("VICTORY!", "RESTART", "MAIN MENU", nullptr);
    else if (gameOver) drawMenuLabels("DEFEAT!", "RESTART", "MAIN MENU", nullptr);
    else drawMenuLabels("PAUSED", "CONTINUE", "RESTART", "MAIN MENU");
}

void drawHudText() {
    if (!textReady) return;
    int wave = currentWave > 3 ? 3 : currentWave;
    drawText("COINS " + std::to_string(playerCoins), 10.95f, 9.3f, 0.42f, glm::vec3(1.0f, 0.85f, 0.2f));
    drawText("WAVE " + std::to_string(wave) + " OF 3", 10.95f, 8.75f, 0.34f, glm::vec3(0.9f, 0.9f, 0.95f));
    for (int i = 0; i < 3; i++) {
        bool unlocked = playerCoins >= cardCost[i];
        float cx = 3.7f + i * 1.15f + 0.525f;
        glm::vec3 col = unlocked ? glm::vec3(1.0f, 0.85f, 0.2f) : glm::vec3(0.6f, 0.6f, 0.6f);
        drawText(std::to_string(cardCost[i]), cx, 8.48f, 0.34f, col);
    }
}

void drawCutsceneText(float t) {
    if (!textReady) return;
    std::string line = cutsceneLine(t);
    if (!line.empty()) drawText(line, 8.0f, 0.27f, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void drawMuteButton() {
    uiBegin();
    glUniform1f(locHighlight, menuHover == 3 ? 1.0f : 0.0f);
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.9f, 0.9f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, (muted ? 0.3f : 0.15f) + (1.0f - (muted ? 0.3f : 0.15f)) * uiRed, (muted ? 0.3f : 0.6f) * (1.0f - uiRed), (muted ? 0.33f : 0.25f) * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLE_FAN, 15, 34);
    }
    float shade = muted ? 0.55f : 1.0f;
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.62f, 0.7f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f, 0.2f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, shade + (1.0f - shade) * uiRed, shade * (1.0f - uiRed), shade * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.82f, 0.7f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f, 0.4f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, shade + (1.0f - shade) * uiRed, shade * (1.0f - uiRed), shade * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.02f, 0.7f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f, 0.6f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, shade + (1.0f - shade) * uiRed, shade * (1.0f - uiRed), shade * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    if (muted) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.5f, 0.84f, 0.0f));
    model = glm::scale(model, glm::vec3(0.8f, 0.1f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.9f + (1.0f - 0.9f) * uiRed, 0.1f * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    uiEnd();
}

void drawCoinTracker() {
    if (textReady) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(9.6f, 8.6f, 0.0f));
    model = glm::scale(model, glm::vec3(2.7f, 1.35f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.12f + (1.0f - 0.12f) * uiRed, 0.12f * (1.0f - uiRed), 0.16f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(12.4f, 8.6f, 0.0f));
    model = glm::scale(model, glm::vec3(3.55f, 1.35f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.12f + (1.0f - 0.12f) * uiRed, 0.12f * (1.0f - uiRed), 0.16f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    int shown = playerCoins / 10;
    for (int i = 0; i < 8; i++) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(12.75f + i * 0.4f, 9.65f, 0.0f));
    model = glm::scale(model, glm::vec3(0.14f, 0.14f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.2f + (1.0f - 0.2f) * uiRed, 0.2f * (1.0f - uiRed), 0.24f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLE_FAN, 15, 34);
    }
    for (int i = 0; i < shown && i < 24; i++) {
        float cx = 12.75f + (i % 8) * 0.4f;
        float cy = 9.65f - (i / 8) * 0.38f;
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(cx, cy, 0.0f));
        model = glm::scale(model, glm::vec3(0.14f, 0.14f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 1.0f + (1.0f - 1.0f) * uiRed, 0.8f * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLE_FAN, 15, 34);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(cx, cy, 0.0f));
        model = glm::scale(model, glm::vec3(0.085f, 0.085f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.9f + (1.0f - 0.9f) * uiRed, 0.62f * (1.0f - uiRed), 0.05f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLE_FAN, 15, 34);
        }
    }
}

void drawCards() {
    float cardColor[3][3] = { { 0.70f, 0.45f, 0.20f }, { 0.15f, 0.55f, 0.55f }, { 0.45f, 0.50f, 0.60f } };
    for (int i = 0; i < 3; i++) {
        float x0 = 3.7f + i * 1.15f, y0 = 8.35f, cx = x0 + 0.525f;
        bool unlocked = playerCoins >= cardCost[i];
        glUniform1f(locGray, unlocked ? 0.0f : 1.0f);
        glUniform1f(locHighlight, (unlocked && i == cardHover) ? 1.0f : 0.0f);
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x0, y0, 0.0f));
        model = glm::scale(model, glm::vec3(1.05f, 1.55f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, (cardColor[i][0]) + (1.0f - (cardColor[i][0])) * uiRed, (cardColor[i][1]) * (1.0f - uiRed), (cardColor[i][2]) * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glUniform1f(locHighlight, 0.0f);
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x0 + 0.06f, y0 + 0.06f, 0.0f));
        model = glm::scale(model, glm::vec3(0.93f, 0.4f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.1f + (1.0f - 0.1f) * uiRed, 0.1f * (1.0f - uiRed), 0.12f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        if (i == 0) {
            {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(cx - 0.04f, y0 + 0.55f, 0.0f));
            model = glm::scale(model, glm::vec3(0.08f, 0.17f, 1.0f));
            glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
            glUniform3f(locTint, 0.45f + (1.0f - 0.45f) * uiRed, 0.28f * (1.0f - uiRed), 0.12f * (1.0f - uiRed));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(cx - 0.22f, y0 + 0.72f, 0.0f));
            model = glm::scale(model, glm::vec3(0.44f, 0.08f, 1.0f));
            glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
            glUniform3f(locTint, 0.85f + (1.0f - 0.85f) * uiRed, 0.75f * (1.0f - uiRed), 0.3f * (1.0f - uiRed));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(cx - 0.05f, y0 + 0.8f, 0.0f));
            model = glm::scale(model, glm::vec3(0.1f, 0.55f, 1.0f));
            glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
            glUniform3f(locTint, 0.9f + (1.0f - 0.9f) * uiRed, 0.9f * (1.0f - uiRed), 0.95f * (1.0f - uiRed));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(cx - 0.05f, y0 + 1.35f, 0.0f));
            model = glm::scale(model, glm::vec3(0.1f, 0.13f, 1.0f));
            glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
            glUniform3f(locTint, 0.9f + (1.0f - 0.9f) * uiRed, 0.9f * (1.0f - uiRed), 0.95f * (1.0f - uiRed));
            glDrawArrays(GL_TRIANGLES, 6, 3);
            }
        }
        else if (i == 1) {
            {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(cx - 0.35f, y0 + 0.98f, 0.0f));
            model = glm::scale(model, glm::vec3(0.5f, 0.06f, 1.0f));
            glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
            glUniform3f(locTint, 0.75f + (1.0f - 0.75f) * uiRed, 0.55f * (1.0f - uiRed), 0.3f * (1.0f - uiRed));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(cx + 0.15f, y0 + 0.87f, 0.0f));
            model = glm::scale(model, glm::vec3(0.22f, 0.28f, 1.0f));
            glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
            glUniform3f(locTint, 0.9f + (1.0f - 0.9f) * uiRed, 0.9f * (1.0f - uiRed), 0.95f * (1.0f - uiRed));
            glDrawArrays(GL_TRIANGLES, 9, 3);
            }
            {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(cx - 0.42f, y0 + 1.04f, 0.0f));
            model = glm::scale(model, glm::vec3(0.14f, 0.13f, 1.0f));
            glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
            glUniform3f(locTint, 0.95f + (1.0f - 0.95f) * uiRed, 0.95f * (1.0f - uiRed), 0.95f * (1.0f - uiRed));
            glDrawArrays(GL_TRIANGLES, 6, 3);
            }
            {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(cx - 0.42f, y0 + 0.85f, 0.0f));
            model = glm::scale(model, glm::vec3(0.14f, 0.13f, 1.0f));
            glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
            glUniform3f(locTint, 0.95f + (1.0f - 0.95f) * uiRed, 0.95f * (1.0f - uiRed), 0.95f * (1.0f - uiRed));
            glDrawArrays(GL_TRIANGLES, 12, 3);
            }
        }
        else {
            {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(cx - 0.27f, y0 + 0.95f, 0.0f));
            model = glm::scale(model, glm::vec3(0.54f, 0.45f, 1.0f));
            glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
            glUniform3f(locTint, 0.9f + (1.0f - 0.9f) * uiRed, 0.9f * (1.0f - uiRed), 0.95f * (1.0f - uiRed));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(cx - 0.27f, y0 + 0.62f, 0.0f));
            model = glm::scale(model, glm::vec3(0.54f, 0.33f, 1.0f));
            glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
            glUniform3f(locTint, 0.9f + (1.0f - 0.9f) * uiRed, 0.9f * (1.0f - uiRed), 0.95f * (1.0f - uiRed));
            glDrawArrays(GL_TRIANGLES, 12, 3);
            }
            {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(cx - 0.04f, y0 + 0.75f, 0.0f));
            model = glm::scale(model, glm::vec3(0.08f, 0.55f, 1.0f));
            glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
            glUniform3f(locTint, (cardColor[i][0]) + (1.0f - (cardColor[i][0])) * uiRed, (cardColor[i][1]) * (1.0f - uiRed), (cardColor[i][2]) * (1.0f - uiRed));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(cx - 0.17f, y0 + 1.08f, 0.0f));
            model = glm::scale(model, glm::vec3(0.34f, 0.08f, 1.0f));
            glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
            glUniform3f(locTint, (cardColor[i][0]) + (1.0f - (cardColor[i][0])) * uiRed, (cardColor[i][1]) * (1.0f - uiRed), (cardColor[i][2]) * (1.0f - uiRed));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
        int pips = textReady ? 0 : cardCost[i] / 10;
        for (int k = 0; k < pips; k++) {
            float px = x0 + 0.525f + (k - (pips - 1) / 2.0f) * 0.28f;
            {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(px, y0 + 0.26f, 0.0f));
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 1.0f));
            glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
            glUniform3f(locTint, 1.0f + (1.0f - 1.0f) * uiRed, 0.8f * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
            glDrawArrays(GL_TRIANGLE_FAN, 15, 34);
            }
            {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(px, y0 + 0.26f, 0.0f));
            model = glm::scale(model, glm::vec3(0.06f, 0.06f, 1.0f));
            glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
            glUniform3f(locTint, 0.9f + (1.0f - 0.9f) * uiRed, 0.62f * (1.0f - uiRed), 0.05f * (1.0f - uiRed));
            glDrawArrays(GL_TRIANGLE_FAN, 15, 34);
            }
        }
    }
    glUniform1f(locGray, 0.0f);
}

void drawHealthBar(float cx, float y, int health, int maxHealth) {
    float frac = (float)health / (float)maxHealth;
    if (frac < 0.0f) frac = 0.0f;
    if (frac > 1.0f) frac = 1.0f;
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(cx - 0.32f, y, 0.0f));
    model = glm::scale(model, glm::vec3(0.64f, 0.1f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.1f + (1.0f - 0.1f) * uiRed, 0.1f * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(cx - 0.3f, y + 0.02f, 0.0f));
    model = glm::scale(model, glm::vec3(0.6f * frac, 0.06f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, (1.0f - frac) + (1.0f - (1.0f - frac)) * uiRed, frac * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void drawBubble(float x, float y, int icon) {
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(0.9f, 0.6f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 1.0f + (1.0f - 1.0f) * uiRed, 1.0f * (1.0f - uiRed), 1.0f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x + 0.2f, y - 0.15f, 0.0f));
    model = glm::scale(model, glm::vec3(0.2f, 0.15f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 1.0f + (1.0f - 1.0f) * uiRed, 1.0f * (1.0f - uiRed), 1.0f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 12, 3);
    }
    if (icon == 0) {
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.29f, y + 0.08f, 0.0f));
        model = glm::scale(model, glm::vec3(0.32f, 0.24f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.55f + (1.0f - 0.55f) * uiRed, 0.35f * (1.0f - uiRed), 0.2f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.22f, y + 0.32f, 0.0f));
        model = glm::scale(model, glm::vec3(0.46f, 0.2f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.75f + (1.0f - 0.75f) * uiRed, 0.2f * (1.0f - uiRed), 0.2f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 6, 3);
        }
    }
    else if (icon == 1) {
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.28f, y + 0.07f, 0.0f));
        model = glm::scale(model, glm::vec3(0.05f, 0.46f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.2f + (1.0f - 0.2f) * uiRed, 0.2f * (1.0f - uiRed), 0.2f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.33f, y + 0.28f, 0.0f));
        model = glm::scale(model, glm::vec3(0.32f, 0.25f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.8f + (1.0f - 0.8f) * uiRed, 0.1f * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 9, 3);
        }
    }
    else {
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.42f, y + 0.2f, 0.0f));
        model = glm::scale(model, glm::vec3(0.06f, 0.28f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.45f + (1.0f - 0.45f) * uiRed, 0.47f * (1.0f - uiRed), 0.55f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.4f, y + 0.48f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.08f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.45f + (1.0f - 0.45f) * uiRed, 0.47f * (1.0f - uiRed), 0.55f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 6, 3);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.33f, y + 0.16f, 0.0f));
        model = glm::scale(model, glm::vec3(0.24f, 0.05f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.5f + (1.0f - 0.5f) * uiRed, 0.32f * (1.0f - uiRed), 0.12f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + 0.43f, y + 0.07f, 0.0f));
        model = glm::scale(model, glm::vec3(0.04f, 0.09f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 0.5f + (1.0f - 0.5f) * uiRed, 0.32f * (1.0f - uiRed), 0.12f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
}

void drawCutscene(float t) {
    float gy = 3.75f;
    if (textReady) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.3f, 0.05f, 0.0f));
    model = glm::scale(model, glm::vec3(15.4f, 0.65f, 1.0f));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniform3f(locTint, 0.1f + (1.0f - 0.1f) * uiRed, 0.1f * (1.0f - uiRed), 0.14f * (1.0f - uiRed));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    if (t > 0.5f && t < 4.5f) {
        float pulse = 0.6f + 0.4f * sin(t * 6.0f);
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.4f, 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(3.2f, 0.1f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, pulse + (1.0f - pulse) * uiRed, pulse * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.4f, 8.5f, 0.0f));
        model = glm::scale(model, glm::vec3(3.2f, 0.1f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, pulse + (1.0f - pulse) * uiRed, pulse * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.4f, 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 8.2f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, pulse + (1.0f - pulse) * uiRed, pulse * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.5f, 0.4f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 8.2f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, pulse + (1.0f - pulse) * uiRed, pulse * (1.0f - uiRed), 0.1f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
    float guardX = 4.0f;
    float lift = 0.0f;
    float enemyX = 16.5f - std::min(t, 4.0f) / 4.0f * 7.5f;
    if (t > 9.5f) guardX = 4.0f + std::min(t - 9.5f, 1.5f) / 1.5f * 4.2f;
    if (t > 11.0f && t < 13.5f) {
        guardX += 0.08f * sin(t * 30.0f);
        enemyX = 8.9f - 0.08f * sin(t * 30.0f);
        lift = 0.15f * sin(t * 30.0f);
        if (((int)(t * 10.0f)) % 2 == 0) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(8.55f, gy + 0.45f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 1.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniform3f(locTint, 1.0f + (1.0f - 1.0f) * uiRed, 0.9f * (1.0f - uiRed), 0.2f * (1.0f - uiRed));
        glDrawArrays(GL_TRIANGLE_FAN, 15, 34);
        }
    }
    else if (t >= 13.5f) {
        enemyX = 8.9f + (t - 13.5f) * 5.0f;
    }
    drawGuard(guardX, gy, lift);
    if (enemyX < 16.5f) drawEnemyActor(enemyX, gy, -lift);
    if (t > 0.5f && t < 4.5f) drawBubble(guardX - 0.3f, gy + 1.05f, 0);
    if (t > 4.5f && t < 7.0f) drawBubble(enemyX - 0.6f, gy + 1.15f, 1);
    if (t > 7.0f && t < 9.5f) drawBubble(guardX - 0.3f, gy + 1.05f, 2);
    if (t > 14.5f) {
        float inX = 17.0f - std::min(t - 14.5f, 1.0f) * 1.7f;
        for (int l = 0; l < 3; l++) drawEnemyActor(inX, soso.tababa[l].yStart + 0.25f, 0.0f, l);
        drawBubble(guardX - 0.3f, gy + 1.05f, 2);
    }
}

void drawMainMenu() {
    glBindVertexArray(menuVAO);
    glUniform3f(fizzo, 1.0f, 1.0f, 1.0f);
    glUniform1f(hilo, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    for (int i = 0; i < 3; i++) {
        glUniform1f(hilo, i == menuHover ? 1.0f : 0.0f);
        glDrawArrays(GL_TRIANGLES, 6 + i * 6, 6);
    }
    glUniform1f(hilo, 0.0f);
    if (!textReady) glDrawArrays(GL_TRIANGLES, menuIconStart, menuIconCount);
}

void drawEndOrPauseMenu() {
    glBindVertexArray(menuVAO);
    glUniform2f(wizzo, 0.0f, 0.0f);
    glUniform3f(fizzo, 1.0f, 1.0f, 1.0f);
    if (isWin || gameOver) {
        glUniform1f(hilo, 0.0f);
        glDrawArrays(GL_TRIANGLES, isWin ? 108 : 114, 6);
        for (int i = 0; i < 2; i++) {
            glUniform1f(hilo, i == menuHover ? 1.0f : 0.0f);
            glDrawArrays(GL_TRIANGLES, 120 + i * 6, 6);
        }
        glUniform1f(hilo, 0.0f);
        if (!textReady) glDrawArrays(GL_TRIANGLES, 132, 15);
    }
    else {
        glUniform1f(hilo, 0.0f);
        glDrawArrays(GL_TRIANGLES, 66, 6);
        for (int i = 0; i < 3; i++) {
            glUniform1f(hilo, i == menuHover ? 1.0f : 0.0f);
            glDrawArrays(GL_TRIANGLES, 72 + i * 6, 6);
        }
        glUniform1f(hilo, 0.0f);
        if (!textReady) glDrawArrays(GL_TRIANGLES, 90, 18);
    }
}

void updateGame(GLFWwindow* window, float dt) {
    animTime += dt;
    startWaveIfNeeded();
    updatePlayerActions(window, dt);
    updateSpawner(dt);
    updateCombat(dt);
    advancePlayers(dt);
    updateEnemiesAndWaves(dt);
}

int main() {
    GLFWwindow* hamada = initWindow();
    if (!hamada) return -1;
    glfwSetFramebufferSizeCallback(hamada, el_shasha_betrg3);
    glfwSetCursorEnterCallback(hamada, el_mouse_dakhal);
    glfwSetCursorPosCallback(hamada, el_mouse_beteeth);
    glfwSetKeyCallback(hamada, dos_zrar);
    glfwSetMouseButtonCallback(hamada, el_mouse_dos);
    sfxInit();
    buildEnvironment();
    buildCoinMesh();
    buildMenuMesh();
    buildUiMesh();
    unsigned int dodo = buildShaderProgram();
    mainProgram = dodo;
    textReady = initText();
    glUseProgram(dodo);
    double el_wa2t_el_adeem = glfwGetTime();

    while (!glfwWindowShouldClose(hamada)) {
        double el_wa2t_el_goded = glfwGetTime();
        float koko = static_cast<float>(el_wa2t_el_goded - el_wa2t_el_adeem);
        el_wa2t_el_adeem = el_wa2t_el_goded;
        if (koko > 0.1f) koko = 0.1f;

        if (!gameOver && !isPaused && !inMainMenu && !isWin && !inCutscene) updateGame(hamada, koko);

        beginFrame(dodo);

        if (inMainMenu) {
            drawMainMenu();
            drawMuteButton();
            drawMainMenuText();
            glfwSwapBuffers(hamada);
            glfwPollEvents();
            continue;
        }

        drawEnvironment();

        if (inCutscene) {
            cutsceneTime += koko;
            uiBegin();
            drawCutscene(cutsceneTime);
            uiEnd();
            drawCutsceneText(cutsceneTime);
            if (cutsceneTime > 16.5f) inCutscene = false;
            glfwSwapBuffers(hamada);
            glfwPollEvents();
            continue;
        }

        drawCoins();

        uiBegin();
        for (const auto& enemy : enemies) {
            if (enemy.isAlive) drawEnemyUnit(enemy);
        }
        for (const auto& p : basal) {
            drawPlayerUnit(p);
        }
        for (const auto& pr : projectiles) {
            drawProjectile(pr);
        }
        drawCoinTracker();
        drawCards();
        uiEnd();
        drawHudText();
        if (isWin || gameOver || isPaused) {
            drawEndOrPauseMenu();
            drawMuteButton();
            drawEndOrPauseText();
        }
        glfwSwapBuffers(hamada);
        glfwPollEvents();
    }
    sfxShutdown();
    glfwTerminate();
    return 0;
}
