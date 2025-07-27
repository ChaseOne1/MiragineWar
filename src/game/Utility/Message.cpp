#include "Message.hpp"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "app/Component/Render/Texture.hpp"
#include "app/Component/Render/ZIndex.hpp"
#include "app/Resources.hpp"
#include "app/Settings.hpp"
#include "app/resources/AllInOneIndex.hpp"
#include "app/Component/Render/Text.hpp"
#include "game/Component/UIElement.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/Movement.hpp"
#include "app/resources/Mox.hpp"
#include "app/Component/Render/TextureGrid.hpp"

using namespace game::util;
using namespace entt;
using namespace mathfu;

const unsigned short Message::msc_LongBoxWidth = app::Settings::GetInstance().GetSettings().at_path("Text.long_box_width").value_or(600u);
const float Message::msc_StandardBoxScale = app::Settings::GetInstance().GetSettings().at_path("Text.standard_box_scale").value_or(0.6f);

Message::Message(std::string_view msg, app::GUID_t font, bool isUIElement)
    : m_Font(app::Resources::GetInstance().Require<TTF_Font>(font, app::idx::TEXT_STYLE_IDX))
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();

    reg.emplace<app::comp::Text>(m_Message, msg, m_Font);

    app::comp::ZIndex zindex = app::comp::ZINDEX_SOLDIER;

    if (isUIElement) {
        reg.emplace<game::comp::UIElement>(m_Message);
        zindex = app::comp::ZINDEX_UIELEMENT;
    }

    reg.emplace<app::comp::ZIndex>(m_Message, zindex);
}

Message::~Message()
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    if (reg.valid(m_Message)) reg.destroy(m_Message);
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

void Message::SetPosition(float x, float y)
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    auto& tsfm = reg.get_or_emplace<game::comp::Transform>(m_Message);
    tsfm.m_Position.x = x;
    tsfm.m_Position.y = y;
}

void Message::SetOffset(float x, float y)
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    auto& text = reg.get<app::comp::Text>(m_Message);

    text.m_Offset = { x, y };
}

void Message::SetMovement(mathfu::vec2 velocity, mathfu::vec2 accleration)
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    reg.emplace_or_replace<game::comp::Movement>(m_Message, std::move(velocity), std::move(accleration));
}

void Message::SetBox(app::GUID_t box, mathfu::vec2i size, float scale)
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
    if (size.x == 0)
        tsfm.m_HalfSize.x = ((m_BoxStyle->m_Header.left_width + m_BoxStyle->m_Header.right_width) * grid.m_fScale + textw) / 2.f;
    else
        tsfm.m_HalfSize.x = size.x / 2.f;
    if (size.y == 0)
        tsfm.m_HalfSize.y = ((m_BoxStyle->m_Header.bottom_height + m_BoxStyle->m_Header.top_height) * grid.m_fScale + texth) / 2.f;
    else
        tsfm.m_HalfSize.y = size.y / 2.f;

    SDL_Log("text size: %d %d", textw, texth);
    float w = 0, h = 0;
    SDL_GetTextureSize(m_BoxStyle->m_Texture.get(), &w, &h);
    SDL_Log("bg size: %.2f %.2f, bg srcSize: %.2f %.2f", tsfm.m_HalfSize.x * 2, tsfm.m_HalfSize.y * 2, w, h);
}

void Message::SetAlignment(Alignment alignment)
{
    if (!m_BoxStyle) {
        SDL_Log("WARNING: setting alignment on a text without a box");
        return;
    }

    registry& reg = utility::Registry::GetInstance().GetRegistry();
    const auto& grid = reg.get<app::comp::TextureGrid>(m_Message);
    const auto& tsfm = reg.get<game::comp::Transform>(m_Message);
    int textw = 0, texth = 0;
    TTF_GetTextSize(reg.get<app::comp::Text>(m_Message).m_Text.get(), &textw, &texth);

    switch (alignment) {
    case Alignment::LEFT:
        SetOffset(-(tsfm.m_HalfSize.x - m_BoxStyle->m_Header.left_width * grid.m_fScale - textw / 2.f), 0.f);
        break;
    case Alignment::CENTER:
        SetOffset(0.f, 0.f);
        break;
    case Alignment::RIGHT:
        SetOffset(tsfm.m_HalfSize.x - m_BoxStyle->m_Header.right_width * grid.m_fScale - textw / 2.f, 0.f);
        break;
    default:
        SDL_Log("WARNING: invalid alignment");
        break;
    }
}
