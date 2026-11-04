#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

struct Mahshy { int id; float yStart, yEnd, centerY; };

// Member 2 Player Unit (with health & tint timer)
struct Hawawshy {
    int laneId;
    float xPos;
    float spd;
    float yOffset;    // Jump height offset
    bool isJumping;   // Air state
    bool isDucking;   // Ducking state
    float jumpTimer;  // Jump duration
    int health;       // Player unit health
    float damageTint; // Red flash timer when damaged
};

// Member 2 Enemy Unit
struct Enemy {
    int laneId;
    float xPos;
    float spd;
    int health;
    int attackType;   // 0 = High Attack (Dodge with S), 1 = Low Attack (Dodge with W)
    float damageTint; // Red flash timer when damaged
};

// Global vectors
std::vector<Enemy> batates_enemies;
std::vector<Hawawshy> basal;
std::vector<bool> tamtam(3, false);
int ziz = -1;
bool tot = true;

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

Koshary soso;
int ard_3ard = 1280, tol_3ard = 720;

void el_tabkh(float z1, float z2, float z3, float z4, float c1, float c2, float c3,
    std::vector<Beshamel>& batates, std::vector<unsigned int>& kofta) {
    unsigned int bada2 = (unsigned int)batates.size();
    batates.push_back({ z1,      z2,      c1, c2, c3 });
    batates.push_back({ z1 + z3, z2,      c1, c2, c3 });
    batates.push_back({ z1 + z3, z2 + z4, c1, c2, c3 });
    batates.push_back({ z1,      z2 + z4, c1, c2, c3 });

    kofta.push_back(bada2 + 0);
    kofta.push_back(bada2 + 1);
    kofta.push_back(bada2 + 2);
    kofta.push_back(bada2 + 2);
    kofta.push_back(bada2 + 3);
    kofta.push_back(bada2 + 0);
}

const char* kalmita = "#version 330 core\n"
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
"void main(){\n"
"   FragColor = vec4(vertexColor * uTint, 1.0);\n"
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
    if (!dakhal) ziz = -1;
}

void dos_zrar(GLFWwindow* tabl, int nono, int zaza, int pika, int mizo) {
    if (nono == GLFW_KEY_ESCAPE && pika == GLFW_PRESS) glfwSetWindowShouldClose(tabl, true);

    // SPACEBAR: Spawn Player Unit
    if (nono == GLFW_KEY_SPACE && pika == GLFW_PRESS) {
        for (size_t i = 0; i < tamtam.size(); ++i) {
            if (tamtam[i]) {
                // laneId, xPos, spd, yOffset, isJumping, isDucking, jumpTimer, health, damageTint
                basal.push_back({ (int)i, soso.akhdar, 3.5f, 0.0f, false, false, 0.0f, 5, 0.0f });
            }
        }
    }

    // E KEY: Spawn High-Attack Enemy Unit (Dodge with S / Duck)
    if (nono == GLFW_KEY_E && pika == GLFW_PRESS) {
        for (size_t i = 0; i < tamtam.size(); ++i) {
            if (tamtam[i]) {
                // laneId, xPos, spd, health, attackType (0 = High), damageTint
                batates_enemies.push_back({ (int)i, 15.5f, 2.0f, 3, 0, 0.0f });
            }
        }
    }

    // R KEY: Spawn Low-Attack Enemy Unit (Dodge with W / Jump)
    if (nono == GLFW_KEY_R && pika == GLFW_PRESS) {
        for (size_t i = 0; i < tamtam.size(); ++i) {
            if (tamtam[i]) {
                // laneId, xPos, spd, health, attackType (1 = Low), damageTint
                batates_enemies.push_back({ (int)i, 15.5f, 2.0f, 3, 1, 0.0f });
            }
        }
    }
}

void el_mouse_beteeth(GLFWwindow* tabl, double mash, double mosh) {
    if (!tot || tol_3ard == 0) return;
    float wX = (float)mash / ard_3ard * 16.0f;
    float wY = (1.0f - (float)mosh / tol_3ard) * 9.0f;
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
    float wX = (float)mash / ard_3ard * 16.0f;
    float wY = (1.0f - (float)mosh / tol_3ard) * 9.0f;
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
        std::cout << "Failed to initialize GLAD\n";
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
    double el_wa2t_el_adeem = glfwGetTime();

    while (!glfwWindowShouldClose(hamada)) {
        double el_wa2t_el_goded = glfwGetTime();
        float koko = static_cast<float>(el_wa2t_el_goded - el_wa2t_el_adeem);
        el_wa2t_el_adeem = el_wa2t_el_goded;
        if (koko > 0.1f) koko = 0.1f;

        // --- Member 2: Jump & Duck Controls ---
        bool wPressed = (glfwGetKey(hamada, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(hamada, GLFW_KEY_UP) == GLFW_PRESS);
        bool sPressed = (glfwGetKey(hamada, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(hamada, GLFW_KEY_DOWN) == GLFW_PRESS);

        for (auto& p : basal) {
            if (wPressed && !p.isJumping) {
                p.isJumping = true;
                p.jumpTimer = 0.0f;
            }

            p.isDucking = sPressed;

            if (p.isJumping) {
                p.jumpTimer += koko * 5.0f;
                p.yOffset = 0.6f * sin(p.jumpTimer);
                if (p.jumpTimer >= 3.14159f) {
                    p.isJumping = false;
                    p.yOffset = 0.0f;
                }
            }
        }

        // --- Member 2: Collision, Block & Damage Engine ---
        for (auto pIt = basal.begin(); pIt != basal.end();) {
            bool playerDestroyed = false;
            bool isColliding = false;

            if (pIt->damageTint > 0.0f) {
                pIt->damageTint -= koko * 3.0f;
            }

            for (auto eIt = batates_enemies.begin(); eIt != batates_enemies.end();) {
                if (eIt->damageTint > 0.0f) {
                    eIt->damageTint -= koko * 3.0f;
                }

                if (pIt->laneId == eIt->laneId) {
                    float distanceX = std::abs(pIt->xPos - eIt->xPos);

                    if (distanceX < 0.6f) {
                        isColliding = true;
                        pIt->spd = 0.0f;
                        eIt->spd = 0.0f;

                        bool hit = true;

                        // Ducking (S) dodges High Attacks (attackType 0)
                        if (eIt->attackType == 0 && pIt->isDucking) {
                            hit = false;
                        }

                        // Jumping (W) dodges Low Attacks (attackType 1)
                        if (eIt->attackType == 1 && pIt->isJumping) {
                            hit = false;
                        }

                        if (hit && pIt->damageTint <= 0.0f) {
                            pIt->health -= 1;
                            pIt->damageTint = 0.5f;

                            eIt->health -= 1;
                            eIt->damageTint = 0.5f;

                            if (pIt->health <= 0) {
                                playerDestroyed = true;
                            }
                            if (eIt->health <= 0) {
                                eIt = batates_enemies.erase(eIt);
                                continue;
                            }
                        }
                    }
                }
                ++eIt;
            }

            // Resume walking forward if no enemy is blocking
            if (!isColliding) {
                pIt->spd = 3.5f;
            }

            if (playerDestroyed) {
                pIt = basal.erase(pIt);
            }
            else {
                ++pIt;
            }
        }

        // --- Update Enemy Movement ---
        for (auto eIt = batates_enemies.begin(); eIt != batates_enemies.end();) {
            eIt->xPos -= eIt->spd * koko;
            if (eIt->xPos < 0.5f) {
                eIt = batates_enemies.erase(eIt);
            }
            else {
                ++eIt;
            }
        }

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

        // --- Render Player Units ---
        for (auto it = basal.begin(); it != basal.end();) {
            it->xPos += it->spd * koko;

            if (it->xPos > 16.5f) {
                it = basal.erase(it);
            }
            else {
                float duckOffset = it->isDucking ? -0.15f : 0.0f; // Visual ducking squish
                float cy = soso.tababa[it->laneId].centerY + it->yOffset + duckOffset;
                glUniform2f(wizzo, it->xPos, cy);

                if (it->damageTint > 0.0f) {
                    glUniform3f(fizzo, 2.0f, 0.2f, 0.2f); // Red flash tint on hit
                }
                else {
                    glUniform3f(fizzo, 1.0f, 1.0f, 1.0f);
                }

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                ++it;
            }
        }

        // --- Render Enemy Units ---
        for (auto& enemy : batates_enemies) {
            float cy = soso.tababa[enemy.laneId].centerY;
            glUniform2f(wizzo, enemy.xPos, cy);

            if (enemy.damageTint > 0.0f) {
                glUniform3f(fizzo, 2.0f, 0.2f, 0.2f);
            }
            else if (enemy.attackType == 1) {
                glUniform3f(fizzo, 0.9f, 0.5f, 0.1f); // Orange for Low Attack Enemies
            }
            else {
                glUniform3f(fizzo, 0.8f, 0.2f, 0.2f); // Dark Red for High Attack Enemies
            }

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(hamada);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}