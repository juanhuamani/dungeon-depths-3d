#pragma once

#include <GLFW/glfw3.h>

class InputManager {
public:
    static void init(GLFWwindow* window);
    static void update();

    static bool isKeyDown(int key);
    static bool isKeyPressed(int key);
    static bool isKeyReleased(int key);

    static bool isMouseButtonDown(int button);
    static bool isMouseButtonPressed(int button);

    static void getMouseDelta(float& dx, float& dy);

private:
    static GLFWwindow* s_window;

    static bool s_currentKeys[GLFW_KEY_LAST + 1];
    static bool s_previousKeys[GLFW_KEY_LAST + 1];

    static bool s_currentMouse[GLFW_MOUSE_BUTTON_LAST + 1];
    static bool s_previousMouse[GLFW_MOUSE_BUTTON_LAST + 1];

    static float  s_mouseDeltaX;
    static float  s_mouseDeltaY;
    static double s_lastCursorX;
    static double s_lastCursorY;
    static bool   s_firstFrame;
};
