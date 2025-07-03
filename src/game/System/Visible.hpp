#pragma once

namespace game::sys {
class Visible : public utility::Singleton<Visible>
{
    friend class utility::Singleton<Visible>;

private:
    Visible() = default;
    ~Visible() = default;

public:
    void Tick();
    void CleanUp();
};
}
