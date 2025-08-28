#pragma once
#include "app/TextEngine.hpp"

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

    Text(const Text& other)
        : m_Text(TTF_CreateText(app::TextEngine::GetInstance().GetRendererTextEngine(), TTF_GetTextFont(other.m_Text.get()), other.m_Text->text, 0u),
              [](TTF_Text* text) { TTF_DestroyText(text); })
        , m_Offset(other.m_Offset)
    { }

    Text(Text&& other) = default;

    ~Text() = default;

    Text& operator=(const Text& other)
    {
        if (this == &other) return *this;

        m_Text.reset(TTF_CreateText(app::TextEngine::GetInstance().GetRendererTextEngine(), TTF_GetTextFont(other.m_Text.get()), other.m_Text->text, 0u));
        m_Offset = other.m_Offset;

        return *this;
    }

    Text& operator=(Text&& other) = default;
};
}
