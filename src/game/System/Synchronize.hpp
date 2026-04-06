#pragma once
#include "app/System/Network.hpp"

namespace game::sys {
class Synchronize : public utility::Singleton<Synchronize>
{
    friend class utility::Singleton<Synchronize>;

private:
    std::optional<app::InboundPacket> m_Snapshot;
    std::unordered_map<entt::id_type, entt::entity> m_EntityMap;

private:
    Synchronize();
    ~Synchronize() = default;

    void OnSynchronizeConstruct(entt::registry&, entt::entity);
    void OnSynchronizeDestroy(entt::registry&, entt::entity);

    void ProcessSnapshot();
    void Interpolate();

public:
    void Tick();
};
};
