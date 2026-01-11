#pragma once
namespace utility {

template <typename K, typename V, typename Hash = std::hash<K>>
class LRUContainer
{
    using ElementContainer_t = std::list<std::pair<K, V>>;
    using CacheContainer_t = std::unordered_map<K, typename ElementContainer_t::iterator, Hash>;

private:
    ElementContainer_t m_Elements;
    CacheContainer_t m_Cache;
    const std::size_t m_nCapacity;

public:
    LRUContainer(std::size_t capacity)
        : m_nCapacity(capacity)
    {
        m_Cache.reserve(capacity);
    }

    // NOTE: need ensure the thread safety!!!!!!

    template <typename... Args>
    V* Put(Args&&... pair_kv) noexcept
    {
        std::pair kv(std::forward<Args>(pair_kv)...);

        if (m_Elements.size() == m_nCapacity) {
            auto e = m_Elements.back().first;
            Release(e);
        }

        auto key = kv.first;
        m_Elements.emplace_front(std::move(kv));
        m_Cache.emplace(std::move(key), m_Elements.begin());

        return &m_Elements.front().second;
    }

    V* Get(const K& key) noexcept
    {
        auto iter = m_Cache.find(key);
        if (iter == m_Cache.end())
            return nullptr;

        m_Elements.splice(m_Elements.begin(), m_Elements, iter->second);

        return &m_Elements.front().second;
    }

    V* View(const K& key) noexcept
    {
        auto iter = m_Cache.find(key);
        if (iter == m_Cache.end())
            return nullptr;

        return &iter->second->second;
    }

    void Release(const K& key)
    {
        auto e = m_Cache.find(key);
        if (e == m_Cache.end())
            throw std::out_of_range("Try release nothing");

        m_Elements.erase(m_Cache.at(key));
        m_Cache.erase(e);
    }

    void Clear()
    {
        m_Elements.clear();
    }

    decltype(auto) begin() const noexcept { return m_Elements.begin(); }
    decltype(auto) end() const noexcept { return m_Elements.end(); }
    decltype(auto) cbegin() const noexcept { return m_Elements.cbegin(); }
    decltype(auto) cend() const noexcept { return m_Elements.cend(); }
};
}
