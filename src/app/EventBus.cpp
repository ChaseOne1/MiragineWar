#include "EventBus.hpp"
#include "app/ScriptManager.hpp"

using namespace app;

void EventBus::RegisterEnv(sol::environment& env)
{
    env.new_usertype<EventBus>("EventBus", sol::no_constructor,
        "Subscribe", &EventBus::Subscribe<sol::function>,
        "Unsubscribe", &EventBus::Unsubscribe
    );
}

void EventBus::Tick(SDL_Event* event)
{
    m_pEvent = event;

    m_Topics.Publish(event->type, event);
    
    m_pEvent = nullptr;
    m_Topics.CleanUp();
}
