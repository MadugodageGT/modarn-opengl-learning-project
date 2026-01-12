#ifndef ORBITCAMERA_H
#define ORBITCAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Default orbit camera values
const float DEFAULT_DISTANCE = 5.0f;
const float DEFAULT_AZIMUTH = 0.0f;
const float DEFAULT_ELEVATION = 20.0f;
const float ORBIT_SENSITIVITY = 0.2f;
const float PAN_SENSITIVITY = 0.0002f;
const float ZOOM_SENSITIVITY = 0.5f;
const float MIN_DISTANCE = 1.0f;
const float MAX_DISTANCE = 50.0f;
const float MIN_ELEVATION = -89.0f;
const float MAX_ELEVATION = 89.0f;

class OrbitCamera
{
public:
    // Orbit parameters
    float Distance;
    float Azimuth;      // Horizontal rotation (degrees)
    float Elevation;    // Vertical rotation (degrees)
    glm::vec3 Target;   // Point we're orbiting around

    // Camera options
    float OrbitSensitivity;
    float PanSensitivity;
    float ZoomSensitivity;

    // Constructor
    OrbitCamera(glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f),
        float distance = DEFAULT_DISTANCE,
        float azimuth = DEFAULT_AZIMUTH,
        float elevation = DEFAULT_ELEVATION)
        : Target(target), Distance(distance), Azimuth(azimuth), Elevation(elevation),
        OrbitSensitivity(ORBIT_SENSITIVITY), PanSensitivity(PAN_SENSITIVITY),
        ZoomSensitivity(ZOOM_SENSITIVITY)
    {
    }

    // Returns the view matrix
    glm::mat4 GetViewMatrix()
    {
        glm::vec3 position = CalculatePosition();
        return glm::lookAt(position, Target, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // Get camera position in world space (useful for lighting calculations later)
    glm::vec3 GetPosition()
    {
        return CalculatePosition();
    }

    // Process mouse dragging for orbit rotation
    void ProcessOrbitRotation(float xoffset, float yoffset)
    {
        xoffset *= OrbitSensitivity;
        yoffset *= OrbitSensitivity;

        Azimuth += xoffset;
        Elevation -= yoffset;

        // Constrain elevation to prevent gimbal lock
        if (Elevation > MAX_ELEVATION)
            Elevation = MAX_ELEVATION;
        if (Elevation < MIN_ELEVATION)
            Elevation = MIN_ELEVATION;

        // Keep azimuth in [0, 360) range (optional, but keeps values clean)
        while (Azimuth >= 360.0f) Azimuth -= 360.0f;
        while (Azimuth < 0.0f) Azimuth += 360.0f;
    }

    // Process mouse scroll for zoom
    void ProcessZoom(float yoffset)
    {
        Distance -= yoffset * ZoomSensitivity;

        if (Distance < MIN_DISTANCE)
            Distance = MIN_DISTANCE;
        if (Distance > MAX_DISTANCE)
            Distance = MAX_DISTANCE;
    }

    // Process mouse dragging for panning
    void ProcessPan(float xoffset, float yoffset)
    {
        // Get camera vectors
        glm::vec3 position = CalculatePosition();
        glm::vec3 viewDir = glm::normalize(Target - position);
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

        // Calculate right and up vectors from camera's perspective
        glm::vec3 right = glm::normalize(glm::cross(viewDir, worldUp));
        glm::vec3 up = glm::normalize(glm::cross(right, viewDir));

        // Pan sensitivity scales with distance (farther = faster pan)
        float panSpeed = PanSensitivity * Distance;

        // Move target based on camera's right and up vectors
        Target -= right * xoffset * panSpeed;
        Target -= up * yoffset * panSpeed;
    }

    // Set the target point to orbit around
    void SetTarget(glm::vec3 newTarget)
    {
        Target = newTarget;
    }


private:
    // Calculate camera position from spherical coordinates
    glm::vec3 CalculatePosition()
    {
        float azimuthRad = glm::radians(Azimuth);
        float elevationRad = glm::radians(Elevation);

        float x = Distance * cos(elevationRad) * cos(azimuthRad);
        float y = Distance * sin(elevationRad);
        float z = Distance * cos(elevationRad) * sin(azimuthRad);

        return Target + glm::vec3(x, y, z);
    }
};

#endif