#pragma once

namespace app::comp {
namespace detail {
    using RenderCallback_t = std::function<void()>;
}

struct PreRender
{
    using RenderCallback_t = detail::RenderCallback_t;
    RenderCallback_t m_fnPreRender;
};

struct PostRender
{
    using RenderCallback_t = detail::RenderCallback_t;
    RenderCallback_t m_fnPostRender;
};
}
