#include "TextureRender.hpp"
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

    for (auto entity : view) {
        auto& texture = view.get<app::comp::Texture>(entity);

        // transform world object to screen
        if (const auto* trans = reg.try_get<game::comp::Transform>(entity)) {
            const Camera& cam = Camera::GetInstance();
            const vec2 cam_fov = cam.GetHalfFOV() * 2.f;
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

        SDL_RenderTexture(renderer.GetSDLRenderer(), texture.m_pTexture.get(),
            texture.m_AvbFRect ? &texture.m_AvbFRect.value() : nullptr,
            texture.m_DstFRect ? &texture.m_DstFRect.value() : nullptr);
    }
}
