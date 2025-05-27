#include "Input.hpp"

using app::Key;
using namespace app::sys;

#define DEFAULT_SETTINGS_PATH "Assets/keymaps.bin"
#define BUTTON_BASE SDL_Scancode::SDL_SCANCODE_COUNT
#define BUTTON2KEY(x) (x + BUTTON_BASE - 1)
#define KEY2BUTTON(x) (x - BUTTON_BASE + 1)

Input::Input()
{
    std::ifstream file(DEFAULT_SETTINGS_PATH, std::ios::in | std::ios::binary);
    if (file.bad()) throw std::invalid_argument("The game file is incomplete.");
    file.read(reinterpret_cast<char*>(m_KeyMaps), sizeof m_KeyMaps);

    EventBus& eventbus = app::EventBus::GetInstance();

    eventbus.Subscribe(SDL_EVENT_MOUSE_BUTTON_DOWN, [this]() {
        SDL_Event* event = app::EventBus::GetInstance().GetEvent();
        m_KeyStatusMap[BUTTON2KEY(event->button.button)] = true;
        m_Topics.Publish(static_cast<Key>(BUTTON2KEY(event->button.button))); });

    eventbus.Subscribe(SDL_EVENT_MOUSE_BUTTON_UP, [this]() {
        SDL_Event* event = app::EventBus::GetInstance().GetEvent();
        m_KeyStatusMap[BUTTON2KEY(event->button.button)] = false;
        m_Topics.Publish(static_cast<Key>(BUTTON2KEY(event->button.button))); });

    eventbus.Subscribe(SDL_EVENT_KEY_DOWN, [this]() {
        SDL_Event* event = app::EventBus::GetInstance().GetEvent();
        m_KeyStatusMap[event->key.scancode] = true;
        m_Topics.Publish(static_cast<Key>(event->key.scancode)); });

    eventbus.Subscribe(SDL_EVENT_KEY_UP, [this]() {
        SDL_Event* event = app::EventBus::GetInstance().GetEvent();
        m_KeyStatusMap[event->key.scancode] = false;
        m_Topics.Publish(static_cast<Key>(event->key.scancode)); });
}

Input::~Input()
{
    // TODO: maybe unnecessary sometimes
    std::ofstream file(DEFAULT_SETTINGS_PATH, std::ios::out | std::ios::binary | std::ios::trunc);
    file.write(reinterpret_cast<char*>(m_KeyMaps), sizeof m_KeyMaps);
}

bool Input::IsPressed(Key key) const noexcept
{
    return m_KeyStatusMap.test(m_KeyMaps[std::size_t(key)]);
}
