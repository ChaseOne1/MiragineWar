#pragma once

namespace app::comp {
struct TextureGrid
{
    float m_fLeftWidth = 0.f, m_fRightWidth = 0.f;
    float m_fTopHeight = 0.f, m_fBottomHeight = 0.f;
    // TODO: if the tile_scale is equal to 0.f, then the RenderTexture9Grid is used, or Tiled
    float m_fScale = 1.f, m_fTileScale = 1.f;
};
}
