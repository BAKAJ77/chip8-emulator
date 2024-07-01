#ifndef WINDOW_H
#define WINDOW_H

#include <string>

struct GLFWwindow;

class Window
{
public:
    /**
     * @brief Initializes GLFW and GLAD, then creates a window along with a OpenGL context.
     * @param title The title to be assigned to the window.
     * @param width The desired width of the window.
     * @param height The desired height of the window.
     */
    Window(std::string_view title, int width = 640, int height = 320);

    ~Window();

    /**
     * @brief Sets the title of the window.
     * @param title The new title to be assigned to the window.
     */
    void SetTitle(std::string_view title);

    /**
     * @brief Sets the size of the window.
     * @param width The new desired width of the window.
     * @param height The new desired height of the window.
     */
    void SetSize(int width, int height);

    /**
     * @brief Fetches and processes pending window events.
     */
    void PollEvents() const;

    /**
     * @brief Swaps the window's front and back rendering buffers.
     */
    void SwapRenderBuffers() const;

    /**
     * @brief Checks whether or not the window has been requested to close.
     * @return `true` if the window has been requested to close, `false` otherwise.
     */
    bool WasRequestedToClose() const;

    /**
     * @brief Gets the title of the window.
     * @return A string representing the title of the window.
     */
    const std::string& GetTitle() const;

    /**
     * @brief Gets the width of the window.
     * @return An integer representing the width of the window.
     */
    const int& GetWidth() const;

    /**
     * @brief Gets the height of the window.
     * @return An integer representing the height of the window.
     */
    const int& GetHeight() const;
private:
    GLFWwindow* m_window;
    std::string m_title;
    int m_width, m_height;
};

#endif