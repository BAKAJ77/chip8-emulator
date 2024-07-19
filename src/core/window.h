#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL.h>
#include <string>

class WindowFrame
{
public:
    /**
     * @brief Initializes GLFW and GLAD, then creates a window along with a OpenGL context.
     * @param[in] title The title to be assigned to the window.
     * @param[in] width The desired width of the window.
     * @param[in] height The desired height of the window.
     * @param[in] xPos The desired x-axis position of the window.
     * @param[in] yPos The desired y-axis position of the window.
     */
    WindowFrame(std::string_view title, int width = 640, int height = 320, int xPos = SDL_WINDOWPOS_CENTERED, 
        int yPos = SDL_WINDOWPOS_CENTERED);

    ~WindowFrame();

    /**
     * @brief Sets the title of the window.
     * @param[in] title The new title to be assigned to the window.
     */
    void SetTitle(std::string_view title);

    /**
     * @brief Sets the position of the window.
     * @param[in] xPos The new desired x-axis position of the window.
     * @param[in] yPos The new desired y-axis position of the window.
     */
    void SetPosition(int xPos, int yPos);

    /**
     * @brief Sets the size of the window.
     * @param[in] width The new desired width of the window.
     * @param[in] height The new desired height of the window.
     */
    void SetSize(int width, int height);

    /**
     * @brief Fetches the next pending event in the event queue.
     * @param[out] event The next event fetched from the event queue.
     * @return True if an event was pending in the queue, or else False is returned.
     */
    bool PollEvents(SDL_Event& event) const;

    /**
     * @brief Gets the title of the window.
     * @return A string representing the title of the window.
     */
    const std::string& GetTitle() const;

    /**
     * @brief Gets the x-axis position of the window.
     * @return An integer representing the x-axis position of the window.
     */
    const int& GetPositionX() const;

    /**
     * @brief Gets the y-axis position of the window.
     * @return An integer representing the y-axis position of the window.
     */
    const int& GetPositionY() const;

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
    SDL_Window* m_frame;
    std::string m_title;
    int m_xPos, m_yPos, m_width, m_height;
};

#endif