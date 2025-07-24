#pragma once

namespace app::comp {
struct Texture
{
    std::shared_ptr<SDL_Texture> m_pTexture {};
    // TODO: refactor with vec4
    std::optional<SDL_FRect> m_SrcFRect; //, m_DstFRect;

public:
    explicit Texture(const std::shared_ptr<SDL_Texture>& texture)
        : m_pTexture(texture)
    { }

    Texture(const std::shared_ptr<SDL_Texture>& texture,
        const std::optional<SDL_FRect>& srcFRect /*, const std::optional<SDL_FRect>& dstFRect*/)
        : m_pTexture(texture)
        , m_SrcFRect(srcFRect)
    //, m_DstFRect(dstFRect)
    { }
};
}
