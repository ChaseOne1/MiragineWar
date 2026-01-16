#include "EventBus.hpp"
#include "app/ScriptManager.hpp"

using namespace app;



EventBus::EventBus()
{
    auto type = app::ScriptManager::GetLuaState()
                    .new_usertype<EventBus>("EventBus", sol::no_constructor);
    type["Subscribe"] = EventBus::Subscribe<sol::function>;
    type["Unsubscribe"] = EventBus::Unsubscribe;
}

void EventBus::Tick(SDL_Event* event)
{
    m_pEvent = event;

    m_Topics.Publish(event->type, event);

    m_pEvent = nullptr;
}
