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
                static_cast<unsigned int>(std::max(0.f, l / mc_fCellWidth)),
                static_cast<unsigned int>(std::max(0.f, t / mc_fCellHeight)),
                std::min(mc_nCols - 1u, static_cast<unsigned int>(r / mc_fCellWidth)),
                std::min(mc_nRows - 1u, static_cast<unsigned int>(b / mc_fCellHeight)),
            };
        }

        unsigned int AlignX(float val) const noexcept
        {
            return std::max(0u, static_cast<unsigned int>(val / mc_fCellWidth));
        }

        unsigned int AlignY(float val) const noexcept
        {
            return std::min(mc_nCols - 1u, static_cast<unsigned int>(val / mc_fCellHeight));
        }

        unsigned int index(unsigned int x, unsigned int y) const noexcept
        {
            return y * mc_nCols + x;
        }

        Cell& CellAt(unsigned int x, unsigned int y)
        {
            return m_Cells.at(index(x, y));
        }

        const Cell& CellAt(unsigned int x, unsigned int y) const
        {
            return m_Cells.at(index(x, y));
        }
    };

    template <typename T>
    struct LooseGrid
    {
        struct Element
        {
            T object;
            float cx = 0.f, cy = 0.f;
            float hw = 0.f, hh = 0.f;
        };

        struct Cell
        {
            std::vector<Element> m_Elements;
            std::array<float, 4> m_MBR { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() };

        public:
            void UpdateMBR(float cx, float cy, float hw, float hh) noexcept
            {
                m_MBR[internal::Border::LEFT] = std::min(m_MBR[internal::Border::LEFT], cx - hw);
                m_MBR[internal::Border::TOP] = std::min(m_MBR[internal::Border::TOP], cy - hh);
                m_MBR[internal::Border::RIGHT] = std::max(m_MBR[internal::Border::RIGHT], cx + hw);
                m_MBR[internal::Border::BOTTOM] = std::max(m_MBR[internal::Border::BOTTOM], cy + hh);
            }
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
                static_cast<unsigned int>(std::max(0.f, l / mc_fCellWidth)),
                static_cast<unsigned int>(std::max(0.f, t / mc_fCellHeight)),
                std::min(mc_nCols - 1u, static_cast<unsigned int>(r / mc_fCellWidth)),
                std::min(mc_nRows - 1u, static_cast<unsigned int>(b / mc_fCellHeight)),
            };
        }

        unsigned int AlignX(float val) const noexcept
        {
            return std::max(0u, static_cast<unsigned int>(val / mc_fCellWidth));
        }

        unsigned int AlignY(float val) const noexcept
        {
            return std::min(mc_nCols - 1u, static_cast<unsigned int>(val / mc_fCellHeight));
        }

        unsigned int index(unsigned int x, unsigned int y) const noexcept
        {
            return y * mc_nCols + x;
        }

        Cell& CellAt(unsigned int x, unsigned int y)
        {
            return m_Cells.at(index(x, y));
        }

        const Cell& CellAt(unsigned int x, unsigned int y) const
        {
            return m_Cells.at(index(x, y));
        }

        Cell& CellAt(float cx, float cy)
        {
            return m_Cells.at(index(AlignX(cx), AlignY(cy)));
        }

        const Cell& CellAt(float cx, float cy) const
        {
            return m_Cells.at(index(AlignX(cx), AlignY(cy)));
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

    // for perfectly forwarding
    template <typename FT, typename = std::enable_if_t<std::is_same_v<std::remove_reference_t<FT>, T>>>
    void Insert(FT&& object, float cx, float cy, float hw, float hh)
    {
        const unsigned int lx = m_LooseGrid.AlignX(cx), ly = m_LooseGrid.AlignY(cy);
        const unsigned int lindex = m_LooseGrid.index(lx, ly);
        auto& lcell = m_LooseGrid.m_Cells[lindex];

        // insert the element to loose grid
        lcell.m_Elements.emplace_back(typename LooseGrid::Element { object, cx, cy, hw, hh });
        const std::array<float, 4> prev_lmbr { lcell.m_MBR };
        lcell.UpdateMBR(cx, cy, hw, hh);

        // update aabb and tight grid
        if (prev_lmbr[Border::LEFT] > prev_lmbr[Border::RIGHT]) {
            const std::array<unsigned int, 4> tmbr = m_TightGrid.Align(
                lcell.m_MBR[Border::LEFT], lcell.m_MBR[Border::TOP], lcell.m_MBR[Border::RIGHT], lcell.m_MBR[Border::BOTTOM]);
            for (unsigned int ty = tmbr[Border::TOP]; ty <= tmbr[Border::BOTTOM]; ++ty) {
                for (unsigned int tx = tmbr[Border::LEFT]; tx <= tmbr[Border::RIGHT]; ++tx) {
                    m_TightGrid.CellAt(tx, ty).m_LooseCells.push_back(lindex);
                }
            }
        } else if (prev_lmbr[Border::LEFT] != lcell.m_MBR[Border::LEFT] || prev_lmbr[Border::TOP] != lcell.m_MBR[Border::TOP]
            || prev_lmbr[Border::RIGHT] != lcell.m_MBR[Border::RIGHT] || prev_lmbr[Border::BOTTOM] != lcell.m_MBR[Border::BOTTOM]) {
            const std::array<unsigned int, 4> prev_tmbr = m_TightGrid.Align(
                prev_lmbr[Border::LEFT], prev_lmbr[Border::TOP], prev_lmbr[Border::RIGHT], prev_lmbr[Border::BOTTOM]);
            const std::array<unsigned int, 4> tmbr = m_TightGrid.Align(
                lcell.m_MBR[Border::LEFT], lcell.m_MBR[Border::TOP], lcell.m_MBR[Border::RIGHT], lcell.m_MBR[Border::BOTTOM]);
            for (unsigned int ty = tmbr[Border::TOP]; ty <= tmbr[Border::BOTTOM]; ++ty) {
                if (ty >= prev_tmbr[Border::TOP] && ty <= prev_tmbr[Border::BOTTOM]) continue;
                for (unsigned int tx = tmbr[Border::LEFT]; tx <= tmbr[Border::RIGHT]; ++tx) {
                    if (tx >= prev_tmbr[Border::LEFT] && tx <= prev_tmbr[Border::RIGHT]) continue;
                    // TODO: optimize vector, use faster struct if possible
                    m_TightGrid.CellAt(tx, ty).m_LooseCells.push_back(lindex);
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
                for (unsigned int lcell_idx : m_TightGrid.CellAt(tx, ty).m_LooseCells)
                    loose_cells.emplace(lcell_idx);
            }
        }

        std::vector<T> res;
        for (const unsigned int lcell_idx : loose_cells)
            for (const auto& element : m_LooseGrid.m_Cells[lcell_idx].m_Elements)
                res.emplace_back(element.object); // TODO: should check if overlap each other

        return res;
    }

    void Delete(const T& object, float cx, float cy)
    {
        auto& elements = m_LooseGrid.CellAt(cx, cy).m_Elements;

        if (auto iter = std::find_if(elements.begin(), elements.end(),
                [&object](typename LooseGrid::Element& element) { return element.object == object; });
            iter != elements.end()) {
            std::swap(*iter, elements.back());
            elements.pop_back();
            // we don't adjust the mbr immidiately to accommodate high frequency deletion
            // but optimize all each frame
        }
    }

    // pass the "from" parameters is to save query time
    void Move(const T& object, float from_cx, float from_cy, float to_cx, float to_cy)
    {
        const unsigned int from_lx = m_LooseGrid.AlignX(from_cx), to_lx = m_LooseGrid.AlignX(to_cx);
        const unsigned int from_ly = m_LooseGrid.AlignY(from_cy), to_ly = m_LooseGrid.AlignY(to_cy);
        auto& lcell = m_LooseGrid.CellAt(from_lx, from_ly);
        auto& elements = lcell.m_Elements;
        auto iter = std::find_if(elements.begin(), elements.end(),
            [&object](typename LooseGrid::Element& element) { return element.object == object; });
        assert(iter != elements.end());

        if (to_lx != from_lx || to_ly != from_ly) {
            std::swap(*iter, elements.back());
            Insert(std::move(elements.back().object), to_cx, to_cy, elements.back().hw, elements.back().hh);
            elements.pop_back();
            // we don't adjust the mbr immidiately but optimize all each frame
        } else {
            // just adjust the aabb
            lcell.UpdateMBR(from_cx, from_cy, iter->hw, iter->hh);
        }
    }

    void Optimize()
    {
        // clear the tight cells' data
        std::for_each(m_TightGrid.m_Cells.begin(), m_TightGrid.m_Cells.end(), [](TightGrid::Cell& cell) { cell.m_LooseCells.clear(); });

        // adjust the loose cells' mbr
        for (auto& cell : m_LooseGrid.m_Cells) {
            // clear
            cell.m_MBR[Border::LEFT] = std::numeric_limits<float>::max();
            cell.m_MBR[Border::TOP] = std::numeric_limits<float>::max();
            cell.m_MBR[Border::RIGHT] = -std::numeric_limits<float>::max();
            cell.m_MBR[Border::BOTTOM] = -std::numeric_limits<float>::max();

            // expand
            for (auto& element : cell.m_Elements)
                cell.UpdateMBR(element.cx, element.cy, element.hw, element.hh);
        }

        // set the data of overlap
        for (unsigned int i = 0; i < m_LooseGrid.m_Cells.size(); ++i) {
            auto& cell = m_LooseGrid.m_Cells[i];
            const std::array<unsigned int, 4> tmbr = m_TightGrid.Align(
                cell.m_MBR[Border::LEFT], cell.m_MBR[Border::TOP], cell.m_MBR[Border::RIGHT], cell.m_MBR[Border::BOTTOM]);
            for (unsigned int ty = tmbr[Border::TOP]; ty <= tmbr[Border::BOTTOM]; ++ty)
                for (unsigned int tx = tmbr[Border::LEFT]; tx <= tmbr[Border::RIGHT]; ++tx)
                    m_TightGrid.CellAt(tx, ty).m_LooseCells.push_back(i);
        }
    }
};
}
