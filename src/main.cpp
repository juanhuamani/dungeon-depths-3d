#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game/Game.h"
#include "world/DungeonConsoleTest.h"
#include "engine/InputManager.h"
#include "engine/Camera.h"
#include "engine/DebugRenderer.h"
#include "player/Player.h"
#include "player/PlayerController.h"

#include <iostream>
#include <cstdlib>
#include <string>

namespace {

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;
constexpr const char* WINDOW_TITLE = "Dungeon Depths 3D";
constexpr const char* TILE_VERT_SHADER = "assets/shaders/tile.vert";
constexpr const char* TILE_FRAG_SHADER = "assets/shaders/tile.frag";

game::Game* g_game = nullptr;

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
    if (g_game) {
        g_game->onFramebufferResize(width, height);
    }
}

void scrollCallback(GLFWwindow*, double /*xoffset*/, double yoffset) {
    if (!g_game) {
        return;
    }
    const float zoomFactor = yoffset > 0.0 ? 1.1f : 0.9f;
    g_game->mapCamera().zoomBy(zoomFactor);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--test-dungeon") {
        world::runDungeonConsoleTest();
        return EXIT_SUCCESS;
    }

    if (!glfwInit()) {
        std::cerr << "Error: No se pudo inicializar GLFW\n";
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (!window) {
        std::cerr << "Error: No se pudo crear la ventana GLFW\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSwapInterval(1);

    const int version = gladLoadGL(glfwGetProcAddress);
    if (!version) {
        std::cerr << "Error: No se pudo inicializar GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    std::cout << "OpenGL " << GLAD_VERSION_MAJOR(version) << "."
              << GLAD_VERSION_MINOR(version) << " inicializado\n";
    std::cout << "Controles: WASD/flechas=mover  scroll=zoom  Z=vista 2D  X=vista 3D  ESC=salir\n";

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.05f, 0.05f, 0.12f, 1.0f);

    // Initialize input, debug renderer, camera and player systems
    InputManager::init(window);

    DebugRenderer debugRenderer;
    if (!debugRenderer.init()) {
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

    std::cout << "=== Dungeon Depths 3D ===\n";
    std::cout << "Controles:\n";
    std::cout << "  WASD      - Moverse\n";
    std::cout << "  Mouse     - Mirar\n";
    std::cout << "  Click izq - Espada (melee)\n";
    std::cout << "  Click der - Ballesta (ranged)\n";
    std::cout << "  V         - Cambiar 1ra/3ra persona\n";
    std::cout << "  ESC       - Salir\n";

    // Initialize the game (dungeon generation + tile rendering)
    {
        game::Game game;
        g_game = &game;

        if (!game.init(WINDOW_WIDTH, WINDOW_HEIGHT, TILE_VERT_SHADER, TILE_FRAG_SHADER)) {
            g_game = nullptr;
            glfwDestroyWindow(window);
            glfwTerminate();
            return EXIT_FAILURE;
        }

        float lastFrame = static_cast<float>(glfwGetTime());

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            InputManager::update();

            const float currentFrame = static_cast<float>(glfwGetTime());
            const float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // Process input
            processInput(window);

            if (InputManager::isKeyPressed(GLFW_KEY_V))
                camera.toggleMode();

            float mouseDX, mouseDY;
            InputManager::getMouseDelta(mouseDX, mouseDY);
            playerController.handleMouseMovement(mouseDX, mouseDY);

            // Update game and player
            game.update(deltaTime, window);
            playerController.handleInput(deltaTime);
            player.update(deltaTime);

            glm::mat4 view = camera.getViewMatrix(player.transform.position);

            // Render
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            int framebufferWidth = 0;
            int framebufferHeight = 0;
            glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
            game.render(framebufferWidth, framebufferHeight);

            // Debug rendering (player cube, melee hitbox, crosshair)
            debugRenderer.setViewProjection(view, projection);

            if (camera.getMode() == CameraMode::THIRD_PERSON) {
                glm::vec3 playerColor(0.2f, 0.45f, 0.85f);

                if (player.isAttacking() && player.getCurrentAttack() == AttackType::MELEE)
                    playerColor = glm::vec3(0.9f, 0.3f, 0.1f);

                debugRenderer.drawCube(player.transform.getModelMatrix(), playerColor);
            }

            if (player.isAttacking() && player.getCurrentAttack() == AttackType::MELEE) {
                AABB hitbox = player.getMeleeHitbox();
                glm::vec3 center = hitbox.getCenter();
                glm::vec3 size = (hitbox.max - hitbox.min);
                debugRenderer.drawCube(center, size, glm::vec3(1.0f, 0.6f, 0.1f));
            }

            if (camera.getMode() == CameraMode::FIRST_PERSON) {
                debugRenderer.drawCrosshair(
                    static_cast<float>(WINDOW_WIDTH),
                    static_cast<float>(WINDOW_HEIGHT),
                    20.0f,
                    glm::vec3(1.0f, 1.0f, 1.0f)
                );
            }

            glfwSwapBuffers(window);
        }

        g_game = nullptr;
        game.shutdown();
    }

    debugRenderer.cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
