#pragma once

namespace game::sys {
class Move : public utility::Singleton<Move>
{
    friend class utility::Singleton<Move>;

private:
    Move() = default;
    ~Move() = default;

public:
    void Tick();
};
}
