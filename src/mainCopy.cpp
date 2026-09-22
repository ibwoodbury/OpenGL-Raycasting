#include <GL/gl.h>
#include <GLFW/glfw3.h>

// #include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
//
// // GENERATE CMAKE FILES FOR NEW PROJECT
// delete build
// cmake -S . -B build
// // BUILD
// //  cmake --build build
// //  BUILD FOR RELEASE
// // cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release

//----------------------------------------
//   GL ORTHO EXAMPLE
//----------------------------------------
// TODO:
// if it hits the maximum number of iters, it counts as hitting a wall
//

#include <GLFW/glfw3.h>

#include "../stb_easy_font.h"
#include <iostream>
// #include <stb_easy_font.h>
void printtheen(std::string message, float theen) { std::cout << message + std::to_string(theen) << std::endl; }

// BOILERPLATE-----------------------------
//
//
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0, width, height, 0.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawText(float x, float y, const char *text) {
    char buffer[10000];

    unsigned char color[4] = {255, 255, 255, 255};

    int quads = stb_easy_font_print(x, y, const_cast<char *>(text), color, buffer, sizeof(buffer));

    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(2, GL_FLOAT, 16, buffer);

    glDrawArrays(GL_QUADS, 0, quads * 4);

    glDisableClientState(GL_VERTEX_ARRAY);
}

// BOILERPLATE-----------------------------

// SETUP -----------------------------
//
//
const int rows = 20;
const int cols = 20;
const int res = 50;
const int numRays = 85;
const float fov = 85;
const int padding = 10;
const int sceneOffset = cols * res + padding;

const float rayAngleStep = fov / numRays;

const int width = res * cols;
const int height = res * rows;
float radians = M_PI / 180.0;
const int maxIters = static_cast<int>(std::sqrt(cols * cols + rows * rows) * 0.80);
struct Ray {
    float x;
    float y;
    float x2;
    float y2;
};

float playerX = width / 2.0;
float playerY = height / 2.0;
float playerSpeed = 3;
float playerSize = 20;
float playerAngle = 0;

// int grid[rows][cols];
int grid[rows][cols] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //
};
Ray rays[numRays];

float magnitude(Ray ray) { return sqrtf((ray.x - ray.x2) * (ray.x - ray.x2) + (ray.y - ray.y2) * (ray.y - ray.y2)); }
int roundToNearest(float num, int to) { return static_cast<int>(std::floor(num / to) * to); }
// Point getCell(float x, float y, int grid[rows][cols]) { return Point{.x = static_cast<int>(std::floor(x / res)), .y = static_cast<int>(std::floor(y / res))}; }
void processKeypress(GLFWwindow &window) {
    float pointX = playerX + cos(playerAngle * radians) * 20;
    float pointY = playerY + sin(playerAngle * radians) * 20;
    glPointSize(5.0f);
    glColor3f(0, 0, 0);
    glBegin(GL_POINTS);
    glVertex2f(pointX, pointY);
    glEnd();
    int gridX = static_cast<int>(std::floor(pointX / res));
    int gridY = static_cast<int>(std::floor(pointY / res));

    std::string coords = "(" + std::to_string(gridX) + "," + std::to_string(gridY) + ")" + "\n" + std::to_string(static_cast<int>(playerAngle));
    glColor3f(0, 0, 0);
    glPushMatrix();
    glTranslatef(playerX, playerY + 50, 0);
    glScalef(2.0f, 2.0f, 1.0f);

    drawText(0, 0, coords.c_str());
    glPopMatrix();
    // printtheen("pointX",pointX);
    // printtheen("pointY",pointY);
    if (glfwGetKey(&window, GLFW_KEY_UP) == GLFW_PRESS && grid[gridY][gridX] == 0) {
        // if (glfwGetKey(&window, GLFW_KEY_UP) == GLFW_PRESS &&grid[static_cast<int>(std::floor((playerY+sin(playerAngle*radians)*10)/res))][static_cast<int>(std::floor((playerX+cos(playerAngle*radians)*10)/res))]==0) {
        // if (glfwGetKey(&window, GLFW_KEY_UP) == GLFW_PRESS && grid[pointY][pointX]==0) {
        // playerY += playerSpeed;
        // SIN AND COS ARE REVERSED BECAUSE BETWEEN THEM THERE IS A PHASE SHIFT OF 90 DEG, and I am countup up (90 deg) as 0
        // deg
        playerY += playerSpeed * sin(playerAngle * radians);
        playerX += playerSpeed * cos(playerAngle * radians);
        // playerY -= playerSpeed * cos(playerAngle * radians);
        // playerX += playerSpeed * sin(playerAngle * radians);
    }
    if (glfwGetKey(&window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        playerAngle -= playerSpeed;
    }
    if (glfwGetKey(&window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        // playerY -= playerSpeed;
        playerX -= playerSpeed * cos(playerAngle * radians);
        playerY -= playerSpeed * sin(playerAngle * radians);
        // playerX -= playerSpeed * sin(playerAngle * radians);
        // playerY += playerSpeed * cos(playerAngle * radians);
    }
    if (glfwGetKey(&window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        playerAngle += playerSpeed;
    }
    if (playerAngle < 0) {
        playerAngle = 360;
    }
    if (playerAngle > 360) {
        playerAngle = 0;
    }
}
void drawGrid(int grid[rows][cols]) {

    // drarw border around grid
    glLineWidth(5.0f);
    glColor3f(0, 0, 0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0, 0);
    glVertex2f(0, height);
    glVertex2f(width, height);
    glVertex2f(width, 0);
    glEnd();

    // draw grid
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j]) {

                glColor3f(0, 0, 0);
                glBegin(GL_QUADS);
                glVertex2f(j * res, i * res);
                glVertex2f(j * res + res, i * res);
                glVertex2f(j * res + res, i * res + res);
                glVertex2f(j * res, i * res + res);
                glEnd();

                // printtheen("Width: ", rows * res + res);
            }

            glLineWidth(1.0f);
            glBegin(GL_LINE_LOOP);
            glVertex2f(j * res, i * res);
            glVertex2f(j * res + res, i * res);
            glVertex2f(j * res + res, i * res + res);
            glVertex2f(j * res, i * res + res);

            glEnd();
            glColor3f(0, 0, 0);
            std::string coords = "(" + std::to_string(j) + "," + std::to_string(i) + ")";
            drawText(j * res + 5, i * res + 10, coords.c_str());
        }
    }
}
void drawPlayer(int x, int y, int size, int angle) {
    int center = size / 2;
    glLineWidth(3.0f);
    glColor3f(1, 0, 0);
    glPushMatrix();
    glTranslatef(x, y, 0);     // move rotation center
    glRotatef(angle, 0, 0, 1); // rotate around Z axis
    glBegin(GL_QUADS);
    // glVertex2f(-center, -center);
    // glVertex2f(-center, center);
    // glVertex2f(center, center);
    // glVertex2f(center, -center);
    glVertex2f(-center, -center);
    glVertex2f(-center, center);
    glVertex2f(center, center);
    glVertex2f(center, -center);

    // glVertex2f(0, 0);
    // glVertex2f(10, 0);
    // glVertex2f(10, 10);
    // glVertex2f(0, 10);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(center, 0);
    glVertex2f(size, 0);
    glEnd();
    glTranslatef(-x, -y, 0); // move back
    glPopMatrix();
    glColor3f(1, 0, 0);
    std::string coords = "(" + std::to_string(static_cast<int>(playerX)) + "," + std::to_string(static_cast<int>(playerY)) + ")";
    drawText(playerX, playerY - 20, coords.c_str());
}
void drawRays() {
    for (int i = 0; i < numRays; i++) {
        glColor3f(1, 0, 0);
        glPointSize(9);
        glBegin(GL_LINES);

        glVertex2f(playerX, playerY);
        glVertex2f(rays[i].x, rays[i].y);
        glEnd();
        glPointSize(10);
        glColor3f(0, 0, 1);
        glBegin(GL_POINTS);
        glVertex2f(rays[i].x, rays[i].y);
        glEnd();
    }
}

void drawScene() {
    int lineWidth = width / numRays;
    for (int i = 0; i < numRays; i++) {
        // float mag = cos(rayAngle-playerAngle)*magnitude(rays[i]);
        // float mag = height - magnitude(rays[i]);
        // float rayAngle = std::atan2<float>(radians*(rays[i].y2 - rays[i].y, rays[i].x2 - rays[i].x));constexpr float PI = 3.14159265359f;

        float rayAngle = std::atan2(rays[i].y2 - rays[i].y, rays[i].x2 - rays[i].x);
        float correctedDist = magnitude(rays[i]) * cos(rayAngle - playerAngle * radians);
        float wallHeight = height * 100.0f / correctedDist;
        float yOffsets = (height - wallHeight) / 2;
        // int color = static_cast<int>((static_cast<int>(mag) % 255));
        float color = static_cast<float>((static_cast<int>(magnitude(rays[i])) % 255)) / 255;
        // printtheen("color: ", color);
        // float yOffsets = (height - mag) / 2;
        glColor3f(color, color, color);
        glLineWidth(lineWidth);
        glBegin(GL_LINES);
        glVertex2f(sceneOffset + i * lineWidth, yOffsets);
        glVertex2f(sceneOffset + i * lineWidth, height - yOffsets);
        glEnd();
    }
}

void processRays() {
    float xOffset, yOffset;
    int curIters = 0;
    float rayAngle;
    int gridX, gridY;
    // glPointSize(8.0);

    // THE PROBLEM IS IT DOESNT KNOW IF YOU ALREADY WROTE THE SHORTEST, ONLY CHANGE THE ARRAY IF ITS ACTUALLY THE BEST ONE
    for (int i = 0; i < numRays; i++) {
        rayAngle = playerAngle - (fov / 2.0f) + rayAngleStep * i;

        // printtheen("", rayAngleStep);
        // rayAngle = playerAngle - (fov / 2.0f) + rayAngleStep * i;
        //
        rayAngle = std::fmod(std::fmod(rayAngle, 360.0f) + 360.0f, 360.0f);
        // rayAngle = playerAngle - (fov / 2) + rayAngleStep * i;
        // rayAngle = ((rayAngle % 361) + 361) % 360;
        // if (rayAngle < 0) {
        //     rayAngle = 360;
        // }
        // if (playerAngle > 360) {
        //     rayAngle = 0;
        // }
        // Ray ray = rays[i];
        // Ray ray2 = rays[i];
        Ray rayVert;
        Ray rayHor;

        rayVert.y = playerY;
        rayVert.x = playerX;
        rayHor.y = playerY;
        rayHor.x = playerX;
        rayVert.x2 = playerX;
        rayVert.y2 = playerY;
        rayHor.x2 = playerX;
        rayHor.y2 = playerY;
        curIters = 0;

        // VERTICAL LINES
        float nTan = -tan(rayAngle * radians);
        if (rayAngle == 90 || rayAngle == 270) {
            // THE HORIZONTAL IS CORRECT
            rayVert.x = playerX;
            rayVert.y = playerY;
            curIters = maxIters;
        }
        if (rayAngle < 90 || rayAngle > 270) {
            rayVert.x = roundToNearest(playerX, res) + res;
            rayVert.y = (playerX - rayVert.x) * nTan + playerY;
            xOffset = res;
            yOffset = -xOffset * nTan;
        }
        // left side
        if (rayAngle > 90 && rayAngle < 270) {
            rayVert.x = roundToNearest(rayVert.x, res);
            rayVert.y = (playerX - rayVert.x) * nTan + playerY;
            xOffset = -res;
            yOffset = -xOffset * nTan;
        }
        while (curIters < maxIters) {
            gridX = static_cast<int>(std::floor(rayVert.x / res));
            gridY = static_cast<int>(std::floor(rayVert.y / res));
            // glColor3f(0, 1, 0);
            // glBegin(GL_POINTS);
            //
            // glVertex2f(ray.x, ray.y);
            // glEnd();

            if (rayAngle > 90 && rayAngle < 270) {
                gridX -= 1;
            }
            // if (rayAngle > 180) {
            //     gridX -= 1;
            // }
            if (gridY >= 0 && gridY < rows && grid[gridY][gridX]) {
                break; // DEBUG  VERTICAL HITTING A BLOCK
            } else {
                rayVert.y += yOffset;
                rayVert.x += xOffset;

                curIters++;
            }
        }
        curIters = 0;
        // HORIZONTAL LINES

        rayHor.x = playerX;
        rayHor.y = playerY;
        float aTan = -1 / tan(rayAngle * radians);
        if (rayAngle == 0 || rayAngle == 360 || rayAngle == 180) {
            rayHor.x = playerX;
            rayHor.y = playerY;
            // shortestRays[i] = rayVert;
            // rays[i] = rayVert;
            curIters = maxIters;
        }
        if (rayAngle > 180) {
            rayHor.y = roundToNearest(rayHor.y, res);
            rayHor.x = (playerY - rayHor.y) * aTan + playerX;
            yOffset = -res;
            xOffset = -yOffset * aTan;
        }
        if (rayAngle < 180) {
            rayHor.y = roundToNearest(playerY, res) + res;
            rayHor.x = (playerY - rayHor.y) * aTan + playerX;
            yOffset = res;
            xOffset = -yOffset * aTan;
        }
        while (curIters < maxIters) {
            gridX = static_cast<int>(std::floor(rayHor.x / res));
            gridY = static_cast<int>(std::floor(rayHor.y / res));
            // glColor3f(0, 1, 0);
            // glBegin(GL_POINTS);
            //
            // glVertex2f(ray2.x, ray2.y);
            // glEnd();
            if (rayAngle > 180) {
                gridY -= 1;
            }
            if (gridX >= 0 && gridX < cols && grid[gridY][gridX]) {

                break;
            } else {
                rayHor.y += yOffset;
                rayHor.x += xOffset;

                curIters++;
            }
        }

        if (magnitude(rayVert) < magnitude(rayHor)) {
            // rays[i] = Ray{.x = 0, .y = 0, .x2 = 0, .y2 = 0};
            rays[i] = rayVert;
            // glColor3f(0, 0, 0);
            // glPointSize(9);
            // glBegin(GL_LINES);
            //
            // glVertex2f(playerX, playerY);
            // glVertex2f(rays[i].x, rays[i].y);
            // glEnd();
            // glPointSize(10);
            // glColor3f(0, 0, 1);
            // glBegin(GL_POINTS);
            // glVertex2f(rays[i].x, rays[i].y);
            // glEnd();

        } else {
            rays[i] = rayHor;
            // glColor3f(1, 0, 0);
            // glPointSize(9);
            // glBegin(GL_LINES);
            //
            // glVertex2f(playerX, playerY);
            // glVertex2f(rays[i].x, rays[i].y);
            // glEnd();
            // glPointSize(10);
            // glColor3f(0, 0, 1);
            // glBegin(GL_POINTS);
            // glVertex2f(rays[i].x, rays[i].y);
            // glEnd();

            // printtheen("--------------------------\n :", i);
            // printtheen("---\nAngle :", rayAngle);
        }
        if (rayAngle == 90 || rayAngle == 270) {
            rays[i] = rayHor;
            // printtheen("--------------------------\nX:", rays[i].x);
            // printtheen("--------------------------\nY:", rays[i].y);
            // glColor3f(0, 1, 0);
            // glPointSize(9);
            // glBegin(GL_LINES);
            //
            // glVertex2f(playerX, playerY);
            // glVertex2f(rays[i].x, rays[i].y);
            // glEnd();
            // glPointSize(10);
            // glColor3f(0, 0, 1);
            // glBegin(GL_POINTS);
            // glVertex2f(rays[i].x, rays[i].y);
            // glEnd();
            // shortestRays[i] = rayHor;
            // rays[i] = Ray{.x = 0, .y = 0, .x2 = 0, .y2 = 0};
        } else if (rayAngle == 0 || rayAngle == 360 || rayAngle == 180) {
            rays[i] = rayVert;
            // glColor3f(1, 0, 1);
            // glPointSize(9);
            // glBegin(GL_LINES);
            //
            // glVertex2f(playerX, playerY);
            // glVertex2f(rays[i].x, rays[i].y);
            // glEnd();
            // glPointSize(10);
            // glColor3f(0, 0, 1);
            // glBegin(GL_POINTS);
            // glVertex2f(rays[i].x, rays[i].y);
            // glEnd();
            // printtheen("--------------------------\nAngle :", rayAngle);
            // printtheen("\n rayVert X:", rayVert.x);
            // printtheen("\n rayVert Y:", rayVert.y);
            // rays[i] = Ray{.x = 0, .y = 0, .x2 = 0, .y2 = 0};
        }
        // rays[i] = rayHor;
        // glColor3f(1, 0, 0);
        // glBegin(GL_POINTS);
        // glVertex2f(ray2.x, ray2.y);
        // glEnd();
        //
        // glColor3f(1, 0, 0);
        // glBegin(GL_POINTS);
        // glVertex2f(ray.x, ray.y);
        // glEnd();

        // if (magnitude(rayVert) < magnitude(rayHor)) {
        // shortestRays[i] = rayVert;
        // rays[i] = rayVert;
        // printtheen("ray1: ", magnitude(ray));
        // printtheen("ray2: ", magnitude(ray));
        // }
        // shortestRays[i] = rayHor;
        // printtheen("rayx: ", magnitude(ray));
    }
    // float rayAngle = playerAngle;
    // float yOffset;
    // float xOffset;
    //
    // for (int i = 0; i < numRays; i++) {
    //     Ray ray = rays[i];
    //     float arcTan = -1 / tan(rayAngle);
    //     if (rayAngle == 180 || rayAngle == 360 || rayAngle == 0) {
    //         ray.y = playerY;
    //         ray.x = playerX;
    //     } else if (rayAngle < 180) {
    //
    //         ray.y = roundToNearest(playerY, res);
    //         ray.x = (playerY - ray.y) * arcTan + playerX;
    //
    //         // printtheen("", ray.x);
    //         yOffset = -64;
    //         xOffset = -yOffset * arcTan;
    //     } else if (rayAngle > 180) {
    //         ray.y = roundToNearest(playerY, res) + res;
    //         ray.x = (playerY - ray.y) * arcTan + playerX;
    //         yOffset = 64;
    //         xOffset = yOffset * arcTan;
    //     }
    //     glColor3f(0, 1, 0);
    //     glBegin(GL_POINTS);
    //     glVertex2f(ray.x, ray.y);
    //     glEnd();
    // }
    // return shortestRays;
}
//
//
// SETUP -----------------------------

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    GLFWwindow *window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr);

    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    framebuffer_size_callback(window, width, height);

    // ------------------------SETUP HERE------------------------
    //
    //

    // Create Random Grid
    // srand(time(nullptr));
    // for (int i = 0; i < rows; i++) {
    //     for (int j = 0; j < cols; j++) {
    //         grid[i][j] = 95 < (rand() % 100);
    //     }
    // }

    for (int i = 0; i < numRays; i++) {
        rays[i] = Ray{.x = playerX, .y = playerY, .x2 = playerX, .y2 = playerY};
    }
    // make sure center four squares are not blocks
    grid[static_cast<int>(rows / 2) - 1][static_cast<int>(cols / 2) - 1] = 0;
    grid[static_cast<int>(rows / 2) - 1][static_cast<int>(cols / 2)] = 0;
    grid[static_cast<int>(rows / 2)][static_cast<int>(cols / 2) - 1] = 0;
    grid[static_cast<int>(rows / 2)][static_cast<int>(cols / 2)] = 0;

    //
    //
    // ------------------------SETUP HERE------------------------

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // ------------------------LOOP HERE------------------------
        //
        //
        // glutSpecialFunc(processKeypress);
        //
        drawGrid(grid);
        drawPlayer(playerX, playerY, playerSize, playerAngle);
        processKeypress(*window);
        processRays();
        drawRays();
        drawScene();
        // glColor3f(1, 0, 0);
        // glPointSize(9);
        // glBegin(GL_LINES);

        // glVertex2f(playerX, playerY);
        // glVertex2f(shortest.x, shortest.y);
        // glEnd();
        // glPointSize(10);
        // glColor3f(0, 0, 1);
        // glBegin(GL_POINTS);
        // glVertex2f(shortest.x, shortest.y);
        // glEnd();

        //
        //
        // ------------------------LOOP HERE------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
