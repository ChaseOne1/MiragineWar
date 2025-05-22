#pragma once

namespace game::sys {
class Logic final : public utility::Singleton<Logic>
{
    friend class utility::Singleton<Logic>;

private:
    Logic() = default;
    ~Logic() = default;

public:
    void Tick();
};
}
