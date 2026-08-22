#include "Renderer.h"
#include "imgui/imgui.h"
#include <filesystem>
// DISABLED: #include "NetworkManager.h"
// DISABLED: #include "FoliageManager.h"
#include <cmath>
#include <exception>

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

    character = std::make_unique<Character>();
    character->Init();
    character->SetState(CharacterState::Idle);
    character->worldPosition = glm::vec3(0.0f, 30.0f, 0.0f);
    characterShader = std::make_unique<Shader>("../src/res/shaders/character.shader");

    terrain = std::make_unique<Terrain>();
    terrain->Init(32.0f, 33);
    terrain->SetNoiseParams(0.02f, 4.0f);
    terrain->SetHillParams(0.005f, 30.0f, 2.0f);
    terrain->SetWaterParams(-20.0f, 3.0f);
    
    // DISABLED: Water
    // water = new Water();
    // water->Init(-20.0f, 512.0f);
    
    // DISABLED: Foliage (grass + trees)
    // foliage = new FoliageManager();
    // foliage->Init(terrain);
    
    terrainShader = std::make_unique<Shader>("../src/res/shaders/terrain.shader");

    camera.setCameraPos(glm::vec3(10.0f, 5.0f, 30.0f));
    sky.Init();
    skyShader = std::make_unique<Shader>("../src/res/shaders/sky.shader");
    
    std::cout << "Scene centered at origin (0, 0, 0)" << std::endl;
    std::cout << "Terrain system initialized" << std::endl;
    std::cout << "Character at: " << character->worldPosition.x << ", " 
              << character->worldPosition.y << ", " << character->worldPosition.z << std::endl;
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
    
    sky.Update(deltaTime);
    
    // Update character physics
    character->Update(deltaTime);

    // Get terrain height at character position for collision
    float terrainHeight = terrain->GetHeightAt(character->worldPosition.x, character->worldPosition.z);
    const float characterHeightOffset = 1.25f;
    float characterFeetY = character->worldPosition.y - characterHeightOffset;

    if (characterFeetY < terrainHeight) {
        character->worldPosition.y = terrainHeight + characterHeightOffset;
        character->physics.velocity.y = 0.0f;
        character->physics.isGrounded = true;
    } else {
        if(character->physics.velocity.y <= 0.0f) 
            character->physics.isGrounded = false;
    }
    
    terrain->Update(camera.getCameraPos(), deltaTime);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::vec3 sunDirection = glm::normalize(glm::vec3(0.5f, 0.8f, -0.5f));
    // Warm sun color (golden)
    glm::vec3 sunColor = glm::vec3(1.1f, 0.95f, 0.8f); 
    // Cool ambient color (brighter blue)
    glm::vec3 ambientColor = glm::vec3(0.35f, 0.4f, 0.55f);

    glm::vec3 cameraPosition = camera.getCameraPos();

    sky.Render(*skyShader, view, projection, sunDirection);    
    
    // Fog Parameters
    glm::vec3 fogColor = glm::vec3(0.6f, 0.7f, 0.8f);
    float fogDensity = 0.007f;
    float fogStart = 20.0f;
    float fogEnd = 150.0f;
    int fogType = 1; // Exponential
    
    terrainShader->Bind();
    terrainShader->SetUniform3v("lightDirection", sunDirection);
    terrainShader->SetUniform3v("lightColor", sunColor);
    terrainShader->SetUniform3v("ambientColor", ambientColor);
    terrainShader->SetUniform3v("viewPos", cameraPosition);
    terrainShader->SetUniform3v("fogColor", fogColor);
    terrainShader->SetUniform1f("fogDensity", fogDensity);
    terrainShader->SetUniform1f("fogStart", fogStart);
    terrainShader->SetUniform1f("fogEnd", fogEnd);
    terrainShader->SetUniform1i("fogType", fogType);
    terrain->Render(*terrainShader, view, projection);
                   
    characterShader->Bind();
    characterShader->SetUniform3v("fogColor", fogColor);
    characterShader->SetUniform1f("fogDensity", fogDensity);
    characterShader->SetUniform1f("fogStart", fogStart);
    characterShader->SetUniform1f("fogEnd", fogEnd);
    character->Render(*characterShader, view, projection);
}

void Renderer::Clean() {
    character.reset();
    characterShader.reset();
    terrain.reset();
    terrainShader.reset();
    skyShader.reset();
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

        glm::vec3 moveDir(0.0f);
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            moveDir.z -= 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            moveDir.z += 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            moveDir.x -= 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            moveDir.x += 1.0f;
        }

        bool isRunning = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
        float moveSpeed = isRunning ? 10.0f : 5.0f;

        character->Move(moveDir, moveSpeed, deltaTime, isRunning);

        static bool spacePressedDebug = false;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressedDebug) {
            character->Jump(15.0f);
            spacePressedDebug = true;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            spacePressedDebug = false;
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
    else {
        // PLAYER MODE (First/Third Person)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        
        // Handle F5 to toggle camera mode
        static bool f5Pressed = false;
        if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS && !f5Pressed) {
            cameraMode = (cameraMode == CameraMode::ThirdPerson) ? 
                          CameraMode::FirstPerson : CameraMode::ThirdPerson;
            f5Pressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_RELEASE) {
            f5Pressed = false;
        }

        // Camera calculations based on mode
        if (cameraMode == CameraMode::ThirdPerson) {
            glm::vec3 targetPos = character->worldPosition;
            targetPos.y += 2.0f;
            
            glm::vec3 front = camera.getCameraFront();
            glm::vec3 offset = -front * cameraDistance;
            
            glm::vec3 desiredPos = targetPos + offset;
            glm::vec3 currentPos = camera.getCameraPos();
            glm::vec3 smoothedPos = currentPos + (desiredPos - currentPos) * deltaTime * 10.0f;
            
            camera.setCameraPos(smoothedPos);
        }
        else {
            glm::vec3 eyePos = character->worldPosition;
            eyePos.y += cameraHeight;
            camera.setCameraPos(eyePos);
        }

        // Movement Logic (Relative to Camera View)
        glm::vec3 forward = glm::normalize(glm::vec3(camera.getCameraFront().x, 0.0f, camera.getCameraFront().z));
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
 
        glm::vec3 moveDir(0.0f);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveDir += forward;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveDir -= forward;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveDir -= right;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveDir += right;
        
        bool isRunning = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
        float moveSpeed = isRunning ? 10.0f : 5.0f;
        character->Move(moveDir, moveSpeed, deltaTime, isRunning);
 
        static bool spacePressedGame = false;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressedGame) {
            character->Jump(15.0f);
            spacePressedGame = true;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            spacePressedGame = false;
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

    if (ImGui::CollapsingHeader("Character Physics", ImGuiTreeNodeFlags_DefaultOpen)) {
        glm::vec3 pos = character->worldPosition;
        ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
        
        glm::vec3 vel = character->physics.velocity;
        ImGui::Text("Velocity: (%.2f, %.2f, %.2f)", vel.x, vel.y, vel.z);
        
        ImGui::Text("Grounded: %s", character->IsGrounded() ? "YES" : "NO");
        
        AABB charAABB = character->GetAABB();
        ImGui::Text("AABB Min: (%.2f, %.2f, %.2f)", charAABB.min.x, charAABB.min.y, charAABB.min.z);
        ImGui::Text("AABB Max: (%.2f, %.2f, %.2f)", charAABB.max.x, charAABB.max.y, charAABB.max.z);
    }

    if (ImGui::CollapsingHeader("Controls")) {
        ImGui::Text("Camera:");
        ImGui::BulletText("WASD - Move");
        ImGui::BulletText("Space - Jump");
        ImGui::BulletText("Shift - Run");
        ImGui::BulletText("Mouse - Look Around");
        ImGui::Separator();
        ImGui::Text("Interface:");
        ImGui::BulletText("F1 - Toggle Debug Window");
        ImGui::BulletText("F5 - Toggle Camera Mode");
    }

    if (ImGui::CollapsingHeader("System")) {
        ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
        ImGui::Text("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    }

    ImGui::End();
}