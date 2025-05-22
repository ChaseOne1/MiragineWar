#pragma once

namespace game::sys {
class Trigger : public utility::Singleton<Trigger>
{
    friend class utility::Singleton<Trigger>;

private:
    Trigger() = default;
    ~Trigger() = default;

public:
    void Tick();
};
}
