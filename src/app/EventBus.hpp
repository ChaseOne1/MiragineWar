#pragma once
#include "utility/Topics.hpp"
#include "app/ScriptModule.hpp"

namespace app {
class EventBus : public utility::Singleton<EventBus>, public app::ScriptModule<EventBus>
{
    friend class utility::Singleton<EventBus>;
    friend class app::ScriptModule<EventBus>;
    using Topic = decltype(SDL_Event::type);

private:
    SDL_Event* m_pEvent = nullptr;
    utility::Topics<Topic, std::function<void(const SDL_Event*)>> m_Topics;

private:
    EventBus() = default;
    ~EventBus() = default;

    void RegisterEnv(sol::environment&);

public:
    void Tick(SDL_Event* event);

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
