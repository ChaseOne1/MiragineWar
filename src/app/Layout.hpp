#pragma once
#include "Component/Render/Texture.hpp"

namespace app {
class Layout : public utility::Singleton<Layout>
{
    using Anchor = std::string;
    friend class utility::Singleton<Layout>;

#define LeftTop "LeftTop"
#define MiddleTop "MiddleTop"
#define RightTop "RightTop"

#define LeftCenter "LeftCenter"
#define MiddleCenter "MiddleCenter"
#define RightCenter "RightCenter"

#define LeftBottom "LeftBottom"
#define MiddleBottom "MiddleBottom"
#define RightBottom "RightBottom"

private:
    static constexpr SDL_Point mc_nDefaultWindowSize { 1920, 1080 };

    std::unordered_map<Anchor, SDL_Point> m_Anchors;
    SDL_FPoint m_fScale {};
    // TODO: a scale factory

private:
    Layout()
    {
        UpdateAnchors();
        EventBus::GetInstance().Subscribe(SDL_EVENT_WINDOW_RESIZED, std::bind(&Layout::UpdateAnchors, this));
    }

    ~Layout() = default;

    void UpdateAnchors()
    {
        SDL_Renderer* renderer = Renderer::GetInstance().GetRenderer();
        int w = 0, h = 0;
        SDL_GetCurrentRenderOutputSize(renderer, &w, &h);

        m_Anchors[LeftTop] = SDL_Point { 0, 0 };
        m_Anchors[LeftCenter] = SDL_Point { 0, h >> 1 };
        m_Anchors[LeftBottom] = SDL_Point { 0, h };
        m_Anchors[MiddleTop] = SDL_Point { w >> 1, 0 };
        m_Anchors[MiddleCenter] = SDL_Point { w >> 1, h >> 1 };
        m_Anchors[MiddleBottom] = SDL_Point { w >> 1, h };
        m_Anchors[RightTop] = SDL_Point { w, 0 };
        m_Anchors[RightCenter] = SDL_Point { w, h >> 1 };
        m_Anchors[RightBottom] = SDL_Point { w, h };

        m_fScale = { w / (float)mc_nDefaultWindowSize.x, h / (float)mc_nDefaultWindowSize.y };
    }

public:
    SDL_FRect Transform(Anchor anchor, const SDL_FPoint& offset, const SDL_FPoint& size)
    {
        try {
            const SDL_Point& anchor_pos = m_Anchors.at(anchor);
            return { anchor_pos.x + offset.x * m_fScale.x - size.x * m_fScale.x / 2.f, anchor_pos.y + offset.y * m_fScale.y - size.y * m_fScale.y / 2.f,
                size.x * m_fScale.x, size.y * m_fScale.y };
        } catch (std::out_of_range e) {
            SDL_Log("%s\n", e.what());
            return {};
        }
    }

    SDL_FRect Transform(toml::node_view<toml::node> node)
    {
        try {
            return Transform(node["Anchor"].value_or<Anchor>(""),
                SDL_FPoint { node.at_path("Offset.x").value_or<float>(0.f), node.at_path("Offset.y").value_or<float>(0.f) },
                SDL_FPoint { node.at_path("Size.w").value_or<float>(2.f), node.at_path("Size.h").value_or<float>(2.f) });
        } catch (toml::parse_error e) {
            SDL_Log("%s\n", e.description().data());
            return {};
        }
    }

    static SDL_FRect Transform(SDL_Point anchor, const SDL_FPoint& offset, const SDL_FPoint& size)
    {
        return { anchor.x + offset.x - size.x / 2.f, anchor.y + offset.y - size.y / 2.f,
            anchor.x + offset.x + size.x / 2.f, anchor.y + offset.y + size.y / 2.f };
    }
};
}
