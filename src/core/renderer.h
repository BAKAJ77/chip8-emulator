#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>
#include <vector.h>

class GraphicsRenderer
{
public:
    /**
     * @brief The default constructor of the class, does nothing but initialize member variables.
     */
    GraphicsRenderer();

    /**
     * @brief Creates a rendering context for the window frame provided.
     * @param[in] frame The window frame to create a rendering context for.
     */
    GraphicsRenderer(SDL_Window* frame);
    
    ~GraphicsRenderer() = default;

    /**
     * @brief Destroys the graphics rendering context.
     */
    void Destroy();
    
    /**
     * @brief Sets the color which will be used when clearing the back render buffer.
     * @param[in] color The color to be used when clearing the back render buffer.
     */
    void SetClearColor(Vector3<uint8_t> color);

    /**
     * @brief Clears the back render buffer.
     */
    void Clear();

    /**
     * @brief Swaps the front and back render buffers.
     */
    void Update();

    /**
     * @brief Draws a rectangle sprite, of specified size and position, onto the back render buffer.
     * @param[in] position The position of the rectangle sprite.
     * @param[in] heightPixels The height of the rectangle sprite in pixels (the width is fixed at 8 pixels).
     * @param[in] color The color of the rectangle sprite.
     */
    void DrawSprite(Vector2<int> position, int heightPixels, Vector3<uint8_t> color = { 255, 255, 255 });

    /**
     * @brief Gets the current assigned color to be used when clearing the back render buffer.
     * @return A 3-component vector representing the clearing color.
     */
    const Vector3<uint8_t>& GetClearColor() const;
private:
    SDL_Renderer* m_renderingContext;
    Vector3<uint8_t> m_clearColor;
};

#endif