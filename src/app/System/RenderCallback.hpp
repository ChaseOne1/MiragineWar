#pragma once

namespace app::sys {
class PreRender : public utility::Singleton<PreRender>
{
    friend class utility::Singleton<PreRender>;

private:
    PreRender() = default;
    ~PreRender() = default;

public:
    void Tick();
};

class PostRender : public utility::Singleton<PostRender>
{
    friend class utility::Singleton<PostRender>;

private:
    PostRender() = default;
    ~PostRender() = default;

public:
    void Tick();
};
}
