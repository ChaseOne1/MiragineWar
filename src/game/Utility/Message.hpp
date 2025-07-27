#pragma once
#include "app/resources/AllInOneRes.hpp"

namespace app {
struct Mox;
}

namespace game::util {
class Message
{
    const entt::entity m_Message = utility::Registry::GetInstance().GetRegistry().create();
    std::shared_ptr<TTF_Font> m_Font;
    std::shared_ptr<app::Mox> m_BoxStyle;

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
    Message(std::string_view msg, app::GUID_t font = app::res::SUBTITLE_TTF, bool isUIElement = true);
    ~Message();

    void SetString(std::string_view msg);
    void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255u);
    void SetPosition(float x, float y);
    void SetOffset(float x, float y);
    void SetMovement(mathfu::vec2 velocity, mathfu::vec2 accleration = mathfu::kZeros2f);
    void SetBox(app::GUID_t box, mathfu::vec2i size = mathfu::kZeros2i, float scale = 0.f);
    void SetAlignment(Alignment alignment);
};
}
