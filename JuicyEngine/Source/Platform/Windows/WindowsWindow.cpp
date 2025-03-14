#include "Window.h"

#include <iostream>
#include <GLFW/glfw3.h>


namespace JuicyEngine
{
Window::Window()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    WindowPtr = glfwCreateWindow(800, 600, "Learn", nullptr, nullptr);
    if (WindowPtr == nullptr)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
    }

    glfwMakeContextCurrent(static_cast<GLFWwindow*>(WindowPtr));
}

Window::~Window()
{
    glfwTerminate();
}

void* Window::GetNativeWindow() const
{
    return WindowPtr;
}
}