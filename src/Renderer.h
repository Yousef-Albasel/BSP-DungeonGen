#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include "Vendor/Camera.h"
#include "Vendor/Shader.h"
#include "level/Level.h"
#include "level/Mesh.h"
class Shader;

class Texture;
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
    std::unique_ptr<Shader> wallShader;
    std::unique_ptr<Shader> floorShader;
    std::unique_ptr<Level> level;
    std::unique_ptr<Mesh> levelMesh;
    std::unique_ptr<Mesh> corridorMesh;
    std::unique_ptr<Mesh> wallMesh;
    std::unique_ptr<Texture> floorTexture;
    std::unique_ptr<Texture> floorNormalTexture;
    std::unique_ptr<Texture> wallTexture;
    std::unique_ptr<Texture> wallNormalTexture;
    // Camera
    Camera camera;
    float cameraDistance = 8.0f;
    float cameraHeight = 1.7f;
    bool showDebugWindow = false;
    bool enableMouseLook = false;
    bool firstMouse = true;
    double mouseX = 0.0, mouseY = 0.0;
    double lastX = 640.0, lastY = 360.0;
    float mouseSensitivity = 0.7f;
    float movementSpeed = 50.0f;
    // Debug stats
    float fpsTimer = 0.0f;
    int frameCount = 0;
    float currentFPS = 0.0f;
    glm::vec3 lightPosition = {10.0f, 10.0f, 10.0f};
    glm::vec3 lightColor = {1.0f, 1.0f, 1.0f};
    glm::vec3 ambientColor = {0.2f, 0.2f, 0.2f};

    float shininess = 32.0f;
    float specularStrength = 0.5f;
};