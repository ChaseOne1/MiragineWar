#pragma once

namespace utility {
class Topic
{
    using Callback_t = std::function<void()>;
    struct Subscriber
    {
        Callback_t callback;
        const void* subscriber;
    };

private:
    std::vector<Subscriber> m_Subscribers;

public:
    void Publish()
    {
        for (auto& subscriber : m_Subscribers) {
            subscriber.callback();
        }
    }

    template <typename F>
    std::remove_reference_t<F>* Subscribe(F&& callback)
    {
        return m_Subscribers.emplace_back(Subscriber { std::forward<F>(callback) })
            .callback.template target<std::remove_reference_t<F>>();
    }

    // NOTE: DO NOT unsubscribe in callback
    template <typename F>
    void Unsubscribe(Topic topic, F* callback)
    {
        // TODO: optimize deletion performance
        m_Subscribers.erase(
            std::remove_if(m_Subscribers.begin(), m_Subscribers.end(),
                [&](const Subscriber& sub) {
                    return callback == sub.callback.template target<F>();
                }),
            m_Subscribers.end());
    }
};
}
