#pragma once
#include "SDL3_ttf/SDL_ttf.h"
#include "app/Layout.hpp"
#include "app/TextEngine.hpp"
#include "app/ScriptComponent.hpp"

namespace app::comp {
struct Text : app::ScriptComponent<Text>
{
    std::shared_ptr<TTF_Font> m_LogicalFont, m_RenderedFont;
    std::unique_ptr<TTF_Text, void (*)(TTF_Text*)> m_Text;
    SDL_FPoint m_Offset {}; // scaled, used for alignment

public:
    Text(std::string_view text, const std::shared_ptr<TTF_Font>& font, float font_size = 0.f)
        : m_LogicalFont(TTF_CopyFont(font.get()), [](void*) { })
        , m_RenderedFont(TTF_CopyFont(font.get()), [](void*) { })
        , m_Text(nullptr, nullptr)
    {
        if (font_size) TTF_SetFontSize(m_LogicalFont.get(), font_size);
        TTF_SetFontSize(m_RenderedFont.get(), TTF_GetFontSize(m_LogicalFont.get()) * app::Layout::GetScale().y);
        m_Text = decltype(m_Text) { TTF_CreateText(app::TextEngine::GetInstance().GetRendererTextEngine(), m_RenderedFont.get(), text.data(), text.length()),
            [](TTF_Text* text) { TTF_DestroyText(text); } };
    }

    Text(std::string_view text, const std::shared_ptr<void>& font, sol::variadic_args font_size)
        : m_LogicalFont(TTF_CopyFont((TTF_Font*)font.get()), [](void*) { })
        , m_RenderedFont(TTF_CopyFont((TTF_Font*)font.get()), [](void*) { })
        , m_Text(nullptr, nullptr)
    {
        if (auto sz = font_size.get<sol::optional<float>>(); sz.value_or(0.f)) TTF_SetFontSize(m_LogicalFont.get(), sz.value());
        TTF_SetFontSize(m_RenderedFont.get(), TTF_GetFontSize(m_LogicalFont.get()) * app::Layout::GetScale().y);
        m_Text = decltype(m_Text) { TTF_CreateText(app::TextEngine::GetInstance().GetRendererTextEngine(), m_RenderedFont.get(), text.data(), text.length()),
            [](TTF_Text* text) { TTF_DestroyText(text); } };
    }

    void SetFontWrapAlignment(TTF_HorizontalAlignment alignment)
    {
        TTF_SetFontWrapAlignment(m_LogicalFont.get(), alignment);
        TTF_SetFontWrapAlignment(m_RenderedFont.get(), alignment);
    }

    float GetLogicalFontSize()
    {
        return TTF_GetFontSize(m_LogicalFont.get());
    }

    float GetRenderedFontSize()
    {
        return TTF_GetFontSize(m_RenderedFont.get());
    }

    std::tuple<int, int> GetLogicalTextSize()
    {
        std::tuple<int, int> size {};
        TTF_GetStringSizeWrapped(m_LogicalFont.get(), m_Text->text, 0, 0, &std::get<0>(size), &std::get<1>(size));
        return size;
    }

    std::tuple<int, int> GetRenderedTextSize()
    {
        std::tuple<int, int> size {};
        TTF_GetTextSize(m_Text.get(), &std::get<0>(size), &std::get<1>(size));
        return size;
    }

    void SetTextColor(sol::stack_table color)
    {
        TTF_SetTextColor(m_Text.get(),
            color["r"].get<Uint8>(), color["g"].get<Uint8>(), color["b"].get<Uint8>(), color["a"].get_or(255u));
    }

    void SetTextWrapWhitespaceVisible(bool visible)
    {
        TTF_SetTextWrapWhitespaceVisible(m_Text.get(), visible);
    }

    Text(const Text& other) = delete;

    Text(Text&& other) = delete;

    ~Text() = default;

    Text& operator=(const Text& other) = delete;

    Text& operator=(Text&& other) = default;
};
}
#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off
srefl_class(app::comp::Text,
    ctors(
        ctor(std::string_view, const std::shared_ptr<void>&, sol::variadic_args)
    )
    fields(
        field(&app::comp::Text::m_Offset),

        field(&app::comp::Text::SetFontWrapAlignment),
        field(&app::comp::Text::GetLogicalFontSize),
        field(&app::comp::Text::GetRenderedFontSize),
        field(&app::comp::Text::GetLogicalTextSize),
        field(&app::comp::Text::GetRenderedTextSize),
        field(&app::comp::Text::SetTextColor),
        field(&app::comp::Text::SetTextWrapWhitespaceVisible)
    )
)
// clang-format on
#include "mirrow/srefl/srefl_end.hpp"
