#pragma once

namespace game::sys {
class Scene : public utility::Singleton<Scene>
{
    friend class utility::Singleton<Scene>;

private:
    Scene();
    ~Scene() = default;

public:
    void Tick();
};
}
