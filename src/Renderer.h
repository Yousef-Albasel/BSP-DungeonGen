#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include "Camera.h"
#include "Character.h"
#include "Terrain.h"
// DISABLED: #include "Water.h"
#include "Sky.h"
#include "Vendor/Shader.h"
// REMOVED: #include "Vendor/Model.h"
// DISABLED: #include "NetworkProtocol.h"
// REMOVED: #include "engine/Inventory.h"
// REMOVED: #include "engine/WorldItem.h"

class Shader;
// DISABLED: #include "engine/PostProcess.h"

class Shader;
// DISABLED: class FoliageManager;

enum class CameraMode {
    ThirdPerson,
    FirstPerson
};

// DISABLED: Networking
// struct RemotePlayerData {
//     glm::vec3 targetPosition;
//     glm::vec3 previousPosition;
//     float targetRotation;
//     float previousRotation;
//     float interpolationProgress;
// };

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
    
    // DISABLED: void OnNetworkPacket(const PacketHeader* header, const unsigned char* data);
    
    Camera& GetCamera() { return camera; }

private:
    // DISABLED: void InitShadowMap();
    // DISABLED: void RenderShadowPass();
    
    std::unique_ptr<Character> character;
    std::unique_ptr<Terrain> terrain;
    // DISABLED: Water* water = nullptr;
    Sky sky;
    // DISABLED: FoliageManager* foliage = nullptr;
    
    std::unique_ptr<Shader> characterShader;
    std::unique_ptr<Shader> terrainShader;
    // DISABLED: Shader* waterShader = nullptr;
    std::unique_ptr<Shader> skyShader;
    
    // DISABLED: PostProcess* postProcess = nullptr;
    
    // DISABLED: Shadow mapping
    // Shader* shadowDepthShader = nullptr;
    // GLuint shadowMapFBO = 0;
    // GLuint shadowMapTexture = 0;
    // glm::mat4 lightSpaceMatrix;
    // const unsigned int SHADOW_WIDTH = 2048;
    // const unsigned int SHADOW_HEIGHT = 2048;
    
    // DISABLED: Remote players (multiplayer)
    // std::map<unsigned int, Character*> remotePlayers;
    // std::map<unsigned int, RemotePlayerData> remotePlayerData;
    
    // Camera
    Camera camera;
    CameraMode cameraMode = CameraMode::ThirdPerson;
    float cameraDistance = 8.0f;
    float cameraHeight = 1.7f;
    // Input state
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