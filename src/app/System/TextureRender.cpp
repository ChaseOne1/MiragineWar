#include "TextureRender.hpp"
#include "app/Component/Render/TextureGrid.hpp"
#include "app/Component/Render/TextureRotation.hpp"
#include "app/component/Render/Texture.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/Visible.hpp"
#include "game/Camera.hpp"

using namespace app;
using namespace app::sys;
using namespace entt;
using namespace mathfu;
using namespace game;

TextureRender::TextureRender()
{
    // to avoid sorting before each rendering
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    reg.on_construct<comp::Texture>().connect<&TextureRender::SortTexture>();
    reg.on_update<comp::Texture>().connect<&TextureRender::SortTexture>();
    // TODO: on destroy
}

void TextureRender::SortTexture(registry& reg, entity)
{
    reg.sort<comp::Texture>(
        [](const comp::Texture& lhs, const comp::Texture& rhs) { return lhs.m_nZIndex < rhs.m_nZIndex; });
}

void TextureRender::Tick()
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    const Renderer& renderer = Renderer::GetInstance();
    auto view = reg.view<app::comp::Texture, game::comp::Visible>();
    view.use<app::comp::Texture>(); // let the view follow the order of Texture while iterating

    for (auto entity : view) {
        auto& texture = view.get<app::comp::Texture>(entity);

        // transform object's vertices in world to screen
        if (const auto* trans = reg.try_get<game::comp::Transform>(entity); trans && trans->m_HalfSize != kZeros2f) {
            const Camera& cam = Camera::GetInstance();
            const vec2 cam_fov = reg.get<game::comp::Transform>(cam.GetCameraEntity()).m_HalfSize.xy() * 2.f;
            const vec2 screen_size { renderer.GetRenderSize() };

            std::array<vec3, 4> vertices = trans->GetVertices();
            for (auto& vertex : vertices) {
                vertex = cam.GetCameraLeftTopTransformMatrix() * vertex;
                vertex = vec3 { vertex.xy() / cam_fov.xy() * screen_size.xy(), 1.f };
            }
            texture.m_DstFRect->x = vertices[0].x;
            texture.m_DstFRect->y = vertices[0].y;
            texture.m_DstFRect->w = vertices[1].x - vertices[0].x;
            texture.m_DstFRect->h = vertices[2].y - vertices[0].y;
        }

        SDL_FlipMode flip_mode = SDL_FlipMode::SDL_FLIP_NONE;
        if (texture.m_SrcFRect && texture.m_SrcFRect->w < 0.f) {
            flip_mode = SDL_FlipMode::SDL_FLIP_HORIZONTAL;
            texture.m_SrcFRect->w = -texture.m_SrcFRect->w;
        } else if (texture.m_SrcFRect && texture.m_SrcFRect->h < 0.f) {
            flip_mode = SDL_FlipMode::SDL_FLIP_VERTICAL;
            texture.m_SrcFRect->h = -texture.m_SrcFRect->h;
        }

        if (auto* rotation = reg.try_get<app::comp::TextureRotation>(entity)) {
            SDL_RenderTextureRotated(renderer.GetSDLRenderer(), texture.m_pTexture.get(),
                texture.m_SrcFRect ? &texture.m_SrcFRect.value() : nullptr,
                texture.m_DstFRect ? &texture.m_DstFRect.value() : nullptr,
                rotation->m_fAngle,
                rotation->m_RotationPivot ? &rotation->m_RotationPivot.value() : nullptr,
                flip_mode);
        } else if (auto* grid = reg.try_get<app::comp::TextureGrid>(entity)) {
            SDL_RenderTexture9GridTiled(renderer.GetSDLRenderer(), texture.m_pTexture.get(),
                texture.m_SrcFRect ? &texture.m_SrcFRect.value() : nullptr,
                grid->m_fLeftWidth, grid->m_fRightWidth, grid->m_fTopHeight, grid->m_fBottomHeight, grid->m_fScale,
                texture.m_DstFRect ? &texture.m_DstFRect.value() : nullptr,
                grid->m_fTileScale);
        } else {
            SDL_RenderTextureRotated(renderer.GetSDLRenderer(), texture.m_pTexture.get(),
                texture.m_SrcFRect ? &texture.m_SrcFRect.value() : nullptr,
                texture.m_DstFRect ? &texture.m_DstFRect.value() : nullptr,
                0.f, nullptr, flip_mode);
        }

        if (flip_mode == SDL_FLIP_HORIZONTAL) texture.m_SrcFRect->w = -texture.m_SrcFRect->w;
        else if (flip_mode == SDL_FLIP_VERTICAL) texture.m_SrcFRect->h = -texture.m_SrcFRect->h;
    }
}
