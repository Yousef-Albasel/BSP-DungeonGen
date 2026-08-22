#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include "Vendor/Camera.h"
#include "Vendor/Shader.h"
class Shader;
class Shader;

class Renderer {
public:
    Renderer();
    ~Renderer();

    void Init();
    void Render();
    void Clean();
    
    void processKeyboardInput(GLFWwindow* window, float deltaTime);
    void ProcessMouseInput(GLFWwindow* window, const float& dt);
    
    void RenderDebugUI(float deltaTime);
    Camera& GetCamera() { return camera; }

private:
    // Camera
    Camera camera;
    float cameraDistance = 8.0f;
    float cameraHeight = 1.7f;
    bool showDebugWindow = false;
    bool enableMouseLook = false;
    bool firstMouse = true;
    double mouseX = 0.0, mouseY = 0.0;
    double lastX = 640.0, lastY = 360.0;
    float mouseSensitivity = 0.1f;
    float movementSpeed = 50.0f;
    // Debug stats
    float fpsTimer = 0.0f;
    int frameCount = 0;
    float currentFPS = 0.0f;
    
};