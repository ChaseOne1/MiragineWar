#pragma once

namespace game::logic {
class MainMenuScene : public utility::Singleton<MainMenuScene>
{
    friend class utility::Singleton<MainMenuScene>;

private:
    entt::entity m_Title = utility::Registry::GetInstance().GetRegistry().create();
    std::vector<entt::entity> m_Soldiers;

private:
    MainMenuScene();
    ~MainMenuScene() = default;

    void SetupWorldScene();
    void SetupTitle();
    void SetupSoldiers();

public:
};
}
