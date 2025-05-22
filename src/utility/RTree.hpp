#pragma once

namespace utility {
constexpr int M = 8;
constexpr int m = M * 0.4f;

struct Rect
{
    SDL_Point left_top {}, right_bottom {}; // the relative positon must be correct

    Rect() = default;

    Rect(SDL_Point lt, SDL_Point rb)
        : left_top(lt)
        , right_bottom(rb)
    {
    }

    Rect(const SDL_Rect& rect)
        : left_top(SDL_Point { rect.x, rect.y })
        , right_bottom(SDL_Point { rect.x + rect.w, rect.y + rect.h })
    {
    }

    static Rect CalcMBR(const Rect& left, const Rect& right) noexcept
    {
        SDL_Point lt { std::min(left.left_top.x, right.left_top.x), std::min(left.left_top.y, right.left_top.y) };
        SDL_Point rb { std::max(left.right_bottom.x, right.right_bottom.x), std::max(left.right_bottom.y, right.right_bottom.y) };

        return Rect { lt, rb };
    }

    int CalcArea() const noexcept
    {
        return (right_bottom.x - left_top.x) * (right_bottom.y - left_top.y);
    }

    bool Overlap(const SDL_Point& point) const noexcept
    {
        return point.x > left_top.x && point.x < right_bottom.x && point.y > left_top.y && point.y < right_bottom.y;
    }

    bool Overlap(const Rect& rect) const noexcept
    {
        return rect.left_top.x >= left_top.x && rect.left_top.y >= left_top.y && rect.right_bottom.x <= right_bottom.x && rect.right_bottom.y <= right_bottom.y;
    }
};

template <typename T>
class RTree;

template <typename T>
class Node // TIPS: maybe should split to DataNode and DirNode
{
    friend class RTree<T>;
    using Element = std::pair<T, Rect>;

private:
    std::vector<Node> children; // in DataNode, it's nothing
    std::vector<Element> data; // in DirNode, it's nothing
                               // any element is managed by a DataNode; A DataNode manages many elements
                               // TODO: std::list
    Rect mbr {};
    Node* parent;

private:
    Node(Node* parentt = nullptr)
        : parent(parentt)
    {
    }

    bool Insert(Node&& node)
    {
        // node.parent = this;
        mbr = Rect::CalcMBR(mbr, node.mbr);
        children.emplace_back(std::move(node));

        if (children.size() > M)
            return true;
        else
            return false;
    }

    template <typename E,
        typename = std::enable_if<std::is_same_v<std::remove_reference_t<E>, Element>>>
    bool Insert(E&& element)
    {
        mbr = Rect::CalcMBR(mbr, element.second);
        data.emplace_back(std::forward<E>(element));

        if (data.size() > M)
            return true; // need split
        else
            return false;
    }

    bool Delete(typename std::vector<Element>::iterator iter)
    {
        std::swap(*iter, data.back());
        data.pop_back();
        mbr = ReCalcMBR();

        if (data.size() < m)
            return true;
        else
            return false;
    }

    T* Overlap(const SDL_Point& point)
    {
        for (auto& child : children) {
            if (child.mbr.Overlap(point)) {
                if (T* val = child.Overlap(point))
                    return val;
            }
        }

        // the last has higher priority if there is overlap
        for (auto iter = data.rbegin(); iter != data.rend(); ++iter) {
            if (iter->second.Overlap(point))
                return &iter->first;
        }

        return nullptr;
    }

    // TODO: std::vector<Value*> Overlap(const Rect& rect);

    // TODO: consider that provide only interface like pop_back() instead of search
    std::pair<Node*, typename std::vector<Element>::reverse_iterator>
    Search(const Element& element)
    {
        for (auto iter = children.begin(); iter != children.end(); ++iter) {
            if (iter->mbr.Overlap(element.second)) {
                // pair + node + element
                if (auto&& pne = iter->Search(element); pne.first != nullptr)
                    return pne;
            }
        }

        for (auto iter = data.rbegin(); iter != data.rend(); ++iter) {
            if (iter->first == element.first /*&& iter->second.Overlap(element.second)*/)
                return { this, iter }; // first is meaningless now
        }

        return { nullptr, data.rend() };
    }

    Rect ReCalcMBR()
    {
        Rect mbr {};

        for (const auto& e : data)
            mbr = Rect::CalcMBR(mbr, e.second);

        return mbr;
    }
};

template <typename T>
class RTree
{
    using Element = std::pair<T, Rect>;
    using NodeT = Node<T>;

private:
    NodeT* root;

private:
    NodeT& ChooseLeaf(const Rect& mbr)
    {
        NodeT* node = root;
        while (true) {
            if (node->data.size() || node == root && !node->children.size())
                return *node;

            Rect min_mbr {};
            int min_diff = 0;
            for (auto& child : node->children) {
                const Rect mbr = Rect::CalcMBR(child.mbr, mbr);
                const int diff = mbr.CalcArea() - child.mbr.CalcArea();
                // min_diff = diff < min_diff ? min_mbr = mbr, diff : min_diff;
                if (diff < min_diff || diff == min_diff && mbr.CalcArea() < min_mbr.CalcArea()) {
                    node = &child;
                    // min_mbr = mbr;
                    // min_diff = diff;
                }
            }
        }
    }

    std::pair<Element*, Element*> FindSeed(NodeT& node)
    {
        int max_area = 0;
        Element *n1 = nullptr, *n2 = nullptr;

        for (int i = 0; i < node.data.size() - 1; ++i) {
            for (int j = i + 1; j < node.data.size(); ++j) {
                int area = Rect::CalcMBR(node.data[i].second, node.data[j].second).CalcArea();
                max_area = area > max_area ? n1 = &node.data[i], n2 = &node.data[j], area : max_area;
            }
        }

        return std::pair(n1, n2);
    }

    std::pair<NodeT, NodeT> SplitNode(NodeT& node)
    {
        NodeT nl {}, nr {};
        nl.parent = nr.parent = node.parent; // why not in C++20, this is too much.
        auto [e1, e2] = FindSeed(node);

        // move seeds firstly
        auto move_seed = [&](Element* e, NodeT& n) {
            std::swap(*e, node.data.back());
            n.data.emplace_back(std::move(node.data.back()));
            node.data.pop_back();
            e = &n.data.front();
            n.mbr = e->second;
        };
        move_seed(e1, nl);
        move_seed(e2, nr);

        // divide the others
        auto emplace = [&](NodeT& n, Element&& e, const Rect& mbr) { // Q: why not move the mbr?
            // A: mbr_x is not created per loop, instead once,
            //    therefore, once moved, it cannot be reused
            n.data.emplace_back(std::move(e));
            n.mbr = mbr;
        };
        for (int i = 0; i < node.data.size(); ++i) {
            const Rect mbr_l = Rect::CalcMBR(node.data[i].second, nl.mbr);
            const int diff_l = mbr_l.CalcArea() - nl.mbr.CalcArea();
            const Rect mbr_r = Rect::CalcMBR(node.data[i].second, nr.mbr);
            const int diff_r = mbr_r.CalcArea() - nr.mbr.CalcArea();
            // balance num of data
            if (node.data.size() - i - 1 == m) {
                if (nl.data.size() < nr.data.size())
                    emplace(nl, std::move(node.data[i]), mbr_l);
                else
                    emplace(nr, std::move(node.data[i]), mbr_r);
                continue;
            }
            // in normal case:
            if (diff_l < diff_r)
                emplace(nl, std::move(node.data[i]), mbr_l);
            else if (diff_l > diff_r)
                emplace(nr, std::move(node.data[i]), mbr_r);
            else if (nl.mbr.CalcArea() < nr.mbr.CalcArea())
                emplace(nl, std::move(node.data[i]), mbr_l);
            else if (nl.mbr.CalcArea() > nr.mbr.CalcArea())
                emplace(nr, std::move(node.data[i]), mbr_r);
            else if (nl.data.size() < nr.data.size())
                emplace(nl, std::move(node.data[i]), mbr_l);
            else
                emplace(nr, std::move(node.data[i]), mbr_r);
        }

        return std::pair(std::move(nl), std::move(nr));
    }

    void Insert(Element&& element)
    {
        NodeT* node = &ChooseLeaf(element.second);
        if (node->Insert(std::move(element))) {
            // AdjustTree or, in other words, spreading division until someone can contain everthing.
            std::pair<NodeT, NodeT> pn { nullptr, nullptr };
            do {
                pn = SplitNode(*node);
                if (node == root) {
                    NodeT* new_root = new NodeT();
                    root = pn.first.parent = pn.second.parent = new_root;
                    delete node;
                    node = root;
                    node->Insert(std::move(pn.first));
                } else {
                    *node = std::move(pn.first); // of course, for second
                    node = node->parent;
                }
            } while (node->Insert(std::move(pn.second)));
        }
    }

public:
    RTree()
        : root(new NodeT())
    {
    }

    ~RTree()
    {
        delete root;
    }

    void Insert(const T& val, const SDL_Rect& mbr)
    {
        Insert(Element { val, mbr });
    }

    void Delete(const T& val /*, const SDL_Rect& mbr*/)
    {
        const auto [data_node, data_riter] = root->Search(Element { val, {} });
        if (!data_node)
            return;

        if (data_node->Delete(data_riter.base() - 1)) {
            NodeT *parent = data_node, *node = data_node; // no problem
            std::forward_list<std::vector<Element>> free_datas;
            do {
                node = parent;
                parent = node->parent;
                if (node == root && node->children.size() == 1 && !node->data.size()) {
                    *root = std::move(node->children.front());
                    break;
                }
                free_datas.emplace_front(std::move(node->data));
                std::swap(*(parent->children.begin() + std::distance(node, parent->children.data())), parent->children.back());
                parent->children.pop_back();
            } while (parent->children.size() < m);

            // forward_list is reversed, so don't need to traverse by post order
            for (auto& vec : free_datas)
                for (auto&& element : vec)
                    Insert(std::move(element));
        }
    }

    T* Search(const SDL_Point& point) const noexcept { return root->Overlap(point); }
};
}
