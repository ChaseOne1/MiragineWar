#pragma once

#include <functional>
#include <vector>
namespace game {
namespace comp {
    class State {
    public:
        //using Skey = uint8_t;

    private:
        //Skey state;

    public:
        std::vector<std::function<bool()>> m_vecTransitions;
        //std::unordered_map<std::function<bool()>, Skey> m_mapTransitions;
        //std::unordered_map<Skey, std::vector<std::function<bool()>>> m_mapSensitives;
    };
}
}
