#ifndef WINDOW_H
#define WINDOW_H

#include <core/renderer.h>
#include <string>

class WindowFrame
{
public:
    /**
     * @brief Initializes GLFW and GLAD, then creates a window along with a OpenGL context.
     * @param[in] title The title to be assigned to the window.
     * @param[in] resolution The desired resolution of the window.
     * @param[in] position The desired position of the window.
     */
    WindowFrame(std::string_view title, Vector2<int> resolution = { 640, 320 }, 
        Vector2<int> position = { SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED });

    ~WindowFrame();

    /**
     * @brief Sets the title of the window.
     * @param[in] title The new title to be assigned to the window.
     */
    void SetTitle(std::string_view title);

    /**
     * @brief Sets the position of the window.
     * @param[in] position The new desired position of the window.
     */
    void SetPosition(Vector2<int> position);

    /**
     * @brief Sets the resolution of the window.
     * @param[in] resolution The new desired resolution of the window.
     */
    void SetResolution(Vector2<int> resolution);

    /**
     * @brief Fetches the next pending event in the event queue.
     * @param[out] event The next event fetched from the event queue.
     * @return True if an event was pending in the queue, or else False is returned.
     */
    bool PollEvents(SDL_Event& event) const;

    /**
     * @brief Gets the window's graphics rendering context.
     * @return A reference to the graphics renderer attached to the window.
     */
    GraphicsRenderer& GetRenderer();

    /**
     * @brief Gets the title of the window.
     * @return A string representing the title of the window.
     */
    const std::string& GetTitle() const;

    /**
     * @brief Gets the position of the window.
     * @return A 2-dimensional vector representing the position of the window.
     */
    const Vector2<int>& GetPosition() const;

    /**
     * @brief Gets the resolution of the window.
     * @return A 2-dimensional vector representing the resolution of the window.
     */
    const Vector2<int>& GetResolution() const;
private:
    SDL_Window* m_frame;
    std::string m_title;
    Vector2<int> m_position, m_resolution;
    
    GraphicsRenderer m_renderer;
};

#endif