#include <core/renderer.h>
#include <stdexcept>
#include "renderer.h"

GraphicsRenderer::GraphicsRenderer() :
    m_renderingContext(nullptr)
{}

GraphicsRenderer::GraphicsRenderer(SDL_Window *frame) : m_clearColor({ 0, 0, 0 })
{
    m_renderingContext = SDL_CreateRenderer(frame, nullptr);
    if (!m_renderingContext)
        throw std::runtime_error("Failed to create SDL rendering context (Error: " + std::string(SDL_GetError()) + ")");
}

void GraphicsRenderer::Destroy() { SDL_DestroyRenderer(m_renderingContext); }

void GraphicsRenderer::SetClearColor(Vector3<uint8_t> color) { m_clearColor = color; }

void GraphicsRenderer::Clear()
{
    SDL_SetRenderDrawColor(m_renderingContext, m_clearColor.r, m_clearColor.g, m_clearColor.b, 0xFF);
    SDL_RenderClear(m_renderingContext);
}

void GraphicsRenderer::Update() { SDL_RenderPresent(m_renderingContext); }

void GraphicsRenderer::DrawRect(Vector2<int> position, Vector2<int> size,  Vector3<uint8_t> color)
{
    SDL_FRect spriteRect = { (float)position.x, (float)position.y, (float)size.x, (float)size.y };

    SDL_SetRenderDrawColor(m_renderingContext, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(m_renderingContext, &spriteRect);
}

const Vector3<uint8_t> &GraphicsRenderer::GetClearColor() const { return m_clearColor; }
