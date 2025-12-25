#ifndef GRID_CLASS_H
#define GRID_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include "shader.h"
#include "Camera.h"

class Grid {
private:
    GLuint gridVAO, gridVBO;
    GLuint axisVAO, axisVBO;
    int gridVertexCount;
    int axisVertexCount;

public:
    Grid(float size = 20.0f, int divisions = 20) {
        // Generate grid lines
        std::vector<float> gridVertices;
        float step = size / divisions;
        float halfSize = size / 2.0f;

        // Grid color (grey)
        glm::vec3 gridColor(0.3f, 0.3f, 0.3f);

        // Lines parallel to X-axis
        for (int i = 0; i <= divisions; i++) {
            float z = -halfSize + i * step;

            // Skip center lines (they will be drawn as axes)
            if (i == divisions / 2) continue;

            // Dim the lines further from center
            float fade = 1.0f - (abs(z) / halfSize) * 0.5f;
            glm::vec3 color = gridColor * fade;

            // Line along X
            gridVertices.push_back(-halfSize); gridVertices.push_back(0.0f); gridVertices.push_back(z);
            gridVertices.push_back(color.r); gridVertices.push_back(color.g); gridVertices.push_back(color.b);

            gridVertices.push_back(halfSize); gridVertices.push_back(0.0f); gridVertices.push_back(z);
            gridVertices.push_back(color.r); gridVertices.push_back(color.g); gridVertices.push_back(color.b);
        }

        // Lines parallel to Z-axis
        for (int i = 0; i <= divisions; i++) {
            float x = -halfSize + i * step;

            // Skip center lines (they will be drawn as axes)
            if (i == divisions / 2) continue;

            // Dim the lines further from center
            float fade = 1.0f - (abs(x) / halfSize) * 0.5f;
            glm::vec3 color = gridColor * fade;

            // Line along Z
            gridVertices.push_back(x); gridVertices.push_back(0.0f); gridVertices.push_back(-halfSize);
            gridVertices.push_back(color.r); gridVertices.push_back(color.g); gridVertices.push_back(color.b);

            gridVertices.push_back(x); gridVertices.push_back(0.0f); gridVertices.push_back(halfSize);
            gridVertices.push_back(color.r); gridVertices.push_back(color.g); gridVertices.push_back(color.b);
        }

        gridVertexCount = gridVertices.size() / 6;

        // Create grid VAO and VBO
        glGenVertexArrays(1, &gridVAO);
        glGenBuffers(1, &gridVBO);

        glBindVertexArray(gridVAO);
        glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
        glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Generate axis lines (X, Y, Z) - both positive and negative directions
        std::vector<float> axisVertices = {
            // X-axis (Red) - negative to positive
            -halfSize, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
            halfSize, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,

            // Y-axis (Green) - negative to positive
            0.0f, -halfSize, 0.0f,  0.0f, 1.0f, 0.0f,
            0.0f, halfSize, 0.0f,  0.0f, 1.0f, 0.0f,

            // Z-axis (Blue) - negative to positive
            0.0f, 0.0f, -halfSize,  0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, halfSize,  0.0f, 0.0f, 1.0f
        };

        axisVertexCount = 6;

        // Create axis VAO and VBO
        glGenVertexArrays(1, &axisVAO);
        glGenBuffers(1, &axisVBO);

        glBindVertexArray(axisVAO);
        glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
        glBufferData(GL_ARRAY_BUFFER, axisVertices.size() * sizeof(float), axisVertices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void Draw(Shader& shader, Camera& camera) {
        shader.use();

        // Set up matrices
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        camera.Matrix(shader, "camMatrix");

        // Draw grid
        glBindVertexArray(gridVAO);
        glLineWidth(1.0f);
        glDrawArrays(GL_LINES, 0, gridVertexCount);

        // Draw axes with thicker lines
        glBindVertexArray(axisVAO);
        glLineWidth(3.0f);
        glDrawArrays(GL_LINES, 0, axisVertexCount);

        glLineWidth(1.0f); // Reset line width
        glBindVertexArray(0);
    }

    ~Grid() {
        glDeleteVertexArrays(1, &gridVAO);
        glDeleteBuffers(1, &gridVBO);
        glDeleteVertexArrays(1, &axisVAO);
        glDeleteBuffers(1, &axisVBO);
    }
};

#endif