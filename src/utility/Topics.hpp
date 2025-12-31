#pragma once

namespace utility {
using TopicsSubscriberID = std::size_t;
constexpr TopicsSubscriberID TopicsSubscriberIDNull = 0u;

template <typename Topic, typename Callback = std::function<void()>>
class Topics
{
    using SubscriberID = std::size_t;

private:
    struct Subscriber
    {
        Callback m_Callback;
        SubscriberID m_ID;
    };

private:
    std::unordered_map<Topic, std::vector<Subscriber>> m_Topics;
    SubscriberID m_CurrentID = 0u;

public:
    template <typename... Args>
    void Publish(Topic topic, Args&&... args)
    {
        auto subscribers = m_Topics.find(topic);
        if (subscribers == m_Topics.end()) return;

        for (auto iter = subscribers->second.begin(); iter != subscribers->second.end();) {
            const std::size_t dis1 = std::distance(iter, subscribers->second.begin());
            iter->m_Callback(std::forward<Args>(args)...);
            const std::size_t dis2 = std::distance(iter, subscribers->second.begin());
            if (dis1 == dis2) ++iter;
        }
    }

    SubscriberID GetNextSubscriberID() const noexcept { return m_CurrentID + 1; }

    template <typename F, typename = std::enable_if_t<std::is_constructible_v<Callback, F>>>
    SubscriberID Subscribe(Topic topic, F&& callback)
    {
        auto& subscribers = m_Topics[topic];
        subscribers.emplace_back(Subscriber{ std::forward<F>(callback), ++m_CurrentID });
        return m_CurrentID;
    }

    // NOTE: DO NOT unsubscribe that are not currently in use in callback
    void Unsubscribe(Topic topic, SubscriberID id)
    {
        // TODO: optimize deletion performance
        auto& subscribers = m_Topics.at(topic);
        subscribers.erase(
            std::remove_if(subscribers.begin(), subscribers.end(),
                [&](const Subscriber& sub) { return sub.m_ID == id; }),
            subscribers.end());
    }

    void CleanUp()
    {
        for (auto iter = m_Topics.begin(); iter != m_Topics.end();) {
            if (iter->second.empty()) iter = m_Topics.erase(iter);
            else ++iter;
        }
    }

    std::size_t GetSubscriberCount(const Topic& topic) const noexcept
    {
        if (auto iter = m_Topics.find(topic); iter != m_Topics.end()) return iter->second.size();
        else return 0u;
    }

    decltype(auto) cbegin() const noexcept { return m_Topics.cbegin(); }
    decltype(auto) cend() const noexcept { return m_Topics.cend(); }
};
}