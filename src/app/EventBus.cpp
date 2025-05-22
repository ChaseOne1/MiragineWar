#include "EventBus.hpp"

using namespace app;

void EventBus::Tick(SDL_Event* event)
{
    m_pEvent = event;

    m_Topics.Publish(event->type);
}

void EventBus::CleanUp()
{
    m_pEvent = nullptr;
    m_Topics.CleanUp();
}

SDL_Event* EventBus::GetEvent() const
{
    return m_pEvent;
}
