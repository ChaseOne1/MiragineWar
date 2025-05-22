#pragma once

namespace app::comp
{
    struct Clickable
    {
        SDL_Rect m_MBR;
        std::function<void()> m_fnCallback;
    };
}
