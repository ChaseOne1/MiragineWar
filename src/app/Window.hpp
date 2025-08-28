#pragma once
#include "AppMetadata.hpp"
#include "Settings.hpp"

namespace app {
class Window final : public utility::Singleton<Window>
{
    friend class utility::Singleton<Window>;

private:
    SDL_Window* m_pWindow;

private:
    Window()
    {
        m_pWindow = SDL_CreateWindow(AppMetaData::NAME,
            Settings::GetInstance().GetSettings().at_path("app.window_width").value_or(1024),
            Settings::GetInstance().GetSettings().at_path("app.window_height").value_or(768),
            SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_RESIZABLE);
    }

    ~Window() { SDL_DestroyWindow(m_pWindow); }

public:
    static SDL_Window* GetWindow() { return GetInstance().m_pWindow; }
};
}
