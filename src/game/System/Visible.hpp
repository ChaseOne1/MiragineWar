#pragma once

namespace game::sys {
class Visible : public utility::Singleton<Visible>
{
    friend class utility::Singleton<Visible>;

private:
    Visible();
    ~Visible() = default;

    void OnUIElementConstruct(entt::registry&, entt::entity);

public:
    void Tick();
    void CleanUp();
};
}
