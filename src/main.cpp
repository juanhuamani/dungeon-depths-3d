#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "game/Game.h"
#include "world/DungeonConsoleTest.h"

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
    g_game->camera().zoomBy(zoomFactor);
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

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.05f, 0.05f, 0.12f, 1.0f);

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
        const float currentFrame = static_cast<float>(glfwGetTime());
        const float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        game.update(deltaTime, window);

        glClear(GL_COLOR_BUFFER_BIT);

        int framebufferWidth = 0;
        int framebufferHeight = 0;
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        game.render(framebufferWidth, framebufferHeight);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    g_game = nullptr;
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
