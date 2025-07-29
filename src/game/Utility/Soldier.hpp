#pragma once
#include "app/resources/AnimSeqFrames.hpp"

namespace game::util {
class Soldier
{
    entt::entity m_Soldier = utility::Registry::GetInstance().GetRegistry().create();
    std::shared_ptr<app::AnimSeqFrames> m_Asf;
    app::AnimSeqFrames::ANIM m_Anim;
    bool m_Mirrored;

public:
    Soldier(app::GUID_t texture, mathfu::vec2 position,
        app::AnimSeqFrames::ANIM animation = app::AnimSeqFrames::ANIM_IDLE, bool mirrored = false);
    ~Soldier();

    entt::entity GetEntity() const noexcept { return m_Soldier; }

    void SetAnimation(app::AnimSeqFrames::ANIM animation);
    void SetZIndex(uint16_t zindex);
    void SetPosition(float x, float y);
    void SetSize(float x, float y);
    void SetSizeRatio(float size_ratio);
    void SetMovement(mathfu::vec2 velocity, mathfu::vec2 acceleration = mathfu::kZeros2f);
    void SetLogic(std::function<void()> logic);
};
}
