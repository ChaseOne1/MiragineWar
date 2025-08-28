#pragma once
#include "Window.hpp"

namespace app {
class Renderer final : public utility::Singleton<Renderer>
{
    friend class utility::Singleton<Renderer>;

private:
    SDL_Renderer* m_pRenderer;

private:
    Renderer()
    {
        m_pRenderer = SDL_CreateRenderer(Window::GetInstance().GetWindow(), nullptr);

        if (!SDL_SetRenderVSync(m_pRenderer, SDL_RENDERER_VSYNC_ADAPTIVE))
            SDL_SetRenderVSync(m_pRenderer, 1);

        if (Settings::GetInstance().GetSettings().at_path("render.use_pixelart_scale").value_or(false)
            && !SDL_SetDefaultTextureScaleMode(m_pRenderer, SDL_SCALEMODE_PIXELART)) {
            SDL_Log("Set texture scale mode failed");
        }
    }

    ~Renderer() { SDL_DestroyRenderer(m_pRenderer); }

public:
    static SDL_Renderer* GetSDLRenderer() noexcept { return GetInstance().m_pRenderer; }

    static mathfu::vec2i GetRenderSize() noexcept
    {
        mathfu::vec2i size = mathfu::kZeros2i;
        SDL_GetCurrentRenderOutputSize(GetInstance().m_pRenderer, &size.x, &size.y);
        return size;
    }
};
}
