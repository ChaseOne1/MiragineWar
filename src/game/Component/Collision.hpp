#pragma once

namespace game::comp {
struct Collision
{
    using Callback_t = std::function<void()>;

    // center_x, center_y, half_w, half_h
    mathfu::vec4 m_CollisionBox { mathfu::kZeros4f };
    Callback_t m_fnOnCollide;

public:
    mathfu::vec4 Transform2World(const mathfu::vec2& posn) const
    {
        return { posn.xy() + m_CollisionBox.xy(), m_CollisionBox.zw() };
    }

   // std::array<mathfu::vec3, 4> Transform2WorldVertices(const Position& posn) const
   // {
   //     mathfu::vec4 coll_inWorld = Transform2World(posn);

   //     return {
   //         mathfu::vec3 { coll_inWorld.xy() - coll_inWorld.zw(), 1.f },
   //         mathfu::vec3 { coll_inWorld.x + coll_inWorld.z, coll_inWorld.y - coll_inWorld.w, 1.f },
   //         mathfu::vec3 { coll_inWorld.x - coll_inWorld.z, coll_inWorld.y + coll_inWorld.w, 1.f },
   //         mathfu::vec3 { coll_inWorld.xy() + coll_inWorld.zw(), 1.f }
   //     };
   // }
};
}
