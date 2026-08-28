#include "Renderer.h"
#include "Vendor/imgui/imgui.h"
#include <filesystem>
#include <cmath>
#include <iostream>
#include <exception>
#include "level/Level.h"
#include "Vendor/Texture.h"
Renderer::Renderer() 
    : camera(glm::vec3(10.0f, 5.0f, 30.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
      wallShader(nullptr),
      floorShader(nullptr),
      level(nullptr),
      levelMesh(nullptr),
      corridorMesh(nullptr),
      wallMesh(nullptr),
      floorTexture(nullptr),
      floorNormalTexture(nullptr),
      wallTexture(nullptr),
      wallNormalTexture(nullptr)
{
}

Renderer::~Renderer() {
    Clean();
}

void Renderer::Init() {
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    glEnable(GL_DEPTH_TEST);
    
    level = std::make_unique<Level>(100, 100);
    levelMesh = std::make_unique<Mesh>(level->generateFloorMesh());
    floorTexture = std::make_unique<Texture>("../src/res/assets/textures/floor.png");
    floorNormalTexture = std::make_unique<Texture>("../src/res/assets/textures/floor_normal.png");
    corridorMesh = std::make_unique<Mesh>(level->generateCorridorMesh());
    wallMesh = std::make_unique<Mesh>(level->generateWallMesh());
    wallShader = std::make_unique<Shader>("../src/res/assets/shaders/wall.shader");
    floorShader = std::make_unique<Shader>("../src/res/assets/shaders/floor.shader");
    wallTexture = std::make_unique<Texture>(
        "../src/res/assets/textures/wall.png");
    wallNormalTexture = std::make_unique<Texture>(
        "../src/res/assets/textures/dungeon_wall_normal_map.png");
    

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

    if (wallShader && floorShader && levelMesh && corridorMesh && wallMesh) {
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

        // ---- Room / corridor floors ----
        floorShader->Bind();
        floorShader->setMVP(model, view, projection);
        floorShader->SetUniformMat4f("u_Model", model);

        floorShader->SetUniform3f(
            "u_LightPosition",
            lightPosition.x, lightPosition.y, lightPosition.z);

        floorShader->SetUniform3f(
            "u_LightColor",
            lightColor.x, lightColor.y, lightColor.z);

        floorShader->SetUniform3f(
            "u_AmbientColor",
            ambientColor.x, ambientColor.y, ambientColor.z);

        floorShader->SetUniform1f("u_Shininess", shininess);
        floorShader->SetUniform1f("u_SpecularStrength", specularStrength);

        floorShader->SetUniform3f(
            "u_ViewPosition",
            camera.getCameraPos().x, camera.getCameraPos().y, camera.getCameraPos().z);

        floorTexture->Bind(0);
        floorNormalTexture->Bind(1);
        floorShader->SetUniform1i("u_Texture", 0);
        floorShader->SetUniform1i("normalMap", 1);

        levelMesh->Bind();
        glDrawElements(GL_TRIANGLES, levelMesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
        levelMesh->Unbind();

        corridorMesh->Bind();
        glDrawElements(GL_TRIANGLES, corridorMesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
        corridorMesh->Unbind();

        floorTexture->Unbind();
        floorNormalTexture->Unbind();
        floorShader->Unbind();

        // ---- Walls ----
        wallShader->Bind();
        wallShader->setMVP(model, view, projection);
        wallShader->SetUniformMat4f("u_Model", model); // <-- was missing; fixes NaN tangent basis

        wallShader->SetUniform3f("u_LightPosition", lightPosition.x, lightPosition.y, lightPosition.z);
        wallShader->SetUniform3f("u_LightColor", lightColor.x, lightColor.y, lightColor.z);
        wallShader->SetUniform3f("u_AmbientColor", ambientColor.x, ambientColor.y, ambientColor.z);
        wallShader->SetUniform3f(
            "u_ViewPosition",
            camera.getCameraPos().x, camera.getCameraPos().y, camera.getCameraPos().z);
        wallShader->SetUniform1f("u_Shininess", shininess);
        wallShader->SetUniform1f("u_SpecularStrength", specularStrength);

        wallTexture->Bind(0);
        wallNormalTexture->Bind(1);
        wallShader->SetUniform1i("u_Texture", 0);
        wallShader->SetUniform1i("u_NormalMap", 1);

        wallMesh->Bind();
        glDrawElements(GL_TRIANGLES, wallMesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
        wallMesh->Unbind();

        wallTexture->Unbind();
        wallNormalTexture->Unbind();
        wallShader->Unbind();
    }
}
void Renderer::Clean() {
    // Must run before glfwDestroyWindow() / glfwTerminate().
    floorTexture.reset();
    floorShader.reset();
    wallShader.reset();

    wallMesh.reset();
    corridorMesh.reset();
    levelMesh.reset();

    level.reset();
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
    if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen))
{
    ImGui::DragFloat3(
        "Light Position",
        &lightPosition.x,
        0.1f);

    ImGui::ColorEdit3(
        "Light Color",
        &lightColor.x);

    ImGui::ColorEdit3(
        "Ambient",
        &ambientColor.x);

    ImGui::SliderFloat(
        "Specular Strength",
        &specularStrength,
        0.0f,
        2.0f);

    ImGui::SliderFloat(
        "Shininess",
        &shininess,
        1.0f,
        256.0f,
        "%.0f",
        ImGuiSliderFlags_Logarithmic);
    }

    ImGui::End();
}