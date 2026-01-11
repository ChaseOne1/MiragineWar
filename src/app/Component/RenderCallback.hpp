#pragma once
#include "app/ScriptComponent.hpp"

namespace app::comp {
namespace detail {
    using RenderCallback_t = std::function<void()>;
}

struct PreRender
{
    REGISTER_TO_ENV(PreRender)

    using RenderCallback_t = detail::RenderCallback_t;
    RenderCallback_t m_fnPreRender;
};

struct PostRender 
{
    REGISTER_TO_ENV(PreRender)

    using RenderCallback_t = detail::RenderCallback_t;
    RenderCallback_t m_fnPostRender;
};
}

#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off
srefl_class(app::comp::PreRender,
    ctors(
        ctor(app::comp::PreRender::RenderCallback_t)
    )
)

srefl_class(app::comp::PostRender,
    ctors(
        ctor(app::comp::PostRender::RenderCallback_t)
    )
)
// clang-format on
#include "mirrow/srefl/srefl_end.hpp"
