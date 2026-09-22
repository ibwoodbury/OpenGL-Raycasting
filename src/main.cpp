#include <GL/gl.h>
#include <GLFW/glfw3.h>

// #include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>
#include <fstream>
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
const bool debugging = false;
const int rows = 20;
const int cols = 20;
const int numMaps = 4;
int curMap = 0;
int maps[numMaps][rows][cols];
const int res = 30;
const int numRays = 300;
const float fov = 100;
const int padding = 100;
const int portal = 5;
const float rayAngleStep = fov / numRays;

const int width = res * cols;
const int height = res * rows;
const int lineWidth = 2 * width / numRays;
const int sceneOffset = cols * res + padding + lineWidth / 2;
float radians = M_PI / 180.0;
const int maxIters = static_cast<int>(std::sqrt(cols * cols + rows * rows) * 0.80); // make this mathmatically correct, manhattan dist?
float value;
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

int grid[rows][cols];
Ray rays[numRays];

const int numLines = 3;
Ray lines[numLines];
bool loadMaps(const std::string &filename, int maps[][rows][cols]) {
    std::ifstream file(filename);

    if (!file)
        return false;

    for (int map = 0; map < numMaps; map++) {
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                if (!(file >> maps[map][row][col]))
                    return false;
            }
        }
    }

    return true;
}

void fillGrid() {
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            grid[row][col] = maps[curMap][row][col];
        }
    }
    // for (int row = 0; row < rows; row++) {
    //     for (int col = 0; col < cols; col++) {
    //         std::cout << grid[row][col] << ' ';
    //     }
    //     std::cout << '\n';
    // }
}
float magnitude(Ray ray) { return sqrtf((ray.x - ray.x2) * (ray.x - ray.x2) + (ray.y - ray.y2) * (ray.y - ray.y2)); }
int roundToNearest(float num, int to) { return static_cast<int>(std::floor(num / to) * to); }

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
    if (debugging) {

        drawText(0, 0, coords.c_str());
    }
    glPopMatrix();
    if (glfwGetKey(&window, GLFW_KEY_UP) == GLFW_PRESS) {
        // if (glfwGetKayerSpeed;
        // SIN AND COS ARE REVERSED BECAUSE BETWEEN THEM THERE IS A PHASE SHIFT OF 90 DEG, and I am countup up (90 deg) as 0
        // deg
        if (grid[gridY][gridX] == 0 || grid[gridY][gridX] == portal) {
            playerY += playerSpeed * sin(playerAngle * radians);
            playerX += playerSpeed * cos(playerAngle * radians);
            if (grid[gridY][gridX] == portal && curMap + 1 < numMaps) {
                curMap += 1;
                fillGrid();
                printtheen("", curMap);
            }
        }
    }
    if (glfwGetKey(&window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        playerAngle -= playerSpeed;
    }
    if (glfwGetKey(&window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        playerX -= playerSpeed * cos(playerAngle * radians);
        playerY -= playerSpeed * sin(playerAngle * radians);
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
void gridColor(int color, bool horizontal_for_colors_only) {
    switch (color) {
    case 1:
        horizontal_for_colors_only ? glColor3f(0, 0, 0) : glColor3f(0.1, 0.1, 0.1);
        break;
    case 2:
        horizontal_for_colors_only ? glColor3f(0.9, 0, 0) : glColor3f(1, 0, 0);
        break;
    case 3:
        horizontal_for_colors_only ? glColor3f(0, 0.9, 0) : glColor3f(0, 1, 0);
        break;
    case 4:
        horizontal_for_colors_only ? glColor3f(0, 0, 0.9) : glColor3f(0, 0, 1);
        break;
    case 6:
        // add seeing thru later if alpha value is under a threshold, maybe walk thru
        horizontal_for_colors_only ? glColor4f(value * 0.9, value * 0.9, value * 0.9, 1 - value) : glColor4f(value * 0.9, value * 0.9, value * 0.9, 1 - value);
        break;
    case portal:
        horizontal_for_colors_only ? glColor3f(value * 0.9, 1 * 0.10, 0 * 0.10) : glColor3f(value, 1 * 0.20, 0 * 0.20);
        break;
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
                gridColor(grid[i][j], false);
                // glColor3f(0, 0, 0);
                glBegin(GL_QUADS);
                glVertex2f(j * res, i * res);
                glVertex2f(j * res + res, i * res);
                glVertex2f(j * res + res, i * res + res);
                glVertex2f(j * res, i * res + res);
                glEnd();
            }

            glColor3f(0, 0, 0);
            glLineWidth(1.0f);
            glBegin(GL_LINE_LOOP);
            glVertex2f(j * res, i * res);
            glVertex2f(j * res + res, i * res);
            glVertex2f(j * res + res, i * res + res);
            glVertex2f(j * res, i * res + res);

            glEnd();
            // glColor3f(0, 0, 0);
            if (debugging) {
                std::string coords = "(" + std::to_string(j) + "," + std::to_string(i) + ")";
                drawText(j * res + 5, i * res + 10, coords.c_str());
            }
        }
    }
}

void drawLines() {
    for (int i = 0; i < numLines; i++) {
        glColor3f(0, 0, 0);
        glBegin(GL_LINES);
        glVertex2f(lines[i].x, lines[i].y);
        glVertex2f(lines[i].x2, lines[i].y2);
        glEnd();
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
    glVertex2f(-center, -center);
    glVertex2f(-center, center);
    glVertex2f(center, center);
    glVertex2f(center, -center);

    glEnd();
    glBegin(GL_LINES);
    glVertex2f(center, 0);
    glVertex2f(size, 0);
    glEnd();
    glTranslatef(-x, -y, 0); // move back
    glPopMatrix();
    glColor3f(1, 0, 0);
    if (debugging) {
        std::string coords = "(" + std::to_string(static_cast<int>(playerX)) + "," + std::to_string(static_cast<int>(playerY)) + ")";
        drawText(playerX, playerY - 20, coords.c_str());
    }
}
bool intersects(const Ray &ray, const Ray &line, float &distance) {
    float x1 = ray.x;
    float y1 = ray.y;
    float x2 = ray.x2;
    float y2 = ray.y2;

    float x3 = line.x;
    float y3 = line.y;
    float x4 = line.x2;
    float y4 = line.y2;

    float denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    if (denominator == 0)
        return false;

    float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denominator;

    float u = ((x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2)) / denominator;

    if (t < 0 || t > 1 || u < 0 || u > 1)
        return false;

    // Intersection point
    float ix = x1 + t * (x2 - x1);
    float iy = y1 + t * (y2 - y1);

    // Distance from ray start to intersection
    float dx = ix - x1;
    float dy = iy - y1;

    distance = std::sqrt(dx * dx + dy * dy);

    return true;
}

void drawRay(Ray ray) {
    glLineWidth(5);
    // for (int i = 0; i < numRays; i++) {
    // glColor3f(1, 0, 0);
    glBegin(GL_LINES);

    glVertex2f(playerX, playerY);
    glVertex2f(ray.x, ray.y);
    // glVertex2f(ray[i].x, rays[i].y);
    glEnd();
    glPointSize(10);
    glColor3f(1, 1, 0);
    glBegin(GL_POINTS);
    glVertex2f(ray.x, ray.y);
    // glVertex2f(rays[i].x, rays[i].y);
    glEnd();
    // }
}

// void drawScene() {
//     int lineWidth = width / numRays;
//     for (int i = 0; i < numRays; i++) {
//
//         float rayAngle = std::atan2(rays[i].y2 - rays[i].y, rays[i].x2 - rays[i].x);
//         float correctedDist = magnitude(rays[i]) * cos(rayAngle - playerAngle * radians);
//         float wallHeight = height * 100.0f / correctedDist;
//         float yOffsets = (height - wallHeight) / 2;
//         float color = static_cast<float>((static_cast<int>(magnitude(rays[i])) % 255)) / 255;
//         glColor3f(color, color, color);
//         glLineWidth(lineWidth);
//         glBegin(GL_LINES);
//         glVertex2f(sceneOffset + i * lineWidth, yOffsets);
//         glVertex2f(sceneOffset + i * lineWidth, height - yOffsets);
//         glEnd();
//     }
// }

void processRays() {
    float xOffset, yOffset;
    int curIters = 0;
    float rayAngle;
    int gridXVert, gridYVert;
    int gridXHor, gridYHor;
    int color = 0;
    bool horizontal_for_colors_only;

    // THE PROBLEM IS IT DOESNT KNOW IF YOU ALREADY WROTE THE SHORTEST, ONLY CHANGE THE ARRAY IF ITS ACTUALLY THE BEST ONE
    for (int i = 0; i < numRays; i++) {
        rayAngle = playerAngle - (fov / 2.00f) + rayAngleStep * i;

        //
        rayAngle = std::fmod(std::fmod(rayAngle, 360.0f) + 360.0f, 360.0f);
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
            gridXVert = static_cast<int>(std::floor(rayVert.x / res));
            gridYVert = static_cast<int>(std::floor(rayVert.y / res));
            if (rayAngle > 90 && rayAngle < 270) {
                gridXVert -= 1;
            }
            if (gridYVert >= 0 && gridYVert < rows && grid[gridYVert][gridXVert]) {
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
            gridXHor = static_cast<int>(std::floor(rayHor.x / res));
            gridYHor = static_cast<int>(std::floor(rayHor.y / res));
            value = (1.0f - std::cos(glfwGetTime() * 2.0 * M_PI / 2.0)) / 2.0f;
            if (rayAngle > 180) {
                gridYHor -= 1;
            }
            if (gridXHor >= 0 && gridXHor < cols && grid[gridYHor][gridXHor]) {

                break;
            } else {
                rayHor.y += yOffset;
                rayHor.x += xOffset;

                curIters++;
            }
        }
        // printtheen("GridXHor", gridXHor);
        // printtheen("GridYHor", gridYHor);
        // printtheen("GridXVert", gridXVert);
        // printtheen("GridYVert", gridYVert);
        // color = grid[gridY][gridX];
        if (magnitude(rayVert) < magnitude(rayHor)) {
            rays[i] = rayVert;
            if (gridYVert >= 0 && gridYVert < rows) {
                color = grid[gridYVert][gridXVert];
                horizontal_for_colors_only = false;
                // printtheen("grid[]", grid[gridYVert][gridXVert]);
            }

        } else {
            rays[i] = rayHor;
            if (gridXHor >= 0 && gridXHor < cols) {

                horizontal_for_colors_only = true;
                color = grid[gridYHor][gridXHor];
                // printtheen("grid[]", grid[gridYHor][gridXHor]);
            }
        }
        if (rayAngle == 90 || rayAngle == 270) {
            rays[i] = rayHor;
            if (gridXHor >= 0 && gridXHor < cols) {
                // printtheen("grid[]", grid[gridYHor][gridXHor]);
                horizontal_for_colors_only = true;
                color = grid[gridYHor][gridXHor];
            }
        } else if (rayAngle == 0 || rayAngle == 360 || rayAngle == 180) {
            rays[i] = rayVert;
            if (gridYVert >= 0 && gridYVert < rows) {

                horizontal_for_colors_only = false;
                color = grid[gridYVert][gridXVert];
                // printtheen("grid[]", grid[gridYVert][gridXVert]);
            }
        }
        float rayMagnitude = magnitude(rays[i]);
        // float shortestLineMagnitude = 99999999.0;
        // for (int j = 0; j < numLines; j++) {
        //     float magnitudeLine;
        //     if (intersects(rays[i], lines[j], magnitudeLine)) {
        //         // printtheen("", magnitudeLine);
        //         magnitudeLine < shortestLineMagnitude ? shortestLineMagnitude = magnitudeLine : 0;
        //     }
        // }
        // if (shortestLineMagnitude < rayMagnitude) {
        //     rayMagnitude = shortestLineMagnitude;
        // }

        gridColor(color, horizontal_for_colors_only);

        float rayAngle = std::atan2(rays[i].y2 - rays[i].y, rays[i].x2 - rays[i].x);
        float correctedDist = rayMagnitude * cos(rayAngle - playerAngle * radians);

        float wallHeight = height * 100.0f / correctedDist;
        float yOffsets = (height - wallHeight) / 2;
        // float color = static_cast<float>((static_cast<int>(magnitude(rays[i])) % 255)) / 255;
        // glColor3f(color, color, color);
        glLineWidth(lineWidth);
        glBegin(GL_LINES);
        glVertex2f(sceneOffset + i * lineWidth, padding + yOffsets);
        glVertex2f(sceneOffset + i * lineWidth, padding + height - yOffsets);
        glEnd();
        drawRay(rays[i]);
    }
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
    if (!loadMaps("./src/maps.txt", maps)) {
        std::cerr << "Failed to load maps\n";
        return 1;
    }
    for (int map = 0; map < numMaps; map++) {
        std::cout << "\nMAP " << map << ":\n";

        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                std::cout << maps[map][row][col] << ' ';
            }
            std::cout << '\n';
        }
    }
    fillGrid();
    // Create Random Grid
    srand(time(nullptr));
    for (int i = 0; i < numLines; i++) {
        lines[i] = Ray{.x = res + static_cast<float>(rand() % width) - res,
                       .y = res + static_cast<float>(rand() % height) - res,
                       .x2 = res + static_cast<float>(rand() % width) - res,
                       .y2 = res + static_cast<float>(rand() % height) - res};
    }
    // for (int i = 0; i < numLines; i++) {
    //     rays[i] = Ray{.x = playerX, .y = playerY, .x2 = playerX, .y2 = playerY};
    // }
    // make sure center four squares are not blocks
    // grid[static_cast<int>(rows / 2) - 1][static_cast<int>(cols / 2) - 1] = 0;
    // grid[static_cast<int>(rows / 2) - 1][static_cast<int>(cols / 2)] = 0;
    // grid[static_cast<int>(rows / 2)][static_cast<int>(cols / 2) - 1] = 0;
    // grid[static_cast<int>(rows / 2)][static_cast<int>(cols / 2)] = 0;

    //
    //
    // ------------------------SETUP HERE------------------------

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // ------------------------LOOP HERE------------------------
        //
        //
        //
        drawGrid(grid);
        drawPlayer(playerX, playerY, playerSize, playerAngle);
        processKeypress(*window);
        processRays();
        // drawLines();
        // drawRays();
        // drawScene();
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
