#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/InputManager.h"
#include "engine/Camera.h"
#include "engine/DebugRenderer.h"
#include "player/Player.h"
#include "player/PlayerController.h"
#include <iostream>
#include <cstdlib>

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;
constexpr const char* WINDOW_TITLE = "Dungeon Depths 3D";

void framebufferSizeCallback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Error: No se pudo inicializar GLFW\n";
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Error: No se pudo crear la ventana GLFW\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSwapInterval(1);

    int version = gladLoadGL(glfwGetProcAddress);
    if (!version)
    {
        std::cerr << "Error: No se pudo inicializar GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    std::cout << "OpenGL " << GLAD_VERSION_MAJOR(version) << "."
              << GLAD_VERSION_MINOR(version) << " inicializado\n";

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);

    InputManager::init(window);

    DebugRenderer debugRenderer;
    if (!debugRenderer.init())
    {
        std::cerr << "Error: No se pudo inicializar DebugRenderer\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    Camera camera;
    Player player;
    PlayerController playerController(player, camera);

    float aspectRatio = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);
    glm::mat4 projection = glm::perspective(glm::radians(70.0f), aspectRatio, 0.1f, 100.0f);

    float lastFrameTime = static_cast<float>(glfwGetTime());
    float deltaTime = 0.0f;

    std::cout << "=== Dungeon Depths 3D ===\n";
    std::cout << "Controles:\n";
    std::cout << "  WASD      - Moverse\n";
    std::cout << "  Mouse     - Mirar\n";
    std::cout << "  Click izq - Espada (melee)\n";
    std::cout << "  Click der - Ballesta (ranged)\n";
    std::cout << "  V         - Cambiar 1ra/3ra persona\n";
    std::cout << "  ESC       - Salir\n";

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        InputManager::update();

        float currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        if (InputManager::isKeyPressed(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);

        if (InputManager::isKeyPressed(GLFW_KEY_V))
            camera.toggleMode();

        float mouseDX, mouseDY;
        InputManager::getMouseDelta(mouseDX, mouseDY);
        playerController.handleMouseMovement(mouseDX, mouseDY);

        playerController.handleInput(deltaTime);
        player.update(deltaTime);

        glm::mat4 view = camera.getViewMatrix(player.transform.position);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        debugRenderer.setViewProjection(view, projection);

        if (camera.getMode() == CameraMode::THIRD_PERSON)
        {
            glm::vec3 playerColor(0.2f, 0.45f, 0.85f);

            if (player.isAttacking() && player.getCurrentAttack() == AttackType::MELEE)
                playerColor = glm::vec3(0.9f, 0.3f, 0.1f);

            debugRenderer.drawCube(player.transform.getModelMatrix(), playerColor);
        }

        if (player.isAttacking() && player.getCurrentAttack() == AttackType::MELEE)
        {
            AABB hitbox = player.getMeleeHitbox();
            glm::vec3 center = hitbox.getCenter();
            glm::vec3 size = (hitbox.max - hitbox.min);
            debugRenderer.drawCube(center, size, glm::vec3(1.0f, 0.6f, 0.1f));
        }



        if (camera.getMode() == CameraMode::FIRST_PERSON)
        {
            debugRenderer.drawCrosshair(
                static_cast<float>(WINDOW_WIDTH),
                static_cast<float>(WINDOW_HEIGHT),
                20.0f,
                glm::vec3(1.0f, 1.0f, 1.0f)
            );
        }

        glfwSwapBuffers(window);
    }

    debugRenderer.cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
