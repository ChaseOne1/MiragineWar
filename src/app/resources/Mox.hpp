#pragma once

struct SDL_Texture;

namespace app {
struct Mox
{
    struct Header
    {
        uint16_t width = 0.f, height = 0.f;
        float left_width = 0.f, right_width = 0.f;
        float top_height = 0.f, bottom_height = 0.f;
    } m_Header;

    std::shared_ptr<SDL_Texture> m_Texture;
};
}
