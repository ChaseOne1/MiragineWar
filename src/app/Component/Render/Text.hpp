#pragma once
#include "app/TextEngine.hpp"

namespace app::comp {
struct Text
{
    std::unique_ptr<TTF_Text, void (*)(TTF_Text*)> m_Text;
    std::shared_ptr<void> m_Font; // TTF_Font
    SDL_FPoint m_Offset {};

public:
    Text(std::string_view text, const std::shared_ptr<TTF_Font>& font)
        : m_Text(TTF_CreateText(app::TextEngine::GetInstance().GetRendererTextEngine(), font.get(), text.data(), text.length()),
              [](TTF_Text* text) { TTF_DestroyText(text); })
        , m_Font(font)
    { }

    Text(std::string_view text, const std::shared_ptr<void>& font)
        : m_Text(TTF_CreateText(app::TextEngine::GetInstance().GetRendererTextEngine(), (TTF_Font*)font.get(), text.data(), text.length()),
              [](TTF_Text* text) { TTF_DestroyText(text); })
        , m_Font(font)
    { }

    Text(const Text& other)
        : m_Text(TTF_CreateText(app::TextEngine::GetInstance().GetRendererTextEngine(), TTF_GetTextFont(other.m_Text.get()), other.m_Text->text, 0u),
              [](TTF_Text* text) { TTF_DestroyText(text); })
        , m_Font(other.m_Font)
        , m_Offset(other.m_Offset)
    { }

    Text(Text&& other) = default;

    ~Text() = default;

    Text& operator=(const Text& other)
    {
        if (this == &other) return *this;

        m_Text.reset(TTF_CreateText(app::TextEngine::GetInstance().GetRendererTextEngine(), TTF_GetTextFont(other.m_Text.get()), other.m_Text->text, 0u));
        m_Font = other.m_Font;
        m_Offset = other.m_Offset;

        return *this;
    }

    Text& operator=(Text&& other) = default;
};
}
#include "mirrow/srefl/srefl_begin.hpp"
srefl_class(app::comp::Text,
    ctors(
        ctor(std::string_view, const std::shared_ptr<void>&)
    )
    fields(
        field(&app::comp::Text::m_Font),
        field(&app::comp::Text::m_Offset)
    )
)
#include "mirrow/srefl/srefl_end.hpp"