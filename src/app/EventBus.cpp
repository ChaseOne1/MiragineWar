#include "EventBus.hpp"

using namespace app;

void EventBus::RegisterEnv(sol::environment& env)
{
    auto type = env.new_usertype<EventBus>("EventBus", sol::no_constructor);
    type["Subscribe"] = &EventBus::Subscribe<sol::function>;
    type["Unsubscribe"] = &EventBus::Unsubscribe;
}

void EventBus::Tick(SDL_Event* event)
{
    m_pEvent = event;

    m_Topics.Publish(event->type, event);

    m_pEvent = nullptr;
}
