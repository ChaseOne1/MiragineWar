#pragma once

namespace app::sys {
class TextureRender : public utility::Singleton<TextureRender>
{
    friend class utility::Singleton<TextureRender>;

private:
    TextureRender();
    ~TextureRender() = default;

    static void SortTexture(entt::registry&, entt::entity);

public:
    void Tick();
};
}
