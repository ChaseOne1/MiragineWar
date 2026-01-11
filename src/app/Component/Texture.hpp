#pragma once
#include "app/ScriptComponent.hpp"

namespace app::comp {
struct Texture : app::ScriptComponent<Texture>
{
    std::shared_ptr<SDL_Texture> m_pTexture {};
    std::optional<SDL_FRect> m_SrcFRect;

public:
    explicit Texture(const std::shared_ptr<SDL_Texture>& texture)
        : m_pTexture(texture)
    { }

    Texture(const std::shared_ptr<SDL_Texture>& texture,
        std::optional<SDL_FRect> srcFRect)
        : m_pTexture(texture)
        , m_SrcFRect(srcFRect)
    { }
};
}

#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off
srefl_class(app::comp::Texture,
    ctors(
        ctor(const std::shared_ptr<SDL_Texture>&),
        ctor(const std::shared_ptr<SDL_Texture>&, std::optional<SDL_FRect>)
        )
    fields(
        field(&app::comp::Texture::m_SrcFRect)
    )
)
// clang-format on
#include "mirrow/srefl/srefl_end.hpp"
