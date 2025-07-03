#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <unordered_set>
#include <vector>

namespace utility {
namespace internal {
    enum Border
    {
        LEFT = 0,
        TOP,
        RIGHT,
        BOTTOM
    };
}
namespace detail {
    struct TightGrid
    {
        struct Cell
        {
            std::vector<unsigned int> m_LooseCells;
        };

    public:
        const unsigned int mc_nRows, mc_nCols;
        const float mc_fCellWidth, mc_fCellHeight;
        std::vector<Cell> m_Cells;

    public:
        TightGrid(float width, float height, float cell_width, float cell_height)
            : mc_fCellWidth(cell_width)
            , mc_fCellHeight(cell_height)
            , mc_nRows(std::ceil(height / cell_height))
            , mc_nCols(std::ceil(width / cell_width))
            , m_Cells(mc_nRows * mc_nCols)
        { }

        std::array<unsigned int, 4> Align(float l, float t, float r, float b) const noexcept
        {
            return {
                std::max(0u, static_cast<unsigned int>(l / mc_fCellWidth)),
                std::max(0u, static_cast<unsigned int>(t / mc_fCellHeight)),
                std::min(mc_nCols, static_cast<unsigned int>(r / mc_fCellWidth)),
                std::min(mc_nRows, static_cast<unsigned int>(b / mc_fCellHeight)),
            };
        }

        unsigned int AlignX(float val) const noexcept
        {
            return std::max(0u, static_cast<unsigned int>(val / mc_fCellWidth));
        }

        unsigned int AlignY(float val) const noexcept
        {
            return std::min(mc_nCols, static_cast<unsigned int>(val / mc_fCellHeight));
        }

        unsigned int index(unsigned int x, unsigned int y) const noexcept
        {
            return y * mc_nCols + x;
        }
    };

    template <typename T>
    struct LooseGrid
    {
        struct Cell
        {
            std::vector<T> m_Elements;
            float m_MBR[4] { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() };
        };

    public:
        const unsigned int mc_nRows, mc_nCols;
        const float mc_fCellWidth, mc_fCellHeight;
        std::vector<Cell> m_Cells;

    public:
        LooseGrid(float width, float height, float cell_width, float cell_height)
            : mc_fCellWidth(cell_width)
            , mc_fCellHeight(cell_height)
            , mc_nRows(std::ceil(height / cell_height))
            , mc_nCols(std::ceil(width / cell_width))
            , m_Cells(mc_nRows * mc_nCols)
        { }

        std::array<unsigned int, 4> Align(float l, float t, float r, float b) const noexcept
        {
            return {
                std::max(0u, static_cast<unsigned int>(l / mc_fCellWidth)),
                std::max(0u, static_cast<unsigned int>(t / mc_fCellHeight)),
                std::min(mc_nCols, static_cast<unsigned int>(r / mc_fCellWidth)),
                std::min(mc_nRows, static_cast<unsigned int>(b / mc_fCellHeight)),
            };
        }

        unsigned int AlignX(float val) const noexcept
        {
            return std::max(0u, static_cast<unsigned int>(val / mc_fCellWidth));
        }

        unsigned int AlignY(float val) const noexcept
        {
            return std::min(mc_nCols, static_cast<unsigned int>(val / mc_fCellHeight));
        }

        unsigned int index(unsigned int x, unsigned int y) const noexcept
        {
            return y * mc_nCols + x;
        }
    };
}

// XXX: only for basic type now
template <typename T>
class LTGrid
{
    using Border = internal::Border;
    using LooseGrid = detail::LooseGrid<T>;
    using TightGrid = detail::TightGrid;

private:
    LooseGrid m_LooseGrid;
    TightGrid m_TightGrid;

public:
    LTGrid(float width, float height, float looseCellWidth, float looseCellHeight, float tightCellWidth, float tightCellHeight)
        : m_LooseGrid(width, height, looseCellWidth, looseCellHeight)
        , m_TightGrid(width, height, tightCellWidth, tightCellHeight)
    { }

    template <typename FT, typename = std::enable_if_t<std::is_same_v<std::remove_reference_t<FT>, T>>>
    void Insert(FT&& element, float cx, float cy, float hw, float hh)
    {
        const unsigned int lx = m_LooseGrid.AlignX(cx);
        const unsigned int ly = m_LooseGrid.AlignY(cy);
        const unsigned int lindex = m_LooseGrid.index(lx, ly);
        auto& lcell = m_LooseGrid.m_Cells[lindex];

        // inset to loose grid
        lcell.m_Elements.push_back(std::forward<FT>(element));
        const float prev_lmbr[4] = { lcell.m_MBR[Border::LEFT], lcell.m_MBR[Border::TOP], lcell.m_MBR[Border::RIGHT], lcell.m_MBR[Border::BOTTOM] };
        lcell.m_MBR[Border::LEFT] = std::min(lcell.m_MBR[Border::LEFT], cx - hw);
        lcell.m_MBR[Border::TOP] = std::min(lcell.m_MBR[Border::TOP], cy - hh);
        lcell.m_MBR[Border::RIGHT] = std::max(lcell.m_MBR[Border::RIGHT], cx + hw);
        lcell.m_MBR[Border::BOTTOM] = std::max(lcell.m_MBR[Border::BOTTOM], cy + hh);

        // update aabb and tight grid
        if (prev_lmbr[Border::LEFT] > prev_lmbr[Border::RIGHT]) {
            const std::array<unsigned int, 4> tmbr = m_TightGrid.Align(
                lcell.m_MBR[Border::LEFT], lcell.m_MBR[Border::TOP], lcell.m_MBR[Border::RIGHT], lcell.m_MBR[Border::BOTTOM]);
            for (unsigned int ty = tmbr[Border::TOP]; ty < tmbr[Border::BOTTOM]; ++ty) {
                for (unsigned int tx = tmbr[Border::LEFT]; tx < tmbr[Border::RIGHT]; ++tx) {
                    m_TightGrid.m_Cells[m_TightGrid.index(tx, ty)].m_LooseCells.push_back(lindex);
                }
            }
        } else if (prev_lmbr[Border::LEFT] != lcell.m_MBR[Border::LEFT] || prev_lmbr[Border::TOP] != lcell.m_MBR[Border::TOP]
            || prev_lmbr[Border::RIGHT] != lcell.m_MBR[Border::RIGHT] || prev_lmbr[Border::BOTTOM] != lcell.m_MBR[Border::BOTTOM]) {
            const std::array<unsigned int, 4> prev_tmbr = m_TightGrid.Align(
                prev_lmbr[Border::LEFT], prev_lmbr[Border::TOP], prev_lmbr[Border::RIGHT], prev_lmbr[Border::BOTTOM]);
            const std::array<unsigned int, 4> tmbr = m_TightGrid.Align(
                lcell.m_MBR[Border::LEFT], lcell.m_MBR[Border::TOP], lcell.m_MBR[Border::RIGHT], lcell.m_MBR[Border::BOTTOM]);
            for (unsigned int ty = tmbr[Border::TOP]; ty < tmbr[Border::BOTTOM]; ++ty) {
                if (ty >= prev_tmbr[Border::TOP] && ty <= prev_tmbr[Border::BOTTOM]) continue;
                for (unsigned int tx = tmbr[Border::LEFT]; tx < tmbr[Border::RIGHT]; ++tx) {
                    if (tx >= prev_tmbr[Border::LEFT] && tx <= prev_tmbr[Border::RIGHT]) continue;
                    // TODO: optimize vector, use faster struct if possible
                    m_TightGrid.m_Cells[m_TightGrid.index(tx, ty)].m_LooseCells.push_back(lindex);
                }
            }
        }
    }

    std::vector<T> Serach(float l, float t, float r, float b) const
    {
        std::unordered_set<unsigned int> loose_cells;
        const std::array<unsigned int, 4> tmbr = m_TightGrid.Align(l, t, r, b);
        for (unsigned int ty = tmbr[Border::TOP]; ty <= tmbr[Border::BOTTOM]; ++ty) {
            for (unsigned int tx = tmbr[Border::LEFT]; tx <= tmbr[Border::RIGHT]; ++tx) {
                // if the query region is smaller than the tight cell,
                // it is necessary to check whether the loose cell intersects the query region
                for (unsigned int lcell_idx : m_TightGrid.m_Cells[m_TightGrid.index(tx, ty)].m_LooseCells)
                    loose_cells.emplace(lcell_idx);
            }
        }

        std::vector<T> res;
        for (const unsigned int lcell_idx : loose_cells)
            for (const auto& element : m_LooseGrid.m_Cells[lcell_idx].m_Elements)
                res.emplace_back(element);

        return res;
    }
};
}
