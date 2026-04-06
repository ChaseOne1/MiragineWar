#include "Synchronize.hpp"
#include "MWProtocol/MessageIdentifiers.hh"
#include "MWProtocol/PositionSyncPacket.hh"
#include "app/Settings.hpp"
#include "game/Component/Movement.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/Synchronize.hpp"
#include "utility/Registry.hpp"

using game::sys::Synchronize;

Synchronize::Synchronize()
{
    auto& network = app::sys::Network::GetInstance();
    network.Subscribe(proto::ID_POSITION_SYNC,
        [](const app::InboundPacket& packet) { GetInstance().m_Snapshot = packet; });

    utility::Registry::GetRegistry().on_construct<game::comp::Synchronize>().connect<&Synchronize::OnSynchronizeConstruct>(this);
    utility::Registry::GetRegistry().on_destroy<game::comp::Synchronize>().connect<&Synchronize::OnSynchronizeDestroy>(this);
}

void Synchronize::OnSynchronizeConstruct(entt::registry& registry, entt::entity entity)
{
    m_EntityMap.emplace(registry.get<game::comp::Synchronize>(entity).m_RemoteID, entity);
}

void Synchronize::OnSynchronizeDestroy(entt::registry& registry, entt::entity entity)
{
    m_EntityMap.erase(registry.get<game::comp::Synchronize>(entity).m_RemoteID);
}

void Synchronize::ProcessSnapshot()
{
    // TODO: specify bytes for each part
    // ID_TIMESTAMP + TIMESTAMP + ID_POSITION_SYNC + EntityID + Flags + Position(x, y) [+ Velocity(x, y)]
    if (!m_Snapshot) return;
    auto& registry = utility::Registry::GetRegistry();

    using namespace proto;

    SLNet::BitStream bitstream(m_Snapshot->m_Packet->data, m_Snapshot->m_Packet->length, false);
    bitstream.IgnoreBytes(
        sizeof(PositionSyncPacketHeader::m_id_timestamp)); // ID_TIMESTAMP
    SLNet::Time last; bitstream.Read(last); // timestamp
    bitstream.IgnoreBytes(
        sizeof(PositionSyncPacketHeader::m_id_positon_sync)); // ID_POSITION_SYNC
    while (true) {
        // entity id
        entt::id_type id = entt::null; 
        if(!bitstream.SerializeBitsFromIntegerRange(false, id, REMOTE_ID_MIN, REMOTE_ID_MAX)) break;
        entt::entity entity = entt::null;
        if (const auto iter = m_EntityMap.find(id); iter == m_EntityMap.end()) {
            LOGW("unknown remote entity {}", id);
            continue;
        } else entity = iter->second;

        // flags
        PositionSyncPacketFlags flags { 0 };
        bitstream.Read(*reinterpret_cast<PositionSyncPacketFlags_t*>(&flags));
        if (!registry.valid(entity)) {
            bitstream.IgnoreBytes(sizeof(PositionSyncPacketPos));
            if (flags.velocity) bitstream.IgnoreBytes(sizeof(PositionSyncPacketVel));
            continue;
        }

        game::comp::Synchronize& sync = registry.get<game::comp::Synchronize>(entity);

        // position
        mathfu::vec2 auth_posn;
        bitstream.SerializeFloat16(false, auth_posn.x, POSITION_X_MIN, POSITION_X_MAX);
        bitstream.SerializeFloat16(false, auth_posn.y, POSITION_Y_MIN, POSITION_Y_MAX);

        // velocity
        mathfu::vec2 auth_vel = sync.m_Velocity;
        if (flags.velocity) {
            bitstream.SerializeFloat16(false, auth_vel.x, VELOCITY_X_MIN, VELOCITY_X_MAX);
            bitstream.SerializeFloat16(false, auth_vel.y, VELOCITY_Y_MIN, VELOCITY_Y_MAX);
            sync.m_Velocity = auth_vel;
        }

        // predict to compensate for delays
        sync.m_Position = auth_posn + auth_vel * ((SLNet::GetTime() - last) / 1000.f);
    }

    m_Snapshot = std::nullopt;
}

void Synchronize::Interpolate()
{
    using game::comp::Synchronize, game::comp::Transform, game::comp::Movement;
    auto view = utility::Registry::GetRegistry().view<Synchronize, Transform, Movement>();
    for (auto entity : view) {
        Synchronize& sync = view.get<Synchronize>(entity);
        Transform& tsfm = view.get<Transform>(entity);
        Movement& mvmt = view.get<Movement>(entity);

        const mathfu::vec2 direction = sync.m_Position - tsfm.m_Position;

        // TODO: magic number
        if (direction.Length() < 0.1f) {
            mvmt.m_Acceleration = mathfu::kZeros2f;
            mvmt.m_Velocity = mathfu::kZeros2f;
            continue;
        }

        const auto& settings = app::Settings::GetSystem().get<sol::table>("Soldier");
        mvmt.m_Velocity = direction * settings.get<float>("approaching_speed");
    }
}

void Synchronize::Tick()
{
    ProcessSnapshot();
    Interpolate();
}
