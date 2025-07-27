#pragma once

namespace app::comp {
struct Text
{
    std::unique_ptr<TTF_Text, void (*)(TTF_Text*)> m_Text;
    SDL_FPoint m_Offset {};

public:
    Text(std::string_view text, std::shared_ptr<TTF_Font> font)
        : m_Text(TTF_CreateText(app::TextEngine::GetInstance().GetRendererTextEngine(), font.get(), text.data(), text.length()),
              [](TTF_Text* text) { TTF_DestroyText(text); })
    { }
};
}
