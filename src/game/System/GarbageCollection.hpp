#pragma once

namespace game::sys {
class GarbageCollection : public utility::Singleton<GarbageCollection>
{
    friend class utility::Singleton<GarbageCollection>;
private:
    GarbageCollection() = default;
    ~GarbageCollection() = default;
public:
    void Tick();
};
}
