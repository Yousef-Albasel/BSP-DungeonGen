#include "Renderer.h"
#include "Vendor/imgui/imgui.h"
#include <filesystem>
#include <cmath>
#include <iostream>
#include <exception>
#include "level/Level.h"
#include "level/RoomVisualizer.h"
Renderer::Renderer() 
    : camera(glm::vec3(10.0f, 5.0f, 30.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f))
{
}

Renderer::~Renderer() {
    Clean();
}

void Renderer::Init() {
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    Level level(50, 50);
    level.printRooms();
    // --- Bake the dungeon to an image ---
    RoomVisualizer viz;
    viz.cellSize   = 12;           // pixels per grid cell
    viz.drawGrid   = true;         // subtle grid lines
    viz.saturation = 0.55f;        // color richness
    viz.lightness  = 0.60f;        // color brightness
    viz.visualize(level.getBsp(), "dungeon_map.png");

    camera.setCameraPos(glm::vec3(10.0f, 5.0f, 30.0f));
}

void Renderer::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate delta time
    static float lastTime = 0.0f;
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    
    const float maxDeltaTime = 0.1f;
    if (deltaTime > maxDeltaTime) {
        deltaTime = maxDeltaTime;
    }

}

void Renderer::Clean() {
}

void Renderer::processKeyboardInput(GLFWwindow* window, float deltaTime) {
    float adjustedDeltaTime = deltaTime * (movementSpeed / 50.0f);

    static bool f1Pressed = false;
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS && !f1Pressed) {
        showDebugWindow = !showDebugWindow;
        f1Pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE) {
        f1Pressed = false;
    }

    if (showDebugWindow) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.updateKeyboardInput(adjustedDeltaTime, 0);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.updateKeyboardInput(adjustedDeltaTime, 1);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.updateKeyboardInput(adjustedDeltaTime, 3);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.updateKeyboardInput(adjustedDeltaTime, 2);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            camera.updateKeyboardInput(adjustedDeltaTime, 4);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera.updateKeyboardInput(adjustedDeltaTime, 5);
        }

        static bool f3Pressed = false;
        if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS && !f3Pressed) {
            enableMouseLook = !enableMouseLook;
            firstMouse = true;
            f3Pressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE) {
            f3Pressed = false;
        }

        if (enableMouseLook) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void Renderer::ProcessMouseInput(GLFWwindow* window, const float& dt) {
    glfwGetCursorPos(window, &mouseX, &mouseY);

    float xpos = static_cast<float>(mouseX);
    float ypos = static_cast<float>(mouseY);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (showDebugWindow) {
        if (enableMouseLook) {
            camera.updateMouseInput(dt, xoffset * mouseSensitivity, yoffset * mouseSensitivity);
        }
    }
    else {
        camera.updateMouseInput(dt, xoffset * mouseSensitivity, yoffset * mouseSensitivity);
    }
}

void Renderer::RenderDebugUI(float deltaTime) {
    if (!showDebugWindow) return;

    frameCount++;
    fpsTimer += deltaTime;
    if (fpsTimer >= 1.0f) {
        currentFPS = frameCount / fpsTimer;
        frameCount = 0;
        fpsTimer = 0.0f;
    }

    ImGui::Begin("Debug Info", &showDebugWindow);
    
    ImGui::Text("Press F1 to toggle this window");
    ImGui::Separator();

    ImGui::Text("FPS: %.1f", currentFPS);
    ImGui::Text("Frame Time: %.3f ms", deltaTime * 1000.0f);
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        glm::vec3 pos = camera.getCameraPos();
        ImGui::Text("Position: (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
        
        glm::vec3 front = camera.getCameraFront();
        ImGui::Text("Front: (%.2f, %.2f, %.2f)", front.x, front.y, front.z);
        
        ImGui::Separator();
        
        if (ImGui::Checkbox("Enable Mouse Look", &enableMouseLook)) {
            firstMouse = true;
        }
        
        ImGui::SliderFloat("Mouse Sensitivity", &mouseSensitivity, 0.01f, 1.0f);
        ImGui::SliderFloat("Movement Speed", &movementSpeed, 10.0f, 200.0f);
    }

    ImGui::End();
}