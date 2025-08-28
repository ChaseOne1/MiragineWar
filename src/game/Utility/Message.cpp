#include "Message.hpp"
#include "SDL3_ttf/SDL_ttf.h"
#include "app/Component/Render/Texture.hpp"
#include "app/Component/Render/ZIndex.hpp"
#include "app/Component/Render/TextureGrid.hpp"
#include "app/EventBus.hpp"
#include "app/Resources.hpp"
#include "app/resources/AllInOneIndex.hpp"
#include "app/resources/Mox.hpp"
#include "app/Settings.hpp"
#include "app/Component/Render/Text.hpp"
#include "game/Component/Logic.hpp"
#include "game/Component/Pointable.hpp"
#include "game/Component/UIElement.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/Movement.hpp"
#include "game/Component/Clickable.hpp"
#include "game/Component/Invalid.hpp"

using namespace game::util;
using namespace entt;
using namespace mathfu;

const unsigned short Message::msc_LongBoxWidth = app::Settings::GetInstance().GetSettings().at_path("Text.standard_long_box_width").value_or(600u);
const float Message::msc_StandardBoxScale = app::Settings::GetInstance().GetSettings().at_path("Text.standard_box_scale").value_or(0.6f);

Message::Message(std::string_view msg, app::GUID_t font, app::comp::ZIndex zindex)
    : m_Font(TTF_CopyFont(app::Resources::GetInstance().Require<TTF_Font>(font, app::idx::TEXT_STYLE_IDX).get()),
          [](TTF_Font* font) { TTF_CloseFont(font); })
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();

    // TTF_SetFontLineSkip(m_Font.get(), app::Settings::GetInstance().GetSettings().at_path("Text.line_skip").value_or(26.f));

    reg.emplace<app::comp::Text>(m_Message, msg, m_Font);

    SetColor(0, 0, 0);

    if (zindex == app::comp::ZINDEX_UIELEMENT) {
        reg.emplace<game::comp::UIElement>(m_Message);
    }

    reg.emplace<app::comp::ZIndex>(m_Message, zindex);
}

Message::Message(const Message& other)
    : m_Font(other.m_Font)
    , m_BoxStyle(other.m_BoxStyle)
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    for (auto [id, storage] : reg.storage())
        if (storage.contains(other.m_Message)) storage.push(m_Message, storage.value(other.m_Message));
}

Message::Message(Message&& other) noexcept
    : m_Message(other.m_Message)
    , m_Font(std::move(other.m_Font))
    , m_BoxStyle(std::move(other.m_BoxStyle))
{
    other.m_Message = entt::null;
}

Message& Message::operator=(const Message& other)
{
    if (this == &other) return *this;

    registry& reg = utility::Registry::GetInstance().GetRegistry();
    if (reg.valid(m_Message)) reg.destroy(m_Message);

    m_Message = reg.create();

    for (auto [id, storage] : reg.storage())
        if (storage.contains(other.m_Message)) storage.push(m_Message, storage.value(other.m_Message));

    m_Font = other.m_Font;
    m_BoxStyle = other.m_BoxStyle;
    m_ResizeID = other.m_ResizeID;

    return *this;
}

Message& Message::operator=(Message&& other) noexcept
{
    if (this == &other) return *this;

    m_Message = other.m_Message;
    other.m_Message = entt::null;
    m_Font = std::move(other.m_Font);
    m_BoxStyle = std::move(other.m_BoxStyle);
    m_ResizeID = other.m_ResizeID;

    return *this;
}

Message::~Message()
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    if (reg.valid(m_Message)) reg.destroy(m_Message);
    if (m_ResizeID != utility::TopicsSubscriberIDNull)
        app::EventBus::Unsubscribe(SDL_EVENT_WINDOW_RESIZED, m_ResizeID);
}

std::string_view Message::GetString() const
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    return { reg.get<app::comp::Text>(m_Message).m_Text->text };
}

void Message::SetString(std::string_view msg)
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    auto& text = reg.get<app::comp::Text>(m_Message);
    TTF_SetTextString(text.m_Text.get(), msg.data(), msg.length());
}

void Message::SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    auto& text = reg.get<app::comp::Text>(m_Message);
    TTF_SetTextColor(text.m_Text.get(), r, g, b, a);
}

void Message::SetPosition(float x, float y, app::Layout::Anchor anchor)
{

    if (anchor == Anchor_Null) {
        registry& reg = utility::Registry::GetInstance().GetRegistry();
        reg.get_or_emplace<game::comp::Transform>(m_Message).m_Position = vec2 { x, y };
        return;
    }

    auto resize_cb = [x, y, anchor, this](const SDL_Event*) {
        registry& reg = utility::Registry::GetInstance().GetRegistry();
        SDL_FRect posn = app::Layout::GetInstance().Transform(anchor, SDL_FPoint { x, y }, SDL_FPoint {});
        reg.get_or_emplace<game::comp::Transform>(m_Message).m_Position = vec2 { posn.x, posn.y };
    };
    resize_cb(nullptr);
    m_ResizeID = app::EventBus::Subscribe(SDL_EVENT_WINDOW_RESIZED, resize_cb);
}

void Message::SetOffset(float x, float y)
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    reg.get<app::comp::Text>(m_Message).m_Offset = { x, y };
}

void Message::SetMovement(mathfu::vec2 velocity, mathfu::vec2 accleration)
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    reg.emplace_or_replace<game::comp::Movement>(m_Message, std::move(velocity), std::move(accleration));
}

void Message::SetBox(app::GUID_t box, int w, int h, float scale)
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    m_BoxStyle = app::Resources::GetInstance().Require<app::Mox>(box, app::idx::TEXT_STYLE_IDX);
    SDL_SetTextureScaleMode(m_BoxStyle->m_Texture.get(), SDL_SCALEMODE_NEAREST);

    reg.emplace_or_replace<app::comp::Texture>(m_Message, m_BoxStyle->m_Texture, std::nullopt);
    auto& grid = reg.emplace_or_replace<app::comp::TextureGrid>(m_Message, m_BoxStyle->m_Header);
    if (const auto& settings = app::Settings::GetInstance().GetSettings(); scale) grid.m_fScale = scale;
    else grid.m_fScale = TTF_GetFontSize(m_Font.get()) / settings.at_path("Text.standard_font_size").value_or(32.f);

    int textw = 0, texth = 0;
    TTF_GetTextSize(reg.get<app::comp::Text>(m_Message).m_Text.get(), &textw, &texth);
    auto& tsfm = reg.get_or_emplace<game::comp::Transform>(m_Message);
    if (w == 0)
        tsfm.m_HalfSize.x = ((m_BoxStyle->m_Header.left_width + m_BoxStyle->m_Header.right_width) * grid.m_fScale + textw) / 2.f;
    else
        tsfm.m_HalfSize.x = w / 2.f;
    if (h == 0)
        tsfm.m_HalfSize.y = ((m_BoxStyle->m_Header.bottom_height + m_BoxStyle->m_Header.top_height) * grid.m_fScale + texth) / 2.f;
    else
        tsfm.m_HalfSize.y = h / 2.f;
}

void Message::SetAlignment(Alignment alignment)
{
    if (!m_BoxStyle) {
        SDL_Log("WARNING: setting alignment on a text without a box");
        return;
    }

    registry& reg = utility::Registry::GetInstance().GetRegistry();
    const auto& [grid, tsfm] = reg.get<app::comp::TextureGrid, game::comp::Transform>(m_Message);
    int textw = 0, texth = 0;
    TTF_GetTextSize(reg.get<app::comp::Text>(m_Message).m_Text.get(), &textw, &texth);

    switch (alignment) {
    case Alignment::LEFT:
        TTF_SetFontWrapAlignment(m_Font.get(), TTF_HORIZONTAL_ALIGN_LEFT);
        SetOffset(-(tsfm.m_HalfSize.x - m_BoxStyle->m_Header.left_width * grid.m_fScale - textw / 2.f), 0.f);
        break;
    case Alignment::CENTER:
        TTF_SetFontWrapAlignment(m_Font.get(), TTF_HORIZONTAL_ALIGN_CENTER);
        SetOffset(0.f, 0.f);
        break;
    case Alignment::RIGHT:
        TTF_SetFontWrapAlignment(m_Font.get(), TTF_HORIZONTAL_ALIGN_RIGHT);
        SetOffset(tsfm.m_HalfSize.x - m_BoxStyle->m_Header.right_width * grid.m_fScale - textw / 2.f, 0.f);
        break;
    default:
        SDL_Log("WARNING: invalid alignment");
        break;
    }
}

void Message::SetLogic(std::function<void()> logic)
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    reg.emplace_or_replace<game::comp::Logic>(m_Message, std::move(logic));
}

void Message::SetClickable(std::function<void()> clickInCallback)
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    auto& tsfm = reg.get<game::comp::Transform>(m_Message);
    reg.emplace_or_replace<game::comp::Clickable>(m_Message, std::move(clickInCallback),
        tsfm.m_Position.x, tsfm.m_Position.y, tsfm.m_HalfSize.x, tsfm.m_HalfSize.y);
}

void Message::SetClickable(std::function<void()> clickInCallback, std::function<void()> clickOutCallback)
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    auto& tsfm = reg.get<game::comp::Transform>(m_Message);
    reg.emplace_or_replace<game::comp::Clickable>(m_Message, std::move(clickInCallback), std::move(clickOutCallback),
        tsfm.m_Position.x, tsfm.m_Position.y, tsfm.m_HalfSize.x, tsfm.m_HalfSize.y);
}

void Message::SetPointable(std::function<void()> pointInCallback)
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    auto& tsfm = reg.get<game::comp::Transform>(m_Message);
    reg.emplace_or_replace<game::comp::Pointable>(m_Message, std::move(pointInCallback),
        tsfm.m_Position.x, tsfm.m_Position.y, tsfm.m_HalfSize.x, tsfm.m_HalfSize.y);
}

void Message::SetPointable(std::function<void()> pointInCallback, std::function<void()> pointOutCallback)
{

    registry& reg = utility::Registry::GetInstance().GetRegistry();
    auto& tsfm = reg.get<game::comp::Transform>(m_Message);
    reg.emplace_or_replace<game::comp::Pointable>(m_Message, std::move(pointInCallback), std::move(pointOutCallback),
        tsfm.m_Position.x, tsfm.m_Position.y, tsfm.m_HalfSize.x, tsfm.m_HalfSize.y);
}

void Message::SetInvalid()
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    reg.emplace_or_replace<game::comp::Invalid>(m_Message);
}
