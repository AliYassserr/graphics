#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdlib>

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
};

struct Beshamel {
    float x, y;
    float r, g, b;
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

bool isPaused = false;
bool inMainMenu = true;
int menuHover = -1;

struct Vec2 { float x, y; };

float triSign(Vec2 p1, Vec2 p2, Vec2 p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool pointInTriangle(Vec2 pt, Vec2 a, Vec2 b, Vec2 c) {
    float d1 = triSign(pt, a, b);
    float d2 = triSign(pt, b, c);
    float d3 = triSign(pt, c, a);

    bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(hasNeg && hasPos);
}

float menuBtn[3][4] = {
    { 5.5f, 6.0f, 10.5f, 7.4f },
    { 5.5f, 4.1f, 10.5f, 5.5f },
    { 5.5f, 2.2f, 10.5f, 3.6f }
};

int menuButtonAt(float x, float y) {
    Vec2 pt = { x, y };
    for (int i = 0; i < 3; i++) {
        Vec2 p0 = { menuBtn[i][0], menuBtn[i][1] };
        Vec2 p1 = { menuBtn[i][2], menuBtn[i][1] };
        Vec2 p2 = { menuBtn[i][2], menuBtn[i][3] };
        Vec2 p3 = { menuBtn[i][0], menuBtn[i][3] };
        if (pointInTriangle(pt, p0, p1, p2) || pointInTriangle(pt, p2, p3, p0)) return i;
    }
    return -1;
}

Koshary soso;
std::vector<Hawawshy> basal;
std::vector<bool> tamtam(3, false);
int ziz = -1;
bool tot = true;

int ard_3ard = 1280, tol_3ard = 720;

void el_tabkh(float z1, float z2, float z3, float z4, float c1, float c2, float c3,
    std::vector<Beshamel>& batates, std::vector<unsigned int>& kofta) {
    unsigned int bada2 = (unsigned int)batates.size();
    batates.push_back({ z1,     z2,     c1, c2, c3 });
    batates.push_back({ z1 + z3, z2,     c1, c2, c3 });
    batates.push_back({ z1 + z3, z2 + z4, c1, c2, c3 });
    batates.push_back({ z1,     z2 + z4, c1, c2, c3 });

    kofta.push_back(bada2 + 0);
    kofta.push_back(bada2 + 1);
    kofta.push_back(bada2 + 2);
    kofta.push_back(bada2 + 2);
    kofta.push_back(bada2 + 3);
    kofta.push_back(bada2 + 0);
}

void el_masla3(float x1, float y1, float x2, float y2, float x3, float y3, float c1, float c2, float c3,
    std::vector<Beshamel>& batates, std::vector<unsigned int>& kofta) {
    unsigned int bada2 = (unsigned int)batates.size();
    batates.push_back({ x1, y1, c1, c2, c3 });
    batates.push_back({ x2, y2, c1, c2, c3 });
    batates.push_back({ x3, y3, c1, c2, c3 });
    kofta.push_back(bada2 + 0);
    kofta.push_back(bada2 + 1);
    kofta.push_back(bada2 + 2);
}

const char* kalmita ="#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 vertexColor;\n"
"uniform mat4 uProjection;\n"
"uniform vec2 uOffset;\n"
"void main(){\n"
"   gl_Position = uProjection * vec4(aPos + uOffset, 0.0, 1.0);\n"
"   vertexColor = aColor;\n"
"}\n";

const char* shabash = "#version 330 core\n"
"in vec3 vertexColor;\n"
"out vec4 FragColor;\n"
"uniform vec3 uTint;\n"
"uniform float u_Highlight;\n"
"void main(){\n"
"   vec3 finalColor = mix(vertexColor * uTint, vec3(1.0, 1.0, 0.0), u_Highlight);\n"
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
void el_mouse_dakhal(GLFWwindow* bobo, int dakhal) {
    tot = dakhal;
    if (!dakhal) { ziz = -1; menuHover = -1; }

}
void dos_zrar(GLFWwindow* tabl, int nono, int zaza, int pika, int mizo) {
    if (inMainMenu) return;

    if (nono == GLFW_KEY_ESCAPE && pika == GLFW_PRESS)
        isPaused = !isPaused;
    if (nono == GLFW_KEY_SPACE && pika == GLFW_PRESS) {
        for (size_t i = 0; i < tamtam.size(); ++i) {
            if (tamtam[i]) {

                basal.push_back({ (int)i, soso.akhdar, 3.5f, 0.0f, false, false, 0.0f, 5, 0.0f });
            }
        }
    }
}
void el_mouse_beteeth(GLFWwindow* tabl, double mash, double mosh) {
    if (!tot || tol_3ard == 0) return;
    int winW, winH;
    glfwGetWindowSize(tabl, &winW, &winH);
    if (winW == 0 || winH == 0) return;
    float wX = (float)mash / winW * 16.0f;
    float wY = (1.0f - (float)mosh / winH) * 10.0f;
    if (inMainMenu) {
        menuHover = menuButtonAt(wX, wY);
        return;
    }
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
    if (inMainMenu) {
        int hit = menuButtonAt(wX, wY);
        if (hit == 0) inMainMenu = false;
        else if (hit == 2) glfwSetWindowShouldClose(tabl, true);
        return;
    }
    if (wX >= soso.asfar && wX <= soso.akhdar) {
        std::cout << "Clicked Gatehouse Home Base!\n";
    }
    else {
        int nawwar = soso.getLaneFromY(wY);
        if (nawwar != -1) {
            tamtam[nawwar] = !tamtam[nawwar];
        }
    }
}

bool g_gameOver = false;
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

Enemy(int enemyId, EnemyType t, int lane, float startX, float laneY) {
    id = enemyId;
    type = t;
    laneId = lane;
    xPos = startX;
    yPos = laneY;
    isAlive = true;
    damageTint = 0.0f;
    blocked = false;

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
        speed = 0.4f;
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

std::vector<Enemy> g_enemies;
std::vector<Coin> g_coins;
int g_currentWave = 1;
int g_playerCoins = 0;
bool g_waveInProgress = false;

void spawnWave(int waveNumber, const Koshary& grid) {
    g_enemies.clear();
    float startX = 16.0f;

    if (waveNumber == 1) {
        for (int i = 0; i < 3; i++) {
            int lane = rand() % 3;
            float laneY = grid.tababa[lane].centerY;
            g_enemies.push_back(Enemy(i, EASY_UPPER, lane, startX + (i * 2.5f), laneY));
        }
    } else if (waveNumber == 2) {
        for (int i = 0; i < 5; i++) {
            EnemyType type = (i % 2 == 0) ? EASY_UPPER : MEDIUM_LOWER;
            int lane = rand() % 3;
            float laneY = grid.tababa[lane].centerY;
            g_enemies.push_back(Enemy(i, type, lane, startX + (i * 2.5f), laneY));
        }
    } else if (waveNumber == 3) {
        for (int i = 0; i < 10; i++) {
            EnemyType type = (i % 3 == 0) ? EASY_UPPER : ((i % 3 == 1) ? MEDIUM_LOWER : HARD_MELEE);
            int lane = rand() % 3;
            float laneY = grid.tababa[lane].centerY;
            g_enemies.push_back(Enemy(i, type, lane, startX + (i * 2.5f), laneY));
        }
    }
    g_waveInProgress = true;
}
int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* hamada = glfwCreateWindow(ard_3ard, tol_3ard, "Tower Defense Engine", NULL, NULL);
    if (!hamada) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(hamada);
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
}
    glfwGetFramebufferSize(hamada, &ard_3ard, &tol_3ard);
    glViewport(0, 0, ard_3ard, tol_3ard);
    glfwSetFramebufferSizeCallback(hamada, el_shasha_betrg3);
    glfwSetCursorEnterCallback(hamada, el_mouse_dakhal);
    glfwSetCursorPosCallback(hamada, el_mouse_beteeth);
    glfwSetKeyCallback(hamada, dos_zrar);
    glfwSetMouseButtonCallback(hamada, el_mouse_dos);
    std::vector<Beshamel> batates;
    std::vector<unsigned int> kofta;
    el_tabkh(0.5f, 0.5f, 3.0f, 8.0f, 0.32f, 0.35f, 0.42f, batates, kofta);
    el_tabkh(1.6f, 0.5f, 0.8f, 2.2f, 0.15f, 0.15f, 0.18f, batates, kofta);
    el_tabkh(1.2f, 6.5f, 0.4f, 0.8f, 0.20f, 0.22f, 0.28f, batates, kofta);
    el_tabkh(2.4f, 6.5f, 0.4f, 0.8f, 0.20f, 0.22f, 0.28f, batates, kofta);
    el_tabkh(0.5f, 8.5f, 0.6f, 0.4f, 0.22f, 0.25f, 0.30f, batates, kofta);
    el_tabkh(1.3f, 8.5f, 0.6f, 0.4f, 0.22f, 0.25f, 0.30f, batates, kofta);
    el_tabkh(2.1f, 8.5f, 0.6f, 0.4f, 0.22f, 0.25f, 0.30f, batates, kofta);
    el_tabkh(2.9f, 8.5f, 0.6f, 0.4f, 0.22f, 0.25f, 0.30f, batates, kofta);
    int meshmesh = (int)kofta.size();
    el_tabkh(3.5f, 6.2f, 12.0f, 2.0f, 0.2f, 0.6f, 0.9f, batates, kofta);
    el_tabkh(3.5f, 3.5f, 12.0f, 2.0f, 0.2f, 0.6f, 0.9f, batates, kofta);
    el_tabkh(3.5f, 0.8f, 12.0f, 2.0f, 0.2f, 0.6f, 0.9f, batates, kofta);
    int foul_indices = (int)kofta.size() - meshmesh;
    el_tabkh(5.5f, 8.2f, 0.4f, 1.2f, 0.45f, 0.30f, 0.15f, batates, kofta);
    el_tabkh(5.0f, 9.0f, 1.4f, 0.8f, 0.15f, 0.55f, 0.20f, batates, kofta);
    el_tabkh(5.2f, 9.6f, 1.0f, 0.6f, 0.18f, 0.65f, 0.22f, batates, kofta);
    el_tabkh(8.0f, 8.2f, 0.2f, 1.6f, 0.30f, 0.30f, 0.30f, batates, kofta);
    el_tabkh(7.0f, 9.0f, 1.0f, 0.7f, 0.75f, 0.15f, 0.15f, batates, kofta);
    el_tabkh(7.3f, 9.2f, 0.3f, 0.3f, 0.95f, 0.85f, 0.20f, batates, kofta);
    el_tabkh(11.0f, 8.2f, 0.2f, 1.0f, 0.35f, 0.25f, 0.15f, batates, kofta);
    el_tabkh(10.8f, 9.1f, 0.6f, 0.3f, 0.25f, 0.25f, 0.28f, batates, kofta);
    el_tabkh(10.95f, 9.4f, 0.3f, 0.5f, 0.95f, 0.50f, 0.10f, batates, kofta);
    el_tabkh(11.05f, 9.5f, 0.15f, 0.3f, 0.95f, 0.90f, 0.20f, batates, kofta);
    el_tabkh(13.5f, 8.2f, 1.8f, 1.5f, 0.40f, 0.43f, 0.52f, batates, kofta);
    el_tabkh(13.5f, 9.7f, 0.4f, 0.3f, 0.28f, 0.30f, 0.38f, batates, kofta);
    el_tabkh(14.2f, 9.7f, 0.4f, 0.3f, 0.28f, 0.30f, 0.38f, batates, kofta);
    el_tabkh(14.9f, 9.7f, 0.4f, 0.3f, 0.28f, 0.30f, 0.38f, batates, kofta);
    el_tabkh(14.2f, 8.8f, 0.4f, 0.6f, 0.18f, 0.20f, 0.25f, batates, kofta);
    el_tabkh(3.5f, 0.0f, 12.0f, 0.8f, 0.35f, 0.38f, 0.45f, batates, kofta);
    for (float wx = 4.0f; wx < 15.0f; wx += 1.2f) {
        el_tabkh(wx, 0.8f, 0.6f, 0.4f, 0.28f, 0.30f, 0.38f, batates, kofta);
    }
    int falafel_indices = (int)kofta.size() - meshmesh - foul_indices;

    unsigned int shiko, biko, fiko;
    glGenVertexArrays(1, &biko);
    glGenBuffers(1, &shiko);
    glGenBuffers(1, &fiko);
    glBindVertexArray(biko);
    glBindBuffer(GL_ARRAY_BUFFER, shiko);
    glBufferData(GL_ARRAY_BUFFER, batates.size() * sizeof(Beshamel), batates.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fiko);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, kofta.size() * sizeof(unsigned int), kofta.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Beshamel), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Beshamel), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    Beshamel miko_verts[] = {
        { -0.2f, -0.2f,  0.95f, 0.85f, 0.2f },
        { 0.2f, -0.2f,  0.95f, 0.85f, 0.2f },
        { 0.2f,  0.2f,  0.95f, 0.85f, 0.2f },
        { -0.2f,  0.2f,  0.95f, 0.85f, 0.2f }
    };
    unsigned int miko_inds[] = { 0, 1, 2, 2, 3, 0 };
    unsigned int ziko, ziro_vao, ziro_ebo;
    glGenVertexArrays(1, &ziro_vao);
    glGenBuffers(1, &ziko);
    glGenBuffers(1, &ziro_ebo);
    glBindVertexArray(ziro_vao);
    glBindBuffer(GL_ARRAY_BUFFER, ziko);
    glBufferData(GL_ARRAY_BUFFER, sizeof(miko_verts), miko_verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ziro_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(miko_inds), miko_inds, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Beshamel), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Beshamel), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    std::vector<Beshamel> menuVerts;
    std::vector<unsigned int> menuInds;
    el_tabkh(4.9f, 1.7f, 6.2f, 6.3f, 0.18f, 0.20f, 0.28f, menuVerts, menuInds);
    el_tabkh(menuBtn[0][0], menuBtn[0][1], menuBtn[0][2] - menuBtn[0][0], menuBtn[0][3] - menuBtn[0][1], 0.20f, 0.60f, 0.25f, menuVerts, menuInds);
    el_tabkh(menuBtn[1][0], menuBtn[1][1], menuBtn[1][2] - menuBtn[1][0], menuBtn[1][3] - menuBtn[1][1], 0.20f, 0.40f, 0.80f, menuVerts, menuInds);
    el_tabkh(menuBtn[2][0], menuBtn[2][1], menuBtn[2][2] - menuBtn[2][0], menuBtn[2][3] - menuBtn[2][1], 0.75f, 0.20f, 0.20f, menuVerts, menuInds);
    el_masla3(7.7f, 6.2f, 7.7f, 7.2f, 8.5f, 6.7f, 1.0f, 1.0f, 1.0f, menuVerts, menuInds);
    el_tabkh(7.55f, 4.3f, 0.25f, 0.4f, 1.0f, 1.0f, 1.0f, menuVerts, menuInds);
    el_tabkh(7.95f, 4.3f, 0.25f, 0.7f, 1.0f, 1.0f, 1.0f, menuVerts, menuInds);
    el_tabkh(8.35f, 4.3f, 0.25f, 1.0f, 1.0f, 1.0f, 1.0f, menuVerts, menuInds);
    el_masla3(7.6f, 2.4f, 7.85f, 2.4f, 8.4f, 3.4f, 1.0f, 1.0f, 1.0f, menuVerts, menuInds);
    el_masla3(8.4f, 3.4f, 8.15f, 3.4f, 7.6f, 2.4f, 1.0f, 1.0f, 1.0f, menuVerts, menuInds);
    el_masla3(7.6f, 3.4f, 7.85f, 3.4f, 8.4f, 2.4f, 1.0f, 1.0f, 1.0f, menuVerts, menuInds);
    el_masla3(8.4f, 2.4f, 8.15f, 2.4f, 7.6f, 3.4f, 1.0f, 1.0f, 1.0f, menuVerts, menuInds);
    int menuIconCount = (int)menuInds.size() - 24;

    unsigned int menuVBO, menuVAO, menuEBO;
    glGenVertexArrays(1, &menuVAO);
    glGenBuffers(1, &menuVBO);
    glGenBuffers(1, &menuEBO);
    glBindVertexArray(menuVAO);
    glBindBuffer(GL_ARRAY_BUFFER, menuVBO);
    glBufferData(GL_ARRAY_BUFFER, menuVerts.size() * sizeof(Beshamel), menuVerts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, menuEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, menuInds.size() * sizeof(unsigned int), menuInds.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Beshamel), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Beshamel), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

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
    int gizmo = glGetUniformLocation(dodo, "uProjection");
    int wizzo = glGetUniformLocation(dodo, "uOffset");
    int fizzo = glGetUniformLocation(dodo, "uTint");
    int hilo = glGetUniformLocation(dodo, "u_Highlight");
    glUseProgram(dodo);
    glUniform1f(hilo, 0.0f);
    double el_wa2t_el_adeem = glfwGetTime();

    while (!glfwWindowShouldClose(hamada)) {
        double el_wa2t_el_goded = glfwGetTime();
        float koko = static_cast<float>(el_wa2t_el_goded - el_wa2t_el_adeem);
        el_wa2t_el_adeem = el_wa2t_el_goded;
        if (koko > 0.1f) koko = 0.1f;

        if (!g_gameOver && !isPaused && !inMainMenu) {

            if (!g_waveInProgress && g_currentWave <= 3) {
                spawnWave(g_currentWave, soso);
            }

            bool wPressed = (glfwGetKey(hamada, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(hamada, GLFW_KEY_UP) == GLFW_PRESS);
            bool sPressed = (glfwGetKey(hamada, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(hamada, GLFW_KEY_DOWN) == GLFW_PRESS);
            for (auto& p : basal) {
                bool laneSelected = tamtam[p.laneId];
                if (laneSelected && wPressed && !p.isJumping) {
                    p.isJumping = true;
                    p.jumpTimer = 0.0f;
                }
                p.isDucking = laneSelected && sPressed;
                if (p.isJumping) {
                    p.jumpTimer += koko * 5.0f;
                    p.yOffset = 0.6f * sin(p.jumpTimer);
                    if (p.jumpTimer >= 3.14159f) {
                        p.isJumping = false;
                        p.yOffset = 0.0f;
                    }
                }
            }

            for (auto& enemy : g_enemies) {
                enemy.blocked = false;
                if (enemy.damageTint > 0.0f) enemy.damageTint -= koko * 3.0f;
            }
            for (auto pIt = basal.begin(); pIt != basal.end();) {
                bool playerDestroyed = false;
                bool isColliding = false;
                if (pIt->damageTint > 0.0f) pIt->damageTint -= koko * 3.0f;

                for (auto& enemy : g_enemies) {
                    if (!enemy.isAlive || enemy.laneId != pIt->laneId) continue;
                    if (std::abs(pIt->xPos - enemy.xPos) < 0.6f) {
                        isColliding = true;
                        enemy.blocked = true;

                        bool hit = true;
                        if (enemy.type == EASY_UPPER && pIt->isDucking) hit = false;
                        if (enemy.type == MEDIUM_LOWER && pIt->isJumping) hit = false;

                        if (hit && pIt->damageTint <= 0.0f) {
                            pIt->health -= enemy.damage;
                            pIt->damageTint = 0.5f;
                            enemy.health -= 1;
                            enemy.damageTint = 0.5f;

                            if (enemy.health <= 0) {
                                enemy.isAlive = false;
                                Coin drop;
                                drop.xPos = enemy.xPos;
                                drop.yPos = enemy.yPos;
                                drop.collected = false;
                                g_coins.push_back(drop);
                                std::cout << "Enemy Defeated! Gold Coin Dropped!" << std::endl;
                            }
                            if (pIt->health <= 0) {
                                playerDestroyed = true;
                                break;
                            }
                        }
                    }
                }

                pIt->spd = isColliding ? 0.0f : 3.5f;

                if (playerDestroyed) pIt = basal.erase(pIt);
                else ++pIt;
            }

            for (auto it = basal.begin(); it != basal.end();) {
                it->xPos += it->spd * koko;
                if (it->xPos > 16.5f) it = basal.erase(it);
                else ++it;
            }

            bool allDefeated = true;
            for (auto& enemy : g_enemies) {
                if (!enemy.isAlive) continue;
                enemy.update(koko);
                allDefeated = false;

                if (enemy.xPos <= 3.5f) {
                    g_gameOver = true;
                    std::cout << "GAME OVER! Enemy breached Home Base!" << std::endl;
                }
            }

            for (auto& coin : g_coins) {
                if (coin.collected) continue;
                for (const auto& p : basal) {
                    float py = soso.tababa[p.laneId].centerY;
                    if (std::abs(coin.xPos - p.xPos) < 0.6f && std::abs(coin.yPos - py) < 0.6f) {
                        coin.collected = true;
                        g_playerCoins += 10;
                        std::cout << "Coin Picked Up! Total Coins: " << g_playerCoins << std::endl;
                        break;
                    }
                }
            }

            if (allDefeated && g_waveInProgress) {
                g_waveInProgress = false;
                g_currentWave++;
                std::cout << "Wave Cleared! Advancing to Wave " << g_currentWave << std::endl;
            }

        }

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(dodo);
        float left = 0.0f, right = 16.0f, bottom = 0.0f, top = 10.0f;
        float dolly[16] = {
            2.0f / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 0.0f,
            -(right + left) / (right - left), -(top + bottom) / (top - bottom), 0.0f, 1.0f
        };
        glUniformMatrix4fv(gizmo, 1, GL_FALSE, dolly);
        glUniform2f(wizzo, 0.0f, 0.0f);

        if (inMainMenu) {
            glBindVertexArray(menuVAO);
            glUniform3f(fizzo, 1.0f, 1.0f, 1.0f);
            glUniform1f(hilo, 0.0f);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
            for (int i = 0; i < 3; i++) {
                glUniform1f(hilo, i == menuHover ? 0.5f : 0.0f);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)((6 + i * 6) * sizeof(unsigned int)));
            }
            glUniform1f(hilo, 0.0f);
            glDrawElements(GL_TRIANGLES, menuIconCount, GL_UNSIGNED_INT, (void*)(24 * sizeof(unsigned int)));
            glfwSwapBuffers(hamada);
            glfwPollEvents();
            continue;
        }

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
        glBindVertexArray(ziro_vao);

        for (const auto& enemy : g_enemies) {
            if (enemy.isAlive) {

                if (enemy.type == EASY_UPPER) {
                    glUniform3f(fizzo, 1.0f, 0.2f, 0.2f);
                } else if (enemy.type == MEDIUM_LOWER) {
                    glUniform3f(fizzo, 1.0f, 0.6f, 0.0f);
                } else if (enemy.type == HARD_MELEE) {
                    glUniform3f(fizzo, 0.6f, 0.1f, 0.8f);
                }
                if (enemy.damageTint > 0.0f) glUniform3f(fizzo, 2.0f, 0.2f, 0.2f);
                glUniform2f(wizzo, enemy.xPos, enemy.yPos);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

                glUniform3f(fizzo, 0.0f, 1.0f, 0.2f);
                glUniform2f(wizzo, enemy.xPos, enemy.yPos + 0.35f);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
            }
        }

        for (const auto& coin : g_coins) {
            if (!coin.collected) {

                glUniform3f(fizzo, 1.0f, 0.85f, 0.0f);

                glUniform2f(wizzo, coin.xPos, coin.yPos);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
            }
        }

        for (const auto& p : basal) {
            float duckOffset = p.isDucking ? -0.15f : 0.0f;
            float cy = soso.tababa[p.laneId].centerY + p.yOffset + duckOffset;
            glUniform2f(wizzo, p.xPos, cy);
            if (p.damageTint > 0.0f) glUniform3f(fizzo, 2.0f, 0.2f, 0.2f);
            else glUniform3f(fizzo, 1.0f, 1.0f, 1.0f);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        glfwSwapBuffers(hamada);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}