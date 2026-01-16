#pragma once
#include "AppMetadata.hpp"
#include "Settings.hpp"

namespace app {
class Window final : public utility::Singleton<Window>
{
    friend class utility::Singleton<Window>;

private:
    SDL_Window* m_pWindow;

    inline static constexpr SDL_Point msc_DefaultWindowSize { 1920, 1080 };

private:
    Window()
    {
        m_pWindow = SDL_CreateWindow(AppMetaData::NAME,
            msc_DefaultWindowSize.x,
            msc_DefaultWindowSize.y,
            SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_RESIZABLE);
    }

    ~Window() { SDL_DestroyWindow(m_pWindow); }

public:
    static SDL_Window* GetWindow() { return GetInstance().m_pWindow; }

    static void ResizeWindow()
    {
        SDL_SetWindowSize(GetInstance().GetWindow(),
            Settings::GetSettings()["app"]["window_width"], Settings::GetSettings()["app"]["window_height"]);
        SDL_SyncWindow(GetInstance().GetWindow());
    }
};
}
