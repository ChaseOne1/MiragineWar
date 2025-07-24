#include "Render.hpp"
#include "app/Component/Render/TextureGrid.hpp"
#include "app/Component/Render/TextureRotation.hpp"
#include "app/Component/Render/ZIndex.hpp"
#include "app/component/Render/Texture.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/UIElement.hpp"
#include "game/Component/Visible.hpp"
#include "game/Camera.hpp"

using namespace app;
using namespace app::sys;
using namespace entt;
using namespace mathfu;
using namespace game;

static void SortZIndex(registry& reg, entity ent)
{
    reg.sort<app::comp::ZIndex>(
        [](const app::comp::ZIndex& lhs, const app::comp::ZIndex& rhs) { return lhs < rhs; });
}

Render::Render()
{
    // to avoid sorting before each rendering
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    reg.on_construct<app::comp::ZIndex>().connect<SortZIndex>();
    reg.on_update<app::comp::ZIndex>().connect<SortZIndex>();
}

void Render::Tick()
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    const Renderer& renderer = Renderer::GetInstance();
    auto view = reg.view<app::comp::Texture, game::comp::Visible, app::comp::ZIndex>();
    view.use<app::comp::ZIndex>(); // let the view follow the order of zindex while iterating

    for (auto entity : view) {
        auto& texture = view.get<app::comp::Texture>(entity);
        std::optional<SDL_FRect> dstFRect;

        // transform object's vertices in world to screen
        if (const auto* trans = reg.try_get<game::comp::Transform>(entity); trans && !reg.all_of<game::comp::UIElement>(entity)) {
            const Camera& cam = Camera::GetInstance();
            const vec2 cam_fov = reg.get<game::comp::Transform>(cam.GetCameraEntity()).m_HalfSize.xy() * 2.f;
            const vec2 screen_size { renderer.GetRenderSize() };

            std::array<vec3, 4> vertices = trans->GetVertices();
            for (auto& vertex : vertices) {
                vertex = cam.GetCameraLeftTopTransformMatrix() * vertex;
                vertex = vec3 { vertex.xy() / cam_fov.xy() * screen_size.xy(), 1.f };
            }
            dstFRect = std::make_optional(SDL_FRect { vertices[0].x, vertices[0].y,
                vertices[1].x - vertices[0].x, vertices[2].y - vertices[0].y });
        } else if (trans) {
            dstFRect = std::make_optional(trans->GetFRect());
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
                dstFRect ? &dstFRect.value() : nullptr,
                rotation->m_fAngle,
                rotation->m_RotationPivot ? &rotation->m_RotationPivot.value() : nullptr,
                flip_mode);
        } else if (auto* grid = reg.try_get<app::comp::TextureGrid>(entity)) {
            SDL_RenderTexture9GridTiled(renderer.GetSDLRenderer(), texture.m_pTexture.get(),
                texture.m_SrcFRect ? &texture.m_SrcFRect.value() : nullptr,
                grid->m_fLeftWidth, grid->m_fRightWidth, grid->m_fTopHeight, grid->m_fBottomHeight, grid->m_fScale,
                dstFRect ? &dstFRect.value() : nullptr,
                grid->m_fTileScale);
        } else {
            SDL_RenderTextureRotated(renderer.GetSDLRenderer(), texture.m_pTexture.get(),
                texture.m_SrcFRect ? &texture.m_SrcFRect.value() : nullptr,
                dstFRect ? &dstFRect.value() : nullptr,
                0.f, nullptr, flip_mode);
        }

        if (flip_mode == SDL_FLIP_HORIZONTAL) texture.m_SrcFRect->w = -texture.m_SrcFRect->w;
        else if (flip_mode == SDL_FLIP_VERTICAL) texture.m_SrcFRect->h = -texture.m_SrcFRect->h;
    }
}
