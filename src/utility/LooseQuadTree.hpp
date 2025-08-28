#pragma once
namespace utility {
template <typename T>
class LooseQuadtree
{
public:
    LooseQuadtree(float centerX, float centerY, float halfWidth, float halfHeight,
        int maxDepth = 6, int nodeCapacity = 4)
        : MAX_DEPTH(maxDepth)
        , NODE_CAPACITY(nodeCapacity)
    {
        AABB bounds { { centerX, centerY }, halfWidth, halfHeight };
        root = std::make_unique<Node>(Node { bounds, 0 });
    }

    void insert(const T& id, float centerX, float centerY, float halfWidth, float halfHeight)
    {
        AABB bounds { { centerX, centerY }, halfWidth, halfHeight };
        elements[id] = bounds;
        insert(root.get(), id, bounds);
    }

    void remove(const T& id)
    {
        auto it = elementToNode.find(id);
        if (it == elementToNode.end()) return;
        Node* node = it->second;
        node->elements.erase(id);
        elementToNode.erase(id);
        elements.erase(id);
    }

    std::vector<T> query(float x, float y) const
    {
        std::vector<T> result;
        Vec2 point { x, y };
        query(root.get(), point, result);
        return result;
    }

private:
    struct Vec2
    {
        float x, y;
    };

    struct AABB
    {
        Vec2 center;
        float halfWidth, halfHeight;

        bool containsPoint(const Vec2& point) const
        {
            return std::abs(point.x - center.x) <= halfWidth && std::abs(point.y - center.y) <= halfHeight;
        }

        bool intersects(const AABB& other) const
        {
            return std::abs(center.x - other.center.x) <= (halfWidth + other.halfWidth) && std::abs(center.y - other.center.y) <= (halfHeight + other.halfHeight);
        }
    };

    struct Node
    {
        AABB bounds;
        int depth;
        std::unordered_set<T> elements;
        std::unique_ptr<Node> children[4] = { nullptr };

        bool isLeaf() const { return children[0] == nullptr; }
    };

    std::unique_ptr<Node> root;
    const int MAX_DEPTH;
    const int NODE_CAPACITY;

    std::unordered_map<T, AABB> elements; // ID -> AABB
    std::unordered_map<T, Node*> elementToNode; // ID -> Node pointer

    void insert(Node* node, const T& id, const AABB& bounds)
    {
        if (!node->bounds.intersects(bounds)) return;

        if (node->isLeaf()) {
            node->elements.insert(id);
            elementToNode[id] = node;

            if ((int)node->elements.size() > NODE_CAPACITY && node->depth < MAX_DEPTH)
                subdivide(node);
        } else {
            for (auto& child : node->children) {
                if (child->bounds.intersects(bounds)) {
                    insert(child.get(), id, bounds);
                }
            }
        }
    }

    void subdivide(Node* node)
    {
        const float hw = node->bounds.halfWidth / 2.0f;
        const float hh = node->bounds.halfHeight / 2.0f;
        const Vec2& c = node->bounds.center;

        node->children[0] = std::make_unique<Node>(Node { { { c.x - hw, c.y - hh }, hw, hh }, node->depth + 1 });
        node->children[1] = std::make_unique<Node>(Node { { { c.x + hw, c.y - hh }, hw, hh }, node->depth + 1 });
        node->children[2] = std::make_unique<Node>(Node { { { c.x - hw, c.y + hh }, hw, hh }, node->depth + 1 });
        node->children[3] = std::make_unique<Node>(Node { { { c.x + hw, c.y + hh }, hw, hh }, node->depth + 1 });

        std::vector<T> reinsert(node->elements.begin(), node->elements.end());
        node->elements.clear();

        for (const T& id : reinsert) {
            elementToNode.erase(id);
            insert(node, id, elements[id]);
        }
    }

    void query(const Node* node, const Vec2& point, std::vector<T>& out) const
    {
        if (!node->bounds.containsPoint(point)) return;

        for (const T& id : node->elements) {
            if (elements.at(id).containsPoint(point)) {
                out.push_back(id);
            }
        }

        if (!node->isLeaf()) {
            for (const auto& child : node->children) {
                query(child.get(), point, out);
            }
        }
    }
};
}
