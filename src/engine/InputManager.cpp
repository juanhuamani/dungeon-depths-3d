#include "engine/InputManager.h"

#include <cstring>

GLFWwindow* InputManager::s_window       = nullptr;
bool InputManager::s_currentKeys[GLFW_KEY_LAST + 1]           = {};
bool InputManager::s_previousKeys[GLFW_KEY_LAST + 1]          = {};
bool InputManager::s_currentMouse[GLFW_MOUSE_BUTTON_LAST + 1] = {};
bool InputManager::s_previousMouse[GLFW_MOUSE_BUTTON_LAST + 1]= {};

float  InputManager::s_mouseDeltaX  = 0.0f;
float  InputManager::s_mouseDeltaY  = 0.0f;
double InputManager::s_lastCursorX  = 0.0;
double InputManager::s_lastCursorY  = 0.0;
bool   InputManager::s_firstFrame   = true;

void InputManager::init(GLFWwindow* window)
{
    s_window = window;
    std::memset(s_currentKeys,   0, sizeof(s_currentKeys));
    std::memset(s_previousKeys,  0, sizeof(s_previousKeys));
    std::memset(s_currentMouse,  0, sizeof(s_currentMouse));
    std::memset(s_previousMouse, 0, sizeof(s_previousMouse));

    s_mouseDeltaX = 0.0f;
    s_mouseDeltaY = 0.0f;
    s_firstFrame  = true;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glfwGetCursorPos(window, &s_lastCursorX, &s_lastCursorY);
}

void InputManager::update()
{
    std::memcpy(s_previousKeys,  s_currentKeys,  sizeof(s_currentKeys));
    std::memcpy(s_previousMouse, s_currentMouse, sizeof(s_currentMouse));

    for (int key = 0; key <= GLFW_KEY_LAST; ++key)
        s_currentKeys[key] = (glfwGetKey(s_window, key) == GLFW_PRESS);

    for (int btn = 0; btn <= GLFW_MOUSE_BUTTON_LAST; ++btn)
        s_currentMouse[btn] = (glfwGetMouseButton(s_window, btn) == GLFW_PRESS);

    double curX, curY;
    glfwGetCursorPos(s_window, &curX, &curY);

    if (s_firstFrame)
    {
        s_lastCursorX = curX;
        s_lastCursorY = curY;
        s_firstFrame  = false;
        s_mouseDeltaX = 0.0f;
        s_mouseDeltaY = 0.0f;
    }
    else
    {
        s_mouseDeltaX = static_cast<float>(curX - s_lastCursorX);
        s_mouseDeltaY = static_cast<float>(s_lastCursorY - curY);
        s_lastCursorX = curX;
        s_lastCursorY = curY;
    }
}

bool InputManager::isKeyDown(int key)
{
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return s_currentKeys[key];
}

bool InputManager::isKeyPressed(int key)
{
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return s_currentKeys[key] && !s_previousKeys[key];
}

bool InputManager::isKeyReleased(int key)
{
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return !s_currentKeys[key] && s_previousKeys[key];
}

bool InputManager::isMouseButtonDown(int button)
{
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
    return s_currentMouse[button];
}

bool InputManager::isMouseButtonPressed(int button)
{
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
    return s_currentMouse[button] && !s_previousMouse[button];
}

void InputManager::getMouseDelta(float& dx, float& dy)
{
    dx = s_mouseDeltaX;
    dy = s_mouseDeltaY;
}
