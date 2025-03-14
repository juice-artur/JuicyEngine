#include "Application.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <ostream>

#include "Window.h"

namespace JuicyEngine
{
Application* Application::AppInstance = nullptr;

Application::Application()
{
    Window GameWindow;
    while (!glfwWindowShouldClose(static_cast<GLFWwindow*>(GameWindow.GetNativeWindow())))
    {
        if(glfwGetKey(static_cast<GLFWwindow*>(GameWindow.GetNativeWindow()), GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(static_cast<GLFWwindow*>(GameWindow.GetNativeWindow()), true);
        glfwSwapBuffers(static_cast<GLFWwindow*>(GameWindow.GetNativeWindow()));
        glfwPollEvents();
    }
    
    
    std::cout << "Application created\n";
    AppInstance = this;
}
}