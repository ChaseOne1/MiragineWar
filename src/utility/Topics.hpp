#pragma once

namespace utility {
template <typename Topic>
class Topics
{
    using Callback_t = std::function<void()>;
    struct Subscriber
    {
        Callback_t callback;
        const void* subscriber;
    };

private:
    std::unordered_map<Topic, std::vector<Subscriber>> m_Topics;

public:
    void Publish(Topic topic)
    {
        auto subscribers = m_Topics.find(topic);
        if (subscribers == m_Topics.end()) return;

        for (auto& subscriber : subscribers->second) {
            subscriber.callback();
        }
    }

    template <typename F>
    std::remove_reference_t<F>* Subscribe(Topic topic, F&& callback)
    {
        auto& subscribers = m_Topics[topic];
        subscribers.emplace_back(Subscriber { std::forward<F>(callback) });
        return subscribers.back().callback.template target<std::remove_reference_t<F>>();
    }

    // NOTE: DO NOT unsubscribe in callback
    template <typename F>
    void Unsubscribe(Topic topic, F* callback)
    {
        // TODO: optimize deletion performance
        auto& subscribers = m_Topics.at(topic);
        subscribers.erase(
            std::remove_if(subscribers.begin(), subscribers.end(),
                [&](const Subscriber& sub) { return callback == sub.callback.template target<F>(); }),
            subscribers.end());
    }

    void CleanUp()
    {
        for (auto iter = m_Topics.begin(); iter != m_Topics.end();) {
            if (iter->second.empty()) iter = m_Topics.erase(iter);
            else ++iter;
        }
    }
};
}
