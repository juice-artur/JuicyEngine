#include "Application.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <ostream>


namespace JuicyEngine
{
Application* Application::AppInstance = nullptr;

Application::Application()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Learn", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    
    std::cout << "Application created\n";
    AppInstance = this;
}
}