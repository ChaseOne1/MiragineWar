#pragma once

namespace app::comp {
struct Texture
{
    uint32_t m_nZIndex = 0u; float m_fAngle = 0.f;
    std::optional<SDL_FPoint> m_RotationPivot;

    // TODO: refactor with vec4
    std::optional<SDL_FRect> m_SrcFRect, m_DstFRect;

    std::shared_ptr<SDL_Texture> m_pTexture {};

public:
    explicit Texture(const std::shared_ptr<SDL_Texture>& texture)
        : m_pTexture(texture)
    { }

    Texture(const std::shared_ptr<SDL_Texture>& texture,
        const std::optional<SDL_FRect>& srcFRect, const std::optional<SDL_FRect>& dstFRect,
        uint32_t zindex)
        : m_pTexture(texture)
        , m_SrcFRect(srcFRect)
        , m_DstFRect(dstFRect)
        , m_nZIndex(zindex)
    { }

    Texture(const std::shared_ptr<SDL_Texture>& texture,
        const std::optional<SDL_FRect>& srcFRect, const std::optional<SDL_FRect>& dstFRect,
        const std::optional<SDL_FPoint>& rotationPivot, float rotationAngle, uint32_t zindex)
        : m_pTexture(texture)
        , m_SrcFRect(srcFRect)
        , m_DstFRect(dstFRect)
        , m_nZIndex(zindex)
        , m_fAngle(rotationAngle)
        , m_RotationPivot(rotationPivot)
    { }
};
}
