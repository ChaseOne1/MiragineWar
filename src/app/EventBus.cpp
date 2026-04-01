#include "EventBus.hpp"

using namespace app;

EventBus::EventBus()
{ }

void EventBus::Tick(SDL_Event* event)
{
    m_pEvent = event;

    m_Topics.Publish(event->type, event);

    m_pEvent = nullptr;
}
