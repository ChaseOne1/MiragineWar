#include "TextureRender.hpp"
#include "app/component/Render/Texture.hpp"
#include "game/Component/Visible.hpp"

using namespace app;
using namespace app::sys;
using namespace entt;

TextureRender::TextureRender()
{
    // to avoid sorting before each rendering
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    reg.on_construct<comp::Texture>().connect<&TextureRender::SortTexture>();
    reg.on_update<comp::Texture>().connect<&TextureRender::SortTexture>();
}

void TextureRender::SortTexture(registry& reg, entity)
{
    reg.sort<comp::Texture>(
        [](const comp::Texture& lhs, const comp::Texture& rhs) { return lhs.m_nZIndex < rhs.m_nZIndex; });
}

void TextureRender::Tick()
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    SDL_Renderer* renderer = Renderer::GetInstance().GetRenderer();

    auto view = reg.view<app::comp::Texture, game::comp::Visible>();
    for (auto entity : view) {
        const auto& texture = view.get<app::comp::Texture>(entity);
        SDL_RenderTexture(renderer, texture.m_pTexture.get(),
            texture.m_AvbRect ? &texture.m_AvbRect.value() : nullptr,
            texture.m_DstRect ? &texture.m_DstRect.value() : nullptr);
    }
}
