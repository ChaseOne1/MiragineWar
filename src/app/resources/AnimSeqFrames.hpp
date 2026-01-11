#pragma once

/* 0         7           15
 * +-------HEADER---------+
 * |   width | height     |
 * |----------------------|
 * |   count | pending    |
 * |----------------------|
 * |       float          |
 * +----------------------+
 * |                      |
 * |     IMAGES_DATA      |
 * |   (volatile size)    |
 * |                      |
 * +----------------------+
 */

struct SDL_Texture;

namespace app {
struct AnimSeqFrames
{
    static constexpr uint8_t msc_Padding = 1u;

    enum ANIM : uint8_t
    {
        ANIM_IDLE,
        ANIM_RUN,
        ANIM_ATTACK,
        ANIM_DIE,
        ANIM_NUM,
    };

    struct AnimInfo
    {
        std::uint8_t width, height;
        std::uint8_t count, pending;
        float speed; // speed in ms

    public:
        float GetFrameX(uint16_t frame) const { return (frame % count) * (width + AnimSeqFrames::msc_Padding); }
    };
    std::array<AnimInfo, ANIM_NUM> m_Info;

    std::shared_ptr<SDL_Texture> m_Frames;

public:
    uint16_t GetGlobalCoordY(ANIM anim) const
    {
        uint16_t res = 0u;

        for (uint8_t i = ANIM_IDLE; i < anim; ++i)
            res += m_Info[i].height + msc_Padding;

        return res;
    };

    SDL_FRect GetAnimSrcRect(ANIM anim, uint16_t frame = 0u) // const noexcept
    {
        SDL_FRect src { m_Info[anim].GetFrameX(frame), 0.f, static_cast<float>(m_Info[anim].width), static_cast<float>(m_Info[anim].height) };

        for (uint8_t i = ANIM_IDLE; i < anim; ++i) {
            src.y += m_Info[i].height + msc_Padding;
        }

        return src;
    }
};
}

 #include "mirrow/srefl/srefl_begin.hpp"
 srefl_class(app::AnimSeqFrames,
     fields(
         field(&app::AnimSeqFrames::m_Info),
         field(&app::AnimSeqFrames::m_Frames),
         field(&app::AnimSeqFrames::GetAnimSrcRect)
     )
 )

srefl_class(app::AnimSeqFrames::AnimInfo,
        fields(
            field(&app::AnimSeqFrames::AnimInfo::width),
            field(&app::AnimSeqFrames::AnimInfo::height),
            field(&app::AnimSeqFrames::AnimInfo::count),
            field(&app::AnimSeqFrames::AnimInfo::pending),
            field(&app::AnimSeqFrames::AnimInfo::speed),
            field(&app::AnimSeqFrames::AnimInfo::GetFrameX)
        )
)
#include "mirrow/srefl/srefl_end.hpp"
