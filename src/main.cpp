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

void scrollCallback(GLFWwindow*, double, double yoffset) {
    if (!g_game) return;
    const float zoomFactor = yoffset > 0.0 ? 1.1f : 0.9f;
    g_game->mapCamera().zoomBy(zoomFactor);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glClearColor(0.05f, 0.05f, 0.12f, 1.0f);

    InputManager::init(window);

    {
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

        std::cout << "=== Dungeon Depths 3D ===" << std::endl;
        std::cout << "  WASD      - Moverse" << std::endl;
        std::cout << "  Mouse     - Mirar" << std::endl;
        std::cout << "  Click izq - Espada" << std::endl;
        std::cout << "  Click der - Ballesta" << std::endl;
        std::cout << "  V         - 1ra/3ra persona" << std::endl;
        std::cout << "  ESC       - Salir" << std::endl;

        game::Game game;
        g_game = &game;

        if (!game.init(WINDOW_WIDTH, WINDOW_HEIGHT, TILE_VERT_SHADER, TILE_FRAG_SHADER)) {
            g_game = nullptr;
            debugRenderer.cleanup();
            glfwDestroyWindow(window);
            glfwTerminate();
            return EXIT_FAILURE;
        }

        glm::vec3 spawnPos = game.getPlayerSpawnPosition();
        player.resetForNewLevel(spawnPos);

        float lastFrame = static_cast<float>(glfwGetTime());

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            InputManager::update();

            const float currentFrame = static_cast<float>(glfwGetTime());
            const float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window);

            if (InputManager::isKeyPressed(GLFW_KEY_V))
                camera.toggleMode();

            float mouseDX, mouseDY;
            InputManager::getMouseDelta(mouseDX, mouseDY);
            playerController.handleMouseMovement(mouseDX, mouseDY);

            game.update(deltaTime, window);

            playerController.handleInput(deltaTime);
            player.update(deltaTime);

            glm::vec3 corrected = game.resolveWallCollision(
                player.transform.position, Player::HALF_SIZE);
            player.transform.position = corrected;

            int framebufferWidth = 0;
            int framebufferHeight = 0;
            glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
            float aspectRatio = static_cast<float>(framebufferWidth) /
                                static_cast<float>(std::max(framebufferHeight, 1));

            glm::mat4 view = camera.getViewMatrix(player.transform.position);
            glm::mat4 projection = glm::perspective(
                glm::radians(70.0f), aspectRatio, 0.1f, 200.0f);
            glm::mat4 viewProjection = projection * view;

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            game.renderWithViewProjection(viewProjection);

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
                    static_cast<float>(framebufferWidth),
                    static_cast<float>(framebufferHeight),
                    20.0f,
                    glm::vec3(1.0f, 1.0f, 1.0f)
                );
            }

            glfwSwapBuffers(window);
        }

        g_game = nullptr;
        game.shutdown();
        debugRenderer.cleanup();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
