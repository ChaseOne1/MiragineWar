#pragma once
#include <algorithm>
#include <array>
#include <bitset>
#include <cstdint>
#include <type_traits>
#include <vector>

namespace utility {
namespace internal {
    struct Rect
    {
        using Point = SDL_Point;

        Point left_top {}, right_bottom {}; // the relative positon must be correct

        Rect() = default;

        Rect(Point lt, Point rb)
            : left_top(lt)
            , right_bottom(rb)
        { }

        Rect(int x, int y, int w, int h)
            : left_top(Point { x, y })
            , right_bottom(Point { x + w, y + h })
        { }

        Rect(const SDL_Rect& rect)
            : left_top(Point { rect.x, rect.y })
            , right_bottom(Point { rect.x + rect.w, rect.y + rect.h })
        { }

        static Rect CalcMBR(const Rect& left, const Rect& right) noexcept
        {
            Point lt { std::min(left.left_top.x, right.left_top.x), std::min(left.left_top.y, right.left_top.y) };
            Point rb { std::max(left.right_bottom.x, right.right_bottom.x), std::max(left.right_bottom.y, right.right_bottom.y) };

            return Rect { lt, rb };
        }

        Rect CalcMBR(const Rect& right) const noexcept
        {
            return CalcMBR(*this, right);
        }

        int CalcArea() const noexcept
        {
            return (right_bottom.x - left_top.x) * (right_bottom.y - left_top.y);
        }

        bool Overlap(const Point& point) const noexcept
        {
            // the border isn't in the rect
            return point.x > left_top.x && point.x < right_bottom.x && point.y > left_top.y && point.y < right_bottom.y;
        }

        bool Overlap(const Rect& rect) const noexcept
        {
            return rect.left_top.x >= left_top.x && rect.left_top.y >= left_top.y && rect.right_bottom.x <= right_bottom.x && rect.right_bottom.y <= right_bottom.y;
        }
    };

    struct Index
    {
        using Index_t = uint32_t;

        using Leaf_t = bool;
        using Depth_t = uint8_t;
        using Capacity_t = uint8_t;
        using SubIndex_t = uint32_t;
        using Mask_t = uint32_t;

    private:
        static constexpr uint8_t IndexBitNum = 32u;
        static constexpr Mask_t DEPTH_READIN_MASK = 0b00001111;
        static constexpr Mask_t DEPTH_READOUT_MASK = 0x78000000; // ~0x87FFFFFF
        static constexpr Mask_t CAPACITY_READIN_MASK = 0b00000111;
        static constexpr Mask_t CAPACITY_READOUT_MASK = 0x7000000;
        static constexpr Mask_t SUBINDEX_READIN_MASK = 0xFFFFFF;
        static constexpr Mask_t SUBINDEX_READOUT_MASK = 0xFFFFFF;

    public:
        // 1(is_leaf) 4(depth) 3(capacity) [8 8 8](index);
        std::bitset<IndexBitNum> m_nIndex;

        Index(Leaf_t isLeaf, Depth_t depth, Capacity_t capacity, SubIndex_t subIndex)
        {
            SetIsleaf(isLeaf);
            SetDepth(depth);
            SetCapacity(capacity);
            SetSubIndex(subIndex);
        }

        Leaf_t GetIsLeaf() const noexcept
        {
            return m_nIndex.test(IndexBitNum - 1);
        }

        void SetIsleaf(Leaf_t isLeaf) noexcept
        {
            m_nIndex.set(IndexBitNum - 1, isLeaf);
        }

        Depth_t GetDepth() const noexcept
        {
            return (m_nIndex.to_ulong() & DEPTH_READOUT_MASK) >> 27;
        }

        void SetDepth(Depth_t depth) noexcept
        {
            depth = std::min(static_cast<Depth_t>(11), depth);
            m_nIndex &= ~DEPTH_READOUT_MASK;
            m_nIndex |= (depth & DEPTH_READIN_MASK) << 27;
        }

        Capacity_t GetCapacity() const noexcept
        {
            return (m_nIndex.to_ulong() & CAPACITY_READOUT_MASK) >> 24;
        }

        void SetCapacity(Capacity_t capacity) noexcept
        {
            m_nIndex &= ~CAPACITY_READOUT_MASK;
            m_nIndex |= (capacity & CAPACITY_READIN_MASK) << 24;
        }

        SubIndex_t GetSubIndex() const noexcept
        {
            return m_nIndex.to_ulong() & SUBINDEX_READOUT_MASK;
        }

        void SetSubIndex(SubIndex_t subIndex) noexcept
        {
            m_nIndex &= ~SUBINDEX_READOUT_MASK;
            m_nIndex |= (subIndex & SUBINDEX_READIN_MASK);
        }
    };

    template <typename T>
    struct Node
    {
        T m_nInfo;
        Rect m_MBR;
    };
}

template <typename T, internal::Index::Capacity_t DataNodeCapacity = 4, internal::Index::Depth_t MaxDepth = 8,
    typename = std::enable_if_t<(DataNodeCapacity < 8)>, typename = std::enable_if_t<(MaxDepth < 12)>> // 12 = int max_depth = log4(max_index / max_capacity);
                                                                                                       // ensure the 24bit index and 3bit capacity are fully used.
class LooseQuadTree
{
    using Index = internal::Index;
    using DirNode = internal::Node<Index>;
    using DirNodeClump = std::array<DirNode, 4u>;
    using DataNode = internal::Node<T>;
    using DataNodeClump = std::array<DataNode, DataNodeCapacity + 1>;
    using Point = internal::Rect::Point;
    using Rect = internal::Rect;

    enum ChildNode
    {
        LEFT_TOP,
        RIGHT_TOP,
        LEFT_BOTTOM,
        RIGHT_BOTTOM,
        CHILD_NUM,
    };

private:
    std::vector<DataNodeClump> m_Data;
    std::vector<DirNodeClump> m_Nodes;
    std::vector<internal::Index::SubIndex_t> m_FreeDataNodeClumps;
    DirNode root;

private:
    void SplitNode(DirNode* node)
    {
        if (node->m_nInfo.GetCapacity() <= DataNodeCapacity || node->m_nInfo.GetDepth() == MaxDepth)
            return;

        // create the four child
        m_Nodes.emplace_back(
            DirNodeClump { DirNode { Index { true, static_cast<internal::Index::Depth_t>(node->m_nInfo.GetDepth() + 1), 0, 0 }, Rect {} },
                DirNode { Index { true, static_cast<internal::Index::Depth_t>(node->m_nInfo.GetDepth() + 1), 0, 0 }, Rect {} },
                DirNode { Index { true, static_cast<internal::Index::Depth_t>(node->m_nInfo.GetDepth() + 1), 0, 0 }, Rect {} },
                DirNode { Index { true, static_cast<internal::Index::Depth_t>(node->m_nInfo.GetDepth() + 1), 0, 0 }, Rect {} } });
        DirNodeClump& leaves = m_Nodes.back();
        // move the element to new leaf
        const Point node_center { (node->m_MBR.left_top.x + node->m_MBR.right_bottom.x) >> 1,
            (node->m_MBR.left_top.y + node->m_MBR.right_bottom.y) >> 1 };
        auto InsertIntoLeaf = [this, node](DirNode& leaf, internal::Index::SubIndex_t j) {
            if (!leaf.m_nInfo.GetSubIndex()) {
                if (m_FreeDataNodeClumps.size()) {
                    leaf.m_nInfo.SetSubIndex(m_FreeDataNodeClumps.back());
                    m_FreeDataNodeClumps.pop_back();
                } else {
                    leaf.m_nInfo.SetSubIndex(m_Data.size());
                    m_Data.emplace_back();
                }
            }
            m_Data[leaf.m_nInfo.GetSubIndex()][leaf.m_nInfo.GetCapacity()] = m_Data[node->m_nInfo.GetSubIndex()][j];
            leaf.m_nInfo.SetCapacity(leaf.m_nInfo.GetCapacity() + 1);
            leaf.m_MBR = leaf.m_MBR.CalcMBR(m_Data[node->m_nInfo.GetSubIndex()][j].m_MBR);
        };
        for (internal::Index::Capacity_t j = 0; j < node->m_nInfo.GetCapacity(); ++j) {
            const Point center { (m_Data[node->m_nInfo.GetSubIndex()][j].m_MBR.left_top.x + m_Data[node->m_nInfo.GetSubIndex()][j].m_MBR.right_bottom.x) >> 1,
                (m_Data[node->m_nInfo.GetSubIndex()][j].m_MBR.left_top.y + m_Data[node->m_nInfo.GetSubIndex()][j].m_MBR.right_bottom.y) >> 1 };
            if (center.x >= node_center.x) {
                if (center.y >= node_center.y) InsertIntoLeaf(leaves[ChildNode::RIGHT_BOTTOM], j);
                else InsertIntoLeaf(leaves[ChildNode::RIGHT_TOP], j);
            } else {
                if (center.y >= node_center.y) InsertIntoLeaf(leaves[ChildNode::LEFT_BOTTOM], j);
                else InsertIntoLeaf(leaves[ChildNode::LEFT_TOP], j);
            }
        }
        m_FreeDataNodeClumps.emplace_back(node->m_nInfo.GetSubIndex());
        node->m_nInfo.SetIsleaf(false);
        node->m_nInfo.SetSubIndex(m_Nodes.size());

        for (int i = ChildNode::LEFT_TOP; i < ChildNode::CHILD_NUM; ++i)
            SplitNode(&leaves[i]);
    }

public:
    LooseQuadTree()
        : root({ { true, 0, 0, 0 }, {} })
        , m_Data(1)
    {
    }

    void Insert(const T& obj, const Rect& mbr)
    {
        const Point center { (mbr.left_top.x + mbr.right_bottom.x) >> 1 };
        DirNode* node = &root;

        // find leaf and adjust mbr on path
        while (!node->m_nInfo.GetIsLeaf()) {
            node->m_MBR = node->m_MBR.CalcMBR(mbr);
            const Point node_center = { (node->m_MBR.left_top.x + node->m_MBR.right_bottom.x) >> 1,
                (node->m_MBR.left_top.y + node->m_MBR.right_bottom.y) >> 1 };
            if (center.x >= node_center.x) {
                if (center.y >= node_center.y) node = &m_Nodes[node->m_nInfo.GetSubIndex()][ChildNode::RIGHT_BOTTOM];
                else node = &m_Nodes[node->m_nInfo.GetSubIndex()][ChildNode::RIGHT_TOP];
            } else {
                if (center.y >= node_center.y) node = &m_Nodes[node->m_nInfo.GetSubIndex()][ChildNode::LEFT_BOTTOM];
                else node = &m_Nodes[node->m_nInfo.GetSubIndex()][ChildNode::LEFT_TOP];
            }
        }

        // insert into the leaf
        m_Data[node->m_nInfo.GetSubIndex()][node->m_nInfo.GetCapacity()] = DataNode { obj, mbr };
        node->m_nInfo.SetCapacity(node->m_nInfo.GetCapacity() + 1);
        node->m_MBR = node->m_MBR.CalcMBR(mbr);
        // split if it is overflow, or nothing
        SplitNode(node);
    }

    // void Insert(T&& obj, const Rect& mbr);

    void Search(const Point& point, std::vector<T>& res, DirNode* node = nullptr)
    {
        if (!node) node = &root;
        if (!node->m_MBR.Overlap(point)) return;

        if (node->m_nInfo.GetIsLeaf()) {
            for (internal::Index::Capacity_t i = 0; i < node->m_nInfo.GetCapacity(); ++i) {
                const auto& element = m_Data[node->m_nInfo.GetSubIndex()][i];
                if (element.m_MBR.Overlap(point)) res.emplace_back(element.m_nInfo);
            }
            return;
        }

        for (int i = ChildNode::LEFT_TOP; i < ChildNode::CHILD_NUM; ++i) {
            Search(point, res, &m_Nodes[node->m_nInfo.GetSubIndex()][i]);
        }
    }

    void Delete(const T& obj);
};
}
