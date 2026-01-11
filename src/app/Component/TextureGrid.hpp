#pragma once
#include "app/resources/Mox.hpp"
#include "app/ScriptComponent.hpp"

namespace app::comp {
struct TextureGrid : app::ScriptComponent<TextureGrid>
{
    float m_fLeftWidth = 0.f, m_fRightWidth = 0.f;
    float m_fTopHeight = 0.f, m_fBottomHeight = 0.f;
    // TODO: if the tile_scale is equal to 0.f, then the RenderTexture9Grid is used, or Tiled
    float m_fScale = 1.f;

public:
    TextureGrid(float left_width, float right_width, float top_height, float bottom_height)
        : m_fLeftWidth(left_width)
        , m_fRightWidth(right_width)
        , m_fTopHeight(top_height)
        , m_fBottomHeight(bottom_height)
    { }

    TextureGrid(const app::Mox::Header& header, float scale = 1.f)
        : m_fLeftWidth(header.left_width)
        , m_fRightWidth(header.right_width)
        , m_fTopHeight(header.top_height)
        , m_fBottomHeight(header.bottom_height)
        , m_fScale(scale)
    { }
};
}

#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off
srefl_class(app::comp::TextureGrid,
    ctors(
        ctor(const app::Mox::Header&, float)
        )
    fields(
        field(&app::comp::TextureGrid::m_fLeftWidth),
        field(&app::comp::TextureGrid::m_fRightWidth),
        field(&app::comp::TextureGrid::m_fTopHeight),
        field(&app::comp::TextureGrid::m_fBottomHeight),
        field(&app::comp::TextureGrid::m_fScale)
    )
)
// clang-format on
#include "mirrow/srefl/srefl_end.hpp"
