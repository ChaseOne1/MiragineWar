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

#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off

srefl_class(app::Mox::Header,
    fields(
        field(&app::Mox::Header::width),
        field(&app::Mox::Header::height),
        field(&app::Mox::Header::left_width),
        field(&app::Mox::Header::right_width),
        field(&app::Mox::Header::top_height),
        field(&app::Mox::Header::bottom_height)
    )
)

srefl_class(app::Mox,
    fields(
        field(&app::Mox::m_Header),
        field(&app::Mox::m_Texture)
    )
)

// clang-format on
#include "mirrow/srefl/srefl_end.hpp"
