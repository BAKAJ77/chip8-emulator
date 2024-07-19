#include <core/window.h>
#include <stdexcept>

WindowFrame::WindowFrame(std::string_view title, int width, int height, int xPos, int yPos) :
    m_title(title), m_xPos(xPos), m_yPos(yPos), m_width(width), m_height(height)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) // Init the SDL video subsystem
        throw std::runtime_error("Failed to initialize SDL3 (Error: " + std::string(SDL_GetError()) + ")");

    // Create and setup the window frame
    m_frame = SDL_CreateWindow(title.data(), width, height, NULL);
    if (!m_frame)
        throw std::runtime_error("Failed to create SDL window (Error: " + std::string(SDL_GetError()) + ")");

    SDL_SetWindowPosition(m_frame, xPos, yPos);
}

WindowFrame::~WindowFrame() 
{ 
    SDL_DestroyWindow(m_frame); 
    SDL_Quit();
}

void WindowFrame::SetTitle(std::string_view title) 
{
    if (SDL_SetWindowTitle(m_frame, title.data()) < 0)
        throw std::runtime_error("Failed to change SDL window title (Error: " + std::string(SDL_GetError()) + ")");

    m_title = title; 
}

void WindowFrame::SetPosition(int xPos, int yPos)
{
    if (SDL_SetWindowPosition(m_frame, xPos, yPos) < 0)
        throw std::runtime_error("Failed to change SDL window position (Error: " + std::string(SDL_GetError()) + ")");

    m_xPos = xPos;
    m_yPos = yPos; 
}

void WindowFrame::SetSize(int width, int height) 
{ 
    if (SDL_SetWindowSize(m_frame, width, height) < 0)
        throw std::runtime_error("Failed to change SDL window size (Error: " + std::string(SDL_GetError()) + ")");

    m_width = width;
    m_height = height; 
}

bool WindowFrame::PollEvents(SDL_Event& event) const { return SDL_PollEvent(&event); }

const std::string& WindowFrame::GetTitle() const { return m_title; }

const int& WindowFrame::GetPositionX() const { return m_xPos; }

const int& WindowFrame::GetPositionY() const { return m_yPos; }

const int& WindowFrame::GetWidth() const { return m_width; }

const int& WindowFrame::GetHeight() const { return m_height; }
