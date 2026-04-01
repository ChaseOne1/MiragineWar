#pragma once

struct SDL_Texture;

namespace app {
struct MoxHeader {
    uint16_t width = 0.f, height = 0.f;
    float left_width = 0.f, right_width = 0.f;
    float top_height = 0.f, bottom_height = 0.f;
};

struct Mox
{
    MoxHeader m_Header;

    std::shared_ptr<SDL_Texture> m_Texture;
};
}

#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off

srefl_class(app::MoxHeader,
    fields(
        field(&app::MoxHeader::width),
        field(&app::MoxHeader::height),
        field(&app::MoxHeader::left_width),
        field(&app::MoxHeader::right_width),
        field(&app::MoxHeader::top_height),
        field(&app::MoxHeader::bottom_height)
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
