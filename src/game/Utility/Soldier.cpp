#include "Soldier.hpp"
#include "app/Resources.hpp"
#include "app/resources/AllInOneIndex.hpp"
#include "app/Component/Render/Texture.hpp"
#include "app/Component/Render/ZIndex.hpp"
#include "app/Component/Render/RenderCallback.hpp"
#include "app/System/Time.hpp"
#include "game/Component/Logic.hpp"
#include "game/Component/Movement.hpp"
#include "game/Component/Transform.hpp"
#include "game/System/Visible.hpp"
#include "game/World.hpp"
#include "app/Settings.hpp"

using namespace game::util;
using namespace mathfu;

Soldier::Soldier(app::GUID_t texture, mathfu::vec2 position, app::AnimSeqFrames::ANIM animation, bool mirrored)
    : m_Asf(app::Resources::GetInstance().Require<app::AnimSeqFrames>(texture, app::idx::SOLDIERS_IDX))
    , m_Anim(animation)
    , m_Mirrored(mirrored)
{
    auto& settings = app::Settings::GetInstance().GetSettings();

    // Animation
    SetAnimation(m_Anim);

    // Transform
    SetPosition(position.x, position.y);
    SetSizeRatio(settings["Soldier"]["size_ratio"]);

    // ZIndex
    const uint16_t ZPrecision = settings["Soldier"]["zprecision"];
    SetZIndex(app::comp::ZIndexVal::ZINDEX_SOLDIER + position.y / game::World::GetInstance().msc_fHeight * ZPrecision);
}

Soldier::~Soldier()
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();
    if (reg.valid(m_Soldier)) reg.destroy(m_Soldier);
}

void Soldier::SetAnimation(app::AnimSeqFrames::ANIM animation)
{
    auto& registry = utility::Registry::GetInstance().GetRegistry();

    m_Anim = animation;
    registry.emplace_or_replace<app::comp::Texture>(m_Soldier, m_Asf->m_Frames,
        SDL_FRect { 0.f, static_cast<float>(m_Asf->GetGlobalCoordY(animation)),
            m_Mirrored ? -static_cast<float>(m_Asf->m_Info[animation].width) : static_cast<float>(m_Asf->m_Info[animation].width),
            static_cast<float>(m_Asf->m_Info[animation].height) });

    const float size_ratio = app::Settings::GetSettings()["Soldier"]["size_ratio"];
    SetSizeRatio(size_ratio);

    registry.emplace_or_replace<app::comp::PreRender>(m_Soldier,
        [soldier = m_Soldier, asf = m_Asf, start_time = app::sys::Time::GetInstance().Now(), animation]() {
            const uint16_t passed_frame
                = std::chrono::duration_cast<std::chrono::milliseconds>(app::sys::Time::GetInstance().Now() - start_time).count()
                * asf->m_Info[animation].speed;
            utility::Registry::GetInstance().GetRegistry().get<app::comp::Texture>(soldier).m_SrcFRect->x
                = asf->m_Info[animation].GetCoordX(passed_frame);
        });
}

void Soldier::SetZIndex(uint16_t zindex)
{
    auto& registry = utility::Registry::GetInstance().GetRegistry();
    registry.emplace_or_replace<app::comp::ZIndex>(m_Soldier, app::comp::ZIndex{zindex});
}

void Soldier::SetPosition(float x, float y)
{
    auto& registry = utility::Registry::GetInstance().GetRegistry();
    vec2& posn = registry.get_or_emplace<game::comp::Transform>(m_Soldier).m_Position;
    const vec2 last_posn = posn;
    posn = vec2 { x, y };
    game::sys::Visible::GetInstance().OnPositionUpdate(m_Soldier, last_posn);
}

void Soldier::SetSize(float x, float y)
{
    auto& registry = utility::Registry::GetInstance().GetRegistry();
    registry.get_or_emplace<game::comp::Transform>(m_Soldier).m_HalfSize = vec2 { x / 2.f, y / 2.f };
    game::sys::Visible::GetInstance().OnSizeUpdate(m_Soldier);
}

void Soldier::SetSizeRatio(float size_ratio)
{
    auto& registry = utility::Registry::GetInstance().GetRegistry();
    registry.get_or_emplace<game::comp::Transform>(m_Soldier).m_HalfSize
        = vec2 { m_Asf->m_Info[m_Anim].width * size_ratio / 2.f, m_Asf->m_Info[m_Anim].height * size_ratio / 2.f };
    game::sys::Visible::GetInstance().OnSizeUpdate(m_Soldier);
}

void Soldier::SetMovement(mathfu::vec2 velocity, mathfu::vec2 acceleration)
{
    auto& registry = utility::Registry::GetInstance().GetRegistry();
    registry.emplace_or_replace<game::comp::Movement>(m_Soldier, std::move(velocity), std::move(acceleration));
}

void Soldier::SetLogic(std::function<void()> logic)
{
    auto& registry = utility::Registry::GetInstance().GetRegistry();
    registry.emplace_or_replace<game::comp::Logic>(m_Soldier, std::move(logic));
}
