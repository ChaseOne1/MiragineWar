#pragma once
#include "Renderer.hpp"

namespace app {
class TextEngine : public utility::Singleton<TextEngine>
{
    friend class utility::Singleton<TextEngine>;

private:
    std::unique_ptr<TTF_TextEngine, void (*)(TTF_TextEngine*)> m_RendererTextEngine;

private:
    TextEngine()
        : m_RendererTextEngine(TTF_CreateRendererTextEngine(Renderer::GetInstance().GetSDLRenderer()),
              [](TTF_TextEngine* engine) { TTF_DestroyRendererTextEngine(engine); })
    { }

    ~TextEngine() = default;

public:
    static TTF_TextEngine* GetRendererTextEngine() noexcept { return GetInstance().m_RendererTextEngine.get(); }
};
}
