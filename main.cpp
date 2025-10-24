#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

const float M_PI = 3.14159265358979323846f;

// Simple 2D point structure
struct Point {
    float x, y;
    Point(float x, float y) : x(x), y(y) {}
};

// Color structure
struct Color {
    float r, g, b;
    Color(float r, float g, float b) : r(r), g(g), b(b) {}
};

// Player class
class Player {
public:
    Point position;
    Color color;
    float size;

    Player(float x, float y) : position(x, y), color(0.2f, 0.6f, 0.9f), size(0.03f) {}

    void draw() {
        glColor3f(color.r, color.g, color.b);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(position.x, position.y);
        for (int i = 0; i <= 20; i++) {
            float angle = 2.0f * M_PI * float(i) / 20.0f;
            glVertex2f(position.x + size * cos(angle), position.y + size * sin(angle));
        }
        glEnd();
    }
};

// Plant class
class Plant {
public:
    Point position;
    Color color;
    float growth;
    bool isGrown;

    Plant(float x, float y) : position(x, y), color(0.2f, 0.8f, 0.3f), growth(0.0f), isGrown(false) {}

    void draw() {
        if (growth < 0.3f) {
            // Seed
            glColor3f(0.5f, 0.35f, 0.05f);
            glBegin(GL_QUADS);
            glVertex2f(position.x - 0.01f, position.y - 0.01f);
            glVertex2f(position.x + 0.01f, position.y - 0.01f);
            glVertex2f(position.x + 0.01f, position.y + 0.01f);
            glVertex2f(position.x - 0.01f, position.y + 0.01f);
            glEnd();
        }
        else if (growth < 0.7f) {
            // Growing plant
            glColor3f(0.2f, 0.7f, 0.2f);
            glBegin(GL_TRIANGLES);
            glVertex2f(position.x, position.y + 0.02f);
            glVertex2f(position.x - 0.015f, position.y - 0.01f);
            glVertex2f(position.x + 0.015f, position.y - 0.01f);
            glEnd();
        }
        else {
            // Fully grown plant
            glColor3f(0.1f, 0.8f, 0.1f);
            glBegin(GL_TRIANGLES);
            glVertex2f(position.x, position.y + 0.035f);
            glVertex2f(position.x - 0.02f, position.y - 0.01f);
            glVertex2f(position.x + 0.02f, position.y - 0.01f);

            glColor3f(0.8f, 0.8f, 0.1f);
            glBegin(GL_QUADS);
            glVertex2f(position.x - 0.008f, position.y + 0.02f);
            glVertex2f(position.x + 0.008f, position.y + 0.02f);
            glVertex2f(position.x + 0.008f, position.y + 0.03f);
            glVertex2f(position.x - 0.008f, position.y + 0.03f);
            glEnd();
        }
    }

    void update() {
        if (growth < 1.0f) {
            growth += 0.001f;
            if (growth >= 1.0f) isGrown = true;
        }
    }
};

// Animal class
class Animal {
public:
    Point position;
    Color color;
    float size;
    bool isPredator;

    Animal(float x, float y, bool predator = false) :
        position(x, y),
        isPredator(predator),
        size(0.02f),
        color(predator ? Color(0.8f, 0.2f, 0.2f) : Color(0.8f, 0.6f, 0.4f))
    {}

    void draw() {
        glColor3f(color.r, color.g, color.b);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(position.x, position.y);
        for (int i = 0; i <= 10; i++) {
            float angle = 2.0f * M_PI * float(i) / 10.0f;
            glVertex2f(position.x + size * cos(angle), position.y + size * sin(angle));
        }
        glEnd();

        if (isPredator) {
            // Draw eyes
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_POINTS);
            glVertex2f(position.x - 0.008f, position.y + 0.008f);
            glVertex2f(position.x + 0.008f, position.y + 0.008f);
            glEnd();
        }
    }
};

// Game state
bool isDay = true;
float dayNightProgress = 0.0f;
std::vector<Plant> plants;
std::vector<Animal> animals;
Player player(0.0f, 0.0f);

// Draw a rounded rectangle
void drawRoundedRect(float x, float y, float width, float height, float radius, Color color) {
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // Center

    for (int i = 0; i <= 20; i++) {
        float angle = 2.0f * M_PI * float(i) / 20.0f;
        glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
    }
    glEnd();

    // Draw the main rectangle body
    glBegin(GL_QUADS);
    glVertex2f(x - width / 2 + radius, y - height / 2);
    glVertex2f(x + width / 2 - radius, y - height / 2);
    glVertex2f(x + width / 2 - radius, y + height / 2);
    glVertex2f(x - width / 2 + radius, y + height / 2);

    glVertex2f(x - width / 2, y - height / 2 + radius);
    glVertex2f(x + width / 2, y - height / 2 + radius);
    glVertex2f(x + width / 2, y + height / 2 - radius);
    glVertex2f(x - width / 2, y + height / 2 - radius);
    glEnd();

    // Draw the side rectangles
    glBegin(GL_QUADS);
    glVertex2f(x - width / 2, y - height / 2 + radius);
    glVertex2f(x - width / 2 + radius, y - height / 2 + radius);
    glVertex2f(x - width / 2 + radius, y + height / 2 - radius);
    glVertex2f(x - width / 2, y + height / 2 - radius);

    glVertex2f(x + width / 2 - radius, y - height / 2 + radius);
    glVertex2f(x + width / 2, y - height / 2 + radius);
    glVertex2f(x + width / 2, y + height / 2 - radius);
    glVertex2f(x + width / 2 - radius, y + height / 2 - radius);
    glEnd();
}

// Draw the UI
void drawUI() {
    // Draw day/night indicator
    float timeColor = isDay ? 0.9f : 0.3f;
    glColor3f(timeColor, timeColor, 0.1f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-0.9f, 0.9f);
    for (int i = 0; i <= 20; i++) {
        float angle = 2.0f * M_PI * float(i) / 20.0f;
        glVertex2f(-0.9f + 0.03f * cos(angle), 0.9f + 0.03f * sin(angle));
    }
    glEnd();

    // Draw inventory background
    drawRoundedRect(0.0f, -0.9f, 0.8f, 0.1f, 0.03f, Color(0.3f, 0.3f, 0.4f));

    // Draw inventory items
    glColor3f(0.5f, 0.35f, 0.05f);
    glBegin(GL_QUADS);
    glVertex2f(-0.3f, -0.93f);
    glVertex2f(-0.27f, -0.93f);
    glVertex2f(-0.27f, -0.87f);
    glVertex2f(-0.3f, -0.87f);
    glEnd();

    glColor3f(0.2f, 0.7f, 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.2f, -0.87f);
    glVertex2f(-0.23f, -0.93f);
    glVertex2f(-0.17f, -0.93f);
    glEnd();

    glColor3f(0.8f, 0.2f, 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.1f, -0.87f);
    glVertex2f(-0.13f, -0.93f);
    glVertex2f(-0.07f, -0.93f);
    glEnd();

    glColor3f(0.8f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, -0.93f);
    glVertex2f(0.03f, -0.93f);
    glVertex2f(0.03f, -0.87f);
    glVertex2f(0.0f, -0.87f);
    glEnd();

    glColor3f(0.4f, 0.4f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(0.1f, -0.93f);
    glVertex2f(0.13f, -0.93f);
    glVertex2f(0.13f, -0.87f);
    glVertex2f(0.1f, -0.87f);
    glEnd();
}

// Draw the game scene
void drawScene() {
    // Draw background based on time of day
    if (isDay) {
        glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    }
    else {
        glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw ground
    if (isDay) {
        glColor3f(0.6f, 0.5f, 0.2f);
    }
    else {
        glColor3f(0.3f, 0.25f, 0.1f);
    }
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, -0.6f);
    glVertex2f(-1.0f, -0.6f);
    glEnd();

    // Draw plants
    for (auto& plant : plants) {
        plant.draw();
    }

    // Draw animals
    for (auto& animal : animals) {
        animal.draw();
    }

    // Draw player
    player.draw();

    // Draw UI
    drawUI();
}

// Update game state
void updateGame() {
    // Update day/night cycle
    dayNightProgress += 0.0005f;
    if (dayNightProgress >= 1.0f) {
        dayNightProgress = 0.0f;
        isDay = !isDay;

        // Spawn predators at night
        if (!isDay) {
            animals.push_back(Animal(0.5f, -0.7f, true));
            animals.push_back(Animal(-0.5f, -0.7f, true));
        }
    }

    // Update plants
    for (auto& plant : plants) {
        plant.update();
    }

    // Randomly add plants for demonstration
    if (rand() % 1000 < 5 && plants.size() < 10) {
        plants.push_back(Plant((rand() % 100 - 50) / 100.0f, -0.7f));
    }

    // Randomly add animals for demonstration
    if (rand() % 1000 < 3 && animals.size() < 5) {
        animals.push_back(Animal((rand() % 100 - 50) / 100.0f, -0.7f));
    }
}

int main() {
    srand(time(0));

    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit()) {
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(800, 600, "FarmLife Adventure", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        updateGame();
        drawScene();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}