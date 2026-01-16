#pragma once
#include "utility/Topics.hpp"

namespace app {
class EventBus : public utility::Singleton<EventBus>
{
    friend class utility::Singleton<EventBus>;
    using Topic = decltype(SDL_Event::type);

private:
    SDL_Event* m_pEvent = nullptr;
    utility::Topics<Topic, std::function<void(const SDL_Event*)>> m_Topics;

private:
    EventBus();
    ~EventBus() = default;


public:
    void Tick(SDL_Event* event);

    static void RegisterToEnv();

    template <typename F>
    static auto Subscribe(Topic topic, F&& callback)
    {
        return GetInstance().m_Topics.Subscribe(topic, std::forward<F>(callback));
    }

    static void Unsubscribe(Topic topic, utility::TopicsSubscriberID id)
    {
        GetInstance().m_Topics.Unsubscribe(topic, id);
    }

    static void CleanUp() { GetInstance().m_Topics.CleanUp(); }
};
}
