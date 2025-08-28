#pragma once
#include "app/Layout.hpp"
#include "utility/Registry.hpp"
#include "app/resources/AllInOneRes.hpp"
#include "app/Component/Render/ZIndex.hpp"
#include "utility/Topics.hpp"

namespace app {
struct Mox;
}

namespace game::util {
class Message
{
    entt::entity m_Message = utility::Registry::GetInstance().GetRegistry().create();
    std::shared_ptr<TTF_Font> m_Font;
    std::shared_ptr<app::Mox> m_BoxStyle;
    utility::TopicsSubscriberID m_ResizeID = utility::TopicsSubscriberIDNull;

public:
    enum class Alignment
    {
        LEFT,
        CENTER,
        RIGHT,
    };

    const static unsigned short msc_LongBoxWidth;
    const static float msc_StandardBoxScale;

public:
    explicit Message(std::string_view msg, app::GUID_t font = app::res::SUBTITLE_TTF, app::comp::ZIndex zindex = app::comp::ZINDEX_UIELEMENT);
    Message(const Message&);
    Message(Message&&) noexcept;

    ~Message();

    Message& operator=(const Message&);
    Message& operator=(Message&&) noexcept;

    std::shared_ptr<TTF_Font> GetFont() const noexcept { return m_Font; }

    entt::entity GetEntity() const noexcept { return m_Message; }

    // NOTE: the box size and offset will not be changed, the alignment performance may be wrong
    void SetString(std::string_view msg);
    std::string_view GetString() const;

    void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255u);
    void SetPosition(float x, float y, app::Layout::Anchor = Anchor_Null);
    void SetOffset(float x, float y);
    void SetMovement(mathfu::vec2 velocity, mathfu::vec2 accleration = mathfu::kZeros2f);
    void SetBox(app::GUID_t box, int w = 0, int h = 0, float scale = 0.f);
    void SetAlignment(Alignment alignment);
    void SetLogic(std::function<void()> logic);
    void SetClickable(std::function<void()> clickInCallback);
    void SetClickable(std::function<void()> clickInCallback, std::function<void()> clickOutCallback);
    void SetPointable(std::function<void()> pointInCallback);
    void SetPointable(std::function<void()> pointInCallback, std::function<void()> pointOutCallback);
    void SetInvalid();
};
}
