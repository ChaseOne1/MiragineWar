#pragma once

namespace app::comp {
struct Texture
{
    uint8_t m_bDirty = 0, m_nZIndex = 0;

    std::optional<SDL_FRect> m_AvbRect {}, m_DstRect {};

    std::shared_ptr<SDL_Texture> m_pTexture {};

public:
    template <typename TexturePtr>
    explicit Texture(TexturePtr&& texture)
        : m_pTexture(std::forward<TexturePtr>(texture))
    { }

    template <typename TexturePtr>
    explicit Texture(TexturePtr&& texture, uint8_t zindex)
        : m_pTexture(std::forward<TexturePtr>(texture))
        , m_nZIndex(zindex)
    { }

    template <typename TexturePtr, typename OptionalRect>
    Texture(TexturePtr&& texture, uint8_t zindex, OptionalRect&& avbRect, OptionalRect&& dstRect)
        : m_pTexture(std::forward<TexturePtr>(texture))
        , m_nZIndex(zindex)
        , m_AvbRect(std::forward<OptionalRect>(avbRect))
        , m_DstRect(std::forward<OptionalRect>(dstRect))
    { }
};
}
