#pragma once
#include "utility/Topics.hpp"

namespace app {
class EventBus : public utility::Singleton<EventBus>
{
    friend class utility::Singleton<EventBus>;
    using Topic = decltype(SDL_Event::type);

private:
    SDL_Event* m_pEvent = nullptr;
    utility::Topics<Topic> m_Topics;

private:
    EventBus() = default;
    ~EventBus() = default;

public:
    void Tick(SDL_Event* event);

    template <typename F>
    void Subscribe(Topic topic, F&& callback)
    {
        m_Topics.Subscribe(topic, std::forward<F>(callback));
    }

    template<typename F>
    void Unsubscribe(Topic topic, F* callback)
    {
        m_Topics.Unsubscribe(topic, callback);
    }

    void CleanUp();

    SDL_Event* GetEvent() const;
};
}
