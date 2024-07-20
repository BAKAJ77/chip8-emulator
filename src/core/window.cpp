#include <core/window.h>
#include <stdexcept>
#include "window.h"

WindowFrame::WindowFrame(std::string_view title, Vector2<int> resolution, Vector2<int> position) :
    m_title(title), m_position(position), m_resolution(resolution)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) // Init the SDL video subsystem
        throw std::runtime_error("Failed to initialize SDL3 (Error: " + std::string(SDL_GetError()) + ")");

    // Create and setup the window frame
    m_frame = SDL_CreateWindow(title.data(), resolution.x, resolution.y, NULL);
    if (!m_frame)
        throw std::runtime_error("Failed to create SDL window (Error: " + std::string(SDL_GetError()) + ")");

    SDL_SetWindowPosition(m_frame, position.x, position.y);
    m_renderer = GraphicsRenderer(m_frame);
}

WindowFrame::~WindowFrame() 
{ 
    m_renderer.Destroy();

    SDL_DestroyWindow(m_frame); 
    SDL_Quit();
}

void WindowFrame::SetTitle(std::string_view title) 
{
    if (SDL_SetWindowTitle(m_frame, title.data()) < 0)
        throw std::runtime_error("Failed to change SDL window title (Error: " + std::string(SDL_GetError()) + ")");

    m_title = title; 
}

void WindowFrame::SetPosition(Vector2<int> position)
{
    if (SDL_SetWindowPosition(m_frame, position.x, position.y) < 0)
        throw std::runtime_error("Failed to change SDL window position (Error: " + std::string(SDL_GetError()) + ")");

    m_position = position; 
}

void WindowFrame::SetResolution(Vector2<int> resolution) 
{ 
    if (SDL_SetWindowSize(m_frame, resolution.x, resolution.y) < 0)
        throw std::runtime_error("Failed to change SDL window size (Error: " + std::string(SDL_GetError()) + ")");

    m_resolution = resolution; 
}

bool WindowFrame::PollEvents(SDL_Event& event) const { return SDL_PollEvent(&event); }

GraphicsRenderer& WindowFrame::GetRenderer() { return m_renderer; }

const std::string& WindowFrame::GetTitle() const { return m_title; }

const Vector2<int>& WindowFrame::GetPosition() const { return m_position; }

const Vector2<int>& WindowFrame::GetResolution() const { return m_resolution; }