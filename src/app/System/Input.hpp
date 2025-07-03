#pragma once
#include "FunctionalKeyDef.hpp"
#include "utility/Topics.hpp"

namespace app::sys {
class Input : public utility::Singleton<Input>
{
    friend class utility::Singleton<Input>;

private:
#define BUTTON_COUNT (sizeof(decltype(SDL_MouseButtonEvent::button)) * CHAR_BIT)
    std::bitset<SDL_Scancode::SDL_SCANCODE_COUNT + 1 + BUTTON_COUNT> m_ScancodeStatusMap;
#undef BUTTON_COUNT
    SDL_Scancode m_Key2Scancode[std::size_t(Key::KEY_COUNT)] {};
    Key m_Scancode2Key[SDL_Scancode::SDL_SCANCODE_COUNT] {};

    utility::Topics<Key> m_Topics;

private:
    Input();
    ~Input();

public:
    void Tick();

    template <typename F>
    void Subsrcibe(Key topic, F&& callback)
    {
        m_Topics.Subscribe(topic, callback);
    }

    template <typename F>
    void Unsubscribe(Key topic, F* callback)
    {
        m_Topics.Unsubscribe(topic, callback);
    }

    // TODO: call this
    void CleanUp()
    {
        m_Topics.CleanUp();
    }

    bool IsPressed(Key key) const noexcept;

    // TODO: support the remapping
};
}
