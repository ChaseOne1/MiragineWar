#pragma once
#include "app/NetPacket.hpp"
#include "game/Utility/Soldier.hpp"
#include "utility/Singleton.hpp"

namespace game::logic {
class MainMenuScene : public utility::Singleton<MainMenuScene>
{
    friend class utility::Singleton<MainMenuScene>;

private:
    entt::entity m_Title = utility::Registry::GetInstance().GetRegistry().create();
    std::vector<game::util::Soldier> m_Soldiers;

private:
    MainMenuScene();
    ~MainMenuScene() = default;

    void SetupWorldScene();
    void SetupTitle();
    void SetupSoldiers();
    void SetupMenu(const app::InboundPacket&);
};
}
