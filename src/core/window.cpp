#include <core/window.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <stdexcept>

Window::Window(std::string_view title, int width, int height) :
    m_title(title), m_width(width), m_height(height)
{
    if (!glfwInit()) // Initialize GLFW
        throw std::runtime_error("Failed to initialize GLFW");

    // Configure window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

    // Create and setup the window
    m_window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
    if (!m_window)
        throw std::runtime_error("Failed to create GLFW window");

    const GLFWvidmode* monitorData = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if (!monitorData)
        throw std::runtime_error("Failed to fetch monitor video mode data");

    glfwSetWindowPos(m_window, (monitorData->width / 2) - (width / 2), (monitorData->height / 2) - (height / 2));
    glfwMakeContextCurrent(m_window);

    // Load the addresses of each OpenGL function
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to load the addresses of each OpenGL function");
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::SetTitle(std::string_view title) 
{
    glfwSetWindowTitle(m_window, title.data()); 
    m_title = title; 
}

void Window::SetSize(int width, int height) 
{ 
    glfwSetWindowSize(m_window, width, height);
    m_width = width;
    m_height = height; 
}

void Window::PollEvents() const { glfwPollEvents(); }

void Window::SwapRenderBuffers() const { glfwSwapBuffers(m_window); }

bool Window::WasRequestedToClose() const { return glfwWindowShouldClose(m_window); }

const std::string &Window::GetTitle() const { return m_title; }

const int &Window::GetWidth() const { return m_width; }

const int &Window::GetHeight() const { return m_height; }
