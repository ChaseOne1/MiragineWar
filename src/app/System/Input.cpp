#include "Input.hpp"
#include "app/EventBus.hpp"

using app::Key;
using namespace app::sys;

#define DEFAULT_SETTINGS_PATH "Assets/keymaps.bin"
#define BUTTON_BASE SDL_Scancode::SDL_SCANCODE_COUNT
#define BUTTON2SCODE(x) (x + BUTTON_BASE - 1)
#define SCODE2BUTTON(x) (x - BUTTON_BASE + 1)

Input::Input()
{
    std::ifstream file(DEFAULT_SETTINGS_PATH, std::ios::in | std::ios::binary);
    if (file.bad()) throw std::invalid_argument("The game file is incomplete.");
    file.read(reinterpret_cast<char*>(m_Key2Scancode), sizeof m_Key2Scancode);
    for (uint8_t i = 0; i < static_cast<uint8_t>(Key::KEY_COUNT); ++i)
        m_Scancode2Key[m_Key2Scancode[i] >= BUTTON_BASE ? SCODE2BUTTON(m_Key2Scancode[i]) : m_Key2Scancode[i]] = static_cast<Key>(i);

    EventBus& eventbus = app::EventBus::GetInstance();

    eventbus.Subscribe(SDL_EVENT_MOUSE_BUTTON_DOWN, [this](const SDL_Event* event) {
        m_ScancodeStatusMap[BUTTON2SCODE(event->button.button)] = true;
        m_Topics.Publish(static_cast<Key>(BUTTON2SCODE(event->button.button)), event); });

    eventbus.Subscribe(SDL_EVENT_MOUSE_BUTTON_UP, [this](const SDL_Event* event) {
        m_ScancodeStatusMap[BUTTON2SCODE(event->button.button)] = false;
        m_Topics.Publish(static_cast<Key>(BUTTON2SCODE(event->button.button)), event); });

    eventbus.Subscribe(SDL_EVENT_KEY_DOWN, [this](const SDL_Event* event) {
        m_ScancodeStatusMap[event->key.scancode] = true;
        m_Topics.Publish(m_Scancode2Key[event->key.scancode],event); });

    eventbus.Subscribe(SDL_EVENT_KEY_UP, [this](const SDL_Event* event) {
        m_ScancodeStatusMap[event->key.scancode] = false;
        m_Topics.Publish(m_Scancode2Key[event->key.scancode],event); });

    m_Topics.CleanUp();
}

Input::~Input()
{
    // TODO: maybe unnecessary sometimes
    std::ofstream file(DEFAULT_SETTINGS_PATH, std::ios::out | std::ios::binary | std::ios::trunc);
    file.write(reinterpret_cast<char*>(m_Key2Scancode), sizeof m_Key2Scancode);
}

bool Input::IsPressed(Key key) const noexcept
{
    return m_ScancodeStatusMap.test(m_Key2Scancode[std::size_t(key)]);
}
