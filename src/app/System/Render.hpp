#pragma once

namespace app::sys {
class Render : public utility::Singleton<Render>
{
    friend class utility::Singleton<Render>;

private:
    Render();
    ~Render();

public:
    void Tick();
};
}
