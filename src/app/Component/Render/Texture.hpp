#pragma once

namespace app::comp {
struct Texture
{
    uint8_t m_bDirty = 0, m_nZIndex = 0;

    //TODO: refactor with vec4
    std::optional<SDL_FRect> m_AvbFRect {}, m_DstFRect {};

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
        , m_AvbFRect(std::forward<OptionalRect>(avbRect))
        , m_DstFRect(std::forward<OptionalRect>(dstRect))
    { }
};
}
