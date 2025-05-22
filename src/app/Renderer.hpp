#pragma once

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
    }

    ~Renderer() { SDL_DestroyRenderer(m_pRenderer); }

public:
    SDL_Renderer* GetRenderer() { return m_pRenderer; }
};
}
