#pragma once
#include "EventBus.hpp"
#include "Renderer.hpp"
#include "app/ScriptManager.hpp"

namespace app {
class Layout : public utility::Singleton<Layout>
{
    friend class utility::Singleton<Layout>;

public:
    using Anchor = std::string_view;
#define AnchorNull "AnchorNull"
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

    std::unordered_map<Anchor, SDL_FPoint> m_Anchors;
    SDL_FPoint m_fScale {};
    // TODO: a scale factory

private:
    Layout()
    {
        UpdateAnchors(nullptr);
        EventBus::GetInstance().Subscribe(SDL_EVENT_WINDOW_RESIZED, std::bind(&Layout::UpdateAnchors, this, std::placeholders::_1));

        auto type = app::ScriptManager::GetLuaState().new_usertype<Layout>("Layout", sol::no_constructor);
        type["transform"] = LuaTransform;
        type["scale"] = sol::var(&m_fScale);
    }

    ~Layout() = default;

    void UpdateAnchors(const SDL_Event* event)
    {
        SDL_Renderer* renderer = Renderer::GetInstance().GetSDLRenderer();
        float w = 0, h = 0;
        {
            int iw = 0, ih = 0;
            SDL_GetCurrentRenderOutputSize(renderer, &iw, &ih);
            w = iw, h = ih;
        }

        m_Anchors[AnchorNull] = SDL_FPoint { 0, 0 };
        m_Anchors[LeftTop] = SDL_FPoint { 0, 0 };
        m_Anchors[LeftCenter] = SDL_FPoint { 0, h / 2.f };
        m_Anchors[LeftBottom] = SDL_FPoint { 0, h };
        m_Anchors[MiddleTop] = SDL_FPoint { w / 2.f, 0 };
        m_Anchors[MiddleCenter] = SDL_FPoint { w / 2.f, h / 2.f };
        m_Anchors[MiddleBottom] = SDL_FPoint { w / 2.f, h };
        m_Anchors[RightTop] = SDL_FPoint { w, 0 };
        m_Anchors[RightCenter] = SDL_FPoint { w, h / 2.f };
        m_Anchors[RightBottom] = SDL_FPoint { w, h };

        m_fScale = { w / (float)mc_nDefaultWindowSize.x, h / (float)mc_nDefaultWindowSize.y };
    }


public:
    static const SDL_FPoint& GetScale() noexcept
    {
        return GetInstance().m_fScale;
    }

    SDL_FRect Transform(Anchor anchor, const SDL_FPoint& offset, const SDL_FPoint& size) const
    {
        try {
            const SDL_FPoint& anchor_pos = m_Anchors.at(anchor);
            return { anchor_pos.x + offset.x * m_fScale.x - size.x * m_fScale.x / 2.f, anchor_pos.y + offset.y * m_fScale.y - size.y * m_fScale.y / 2.f,
                size.x * m_fScale.x, size.y * m_fScale.y };
        } catch (std::out_of_range e) {
            SDL_Log("%s\n", e.what());
            return {};
        }
    }

    static SDL_FRect& Transform(Anchor anchor, const mathfu::vec2& offset, const mathfu::vec2& half_size, SDL_FRect& out)
    {
        const auto& inst = GetInstance();
        const SDL_FPoint& anchor_pos = inst.m_Anchors.at(anchor);
        out.x = anchor_pos.x + offset.x * inst.m_fScale.x - half_size.x * inst.m_fScale.x;
        out.y = anchor_pos.y + offset.y * inst.m_fScale.y - half_size.y * inst.m_fScale.y;
        out.w = half_size.x * inst.m_fScale.x * 2.0;
        out.h = half_size.y * inst.m_fScale.y * 2.0;
        return out;
    }

    static mathfu::vec2 Transform(Anchor anchor, const mathfu::vec2& offset)
    {
        const auto& inst = GetInstance();
        const SDL_FPoint& anchor_pos = inst.m_Anchors.at(anchor);
        return { anchor_pos.x + offset.x * inst.m_fScale.x, anchor_pos.y + offset.y * inst.m_fScale.y };
    }

    static mathfu::vec4 Transform(Anchor anchor, const mathfu::vec4& rect)
    {
        const auto& inst = GetInstance();
        const SDL_FPoint& anchor_pos = inst.m_Anchors.at(anchor);
        return { anchor_pos.x + rect.x * inst.m_fScale.x, anchor_pos.y + rect.y * inst.m_fScale.y,
            rect.z * inst.m_fScale.x, rect.w * inst.m_fScale.y };
    }

    static SDL_FPoint GetAnchor(Anchor anchor) { return GetInstance().m_Anchors.at(anchor); }

    static SDL_FRect Transform(SDL_Point anchor, const SDL_FPoint& offset, const SDL_FPoint& size)
    {
        return { anchor.x + offset.x - size.x / 2.f, anchor.y + offset.y - size.y / 2.f,
            anchor.x + offset.x + size.x / 2.f, anchor.y + offset.y + size.y / 2.f };
    }

    static sol::table LuaTransform(sol::object anchor, sol::table layout, sol::this_environment e)
    {
        sol::environment& env = e;

        if (auto anc = anchor.as<sol::optional<Anchor>>(); anc) {
            const auto& [x, y] = GetInstance().m_Anchors.at(anc.value());
            return env.create_with(
                "x", x + layout["x"].get<float>() * GetInstance().m_fScale.x,
                "y", y + layout["y"].get<float>() * GetInstance().m_fScale.y,
                "w", layout["w"].get_or(0.f) * GetInstance().m_fScale.x,
                "h", layout["h"].get_or(0.f) * GetInstance().m_fScale.y);
        } else {
            return env.create_with(
                "w", layout["w"].get<float>() * GetInstance().m_fScale.x,
                "h", layout["h"].get<float>() * GetInstance().m_fScale.y);
        }
    }
};
}
