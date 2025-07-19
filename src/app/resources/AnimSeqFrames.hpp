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
        uint16_t GetCoordX(uint16_t frame) const { return (frame % count) * (width + AnimSeqFrames::msc_Padding); }
    } m_Info[ANIM_NUM];

    std::shared_ptr<SDL_Texture> m_Frames;

public:
    uint16_t GetGlobalCoordY(ANIM anim) const
    {
        uint16_t res = 0u;

        for (uint8_t i = ANIM_IDLE; i < anim; ++i)
            res += m_Info[i].height + msc_Padding;

        return res;
    };
};
}
