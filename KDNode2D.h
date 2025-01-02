//
// Created by Anis Han on 24-12-28.
//

#ifndef KDNODE2D_H
#define KDNODE2D_H

#include <vector>
#include <algorithm>


/**
 * KD树的三元组
 * @tparam T 第一个维度
 * @tparam K 第二个维度
 * @tparam V 数据
 */
template<class T, class K, class V>
struct  KDPair2D {
    T first;
    K second;
    V value;
};

/**
 * 真正KD树的节点，里面是三元组对象和前后KD树指针
 * @tparam T
 * @tparam K
 * @tparam V
 */
template<class T, class K, class V>
struct KDNode2D {
    KDPair2D<T, K, V> data;
    KDNode2D *left;
    KDNode2D *right;
};

template<class T, class V>
class KDTree2D {
    using K = T;

    struct KVPair {
        T first;
        K second;
    };

public:
    using Node = KDNode2D<T, K, V>;
    using Pair = KDPair2D<T, K, V>;
    using Iterator = typename std::vector<Pair>::iterator;

private:
    /**
     * 切分中文数，目前基于快排，后期将基于TopK算法（局部排序）
     * @param pairs
     * @param beg 开始索引
     * @param end 结束索引
     * @param dimension 维度true为第一维度，false为第二维度
     * @return 返回中位数迭代器
     */
    static size_t get_median(std::vector<Pair> &pairs, size_t beg, size_t end, bool dimension) {
        if (beg == end) {
            return end;
        }
        std::sort(pairs.begin() + beg, pairs.begin() + end, [dimension] (const Pair &p1, const Pair &p2) {
            bool result;
            if (dimension) {
                // 第二维度
                result = p1.second < p2.second;
            } else {
                // 第一维度
                result = p1.first < p2.first;

            }
            return result;
        });
        return (beg + end) / 2;;

    }

    static Node *buildTreeRecursive(std::vector<Pair> &pairs, size_t beg, size_t end, bool dimension) {
        size_t median = get_median(pairs, beg, end, dimension);
        if (median == end) {
            return nullptr;
        }

        Node *curr = new Node{pairs[median], nullptr, nullptr};

        // 继续填充，翻转维度
        curr->left = buildTreeRecursive(pairs, beg, median, !dimension);
        curr->right = buildTreeRecursive(pairs, median + 1, end, !dimension);

        return curr;

    }


    template<class Consumer>
    void dlr_recursion(Node *root, Consumer consumer) {
        if (root == nullptr) {
            return;
        }
        consumer(root);
        dlr_recursion(root->left, consumer);
        dlr_recursion(root->right, consumer);
    }

    template<class Consumer>
    void ldr_recursion(Node *root, Consumer consumer) {
        if (root == nullptr) {
            return;
        }

        ldr_recursion(root->left, consumer);
        consumer(root);
        ldr_recursion(root->right, consumer);
    }

    static T get_dimension_value(const Pair &node, bool dimension) {
        return dimension ? node.first: node.second;;
    }
    static T get_dimension_value(const KVPair &node, bool dimension) {
        return dimension ? node.first: node.second;;
    }

    static T get_dimension_value(const Node *node, const bool dimension) {
        if (node == nullptr) {
            return std::numeric_limits<T>::max();
        }
        return get_dimension_value(node->data, dimension);
    }


    static double get_distance(const Node *node, const KVPair &node_pair) {
        if (node == nullptr) {
            return std::numeric_limits<T>::max();
        }
        const auto diff1 = node->data.first - node_pair.first;
        const auto diff2 = node->data.second - node_pair.second;

        return std::sqrt(diff1 * diff1 + diff2 * diff2);

    }

    static double get_boundary_distance(const Node *node, const KVPair &node_pair, bool dimension) {
        if (node == nullptr) {
            return std::numeric_limits<T>::max();
        }

        const auto boundary = get_dimension_value(node->data, dimension);
        const auto search = get_dimension_value(node_pair, dimension);
        return std::abs(boundary - search);
    }

    static const Node *backtrack_min(const Node *min_node, const Node* root, const KVPair &pair, const bool dimension) {
        if (root == nullptr) {
            return min_node;
        }

        const auto boundary_distance = get_boundary_distance(root, pair, dimension);
        auto radius = get_distance(min_node, pair);

        if (radius < boundary_distance) {
            return min_node;
        }

        const auto new_radius = get_distance(root, pair);
        if (new_radius < radius) {
            min_node = root;
        }

        min_node = backtrack_min(min_node, root->left, pair, !dimension);
        min_node = backtrack_min(min_node, root->right, pair, !dimension);
        return min_node;
    }

    //   回溯
    static const Node *backtrack(const Node *min_node, const Node* root, const KVPair &pair, const bool dimension) {
        if (root == nullptr) {
            return min_node;
        }
        if (min_node == nullptr) {
            return root;
        }

        const T search = get_dimension_value(pair, dimension);
        const T curr = get_dimension_value(root->data, dimension);

        const auto is_left = search < curr;
        const auto next_search = is_left ? root->right : root->left;

        auto radius = get_distance(min_node, pair);
        const auto boundary_distance = get_boundary_distance(next_search, pair, dimension);
        const auto new_radius = get_distance(root, pair);

        if (new_radius < radius) {
            min_node = root;
        }
        min_node = backtrack_min(min_node, next_search, pair, !dimension);
        return min_node;
    }

    const Node *find_min(const Node *root, const KVPair &pair, bool dimension) {
        if (root == nullptr) {
            return nullptr;
        }


        const T search = get_dimension_value(pair, dimension);
        const T curr = get_dimension_value(root->data, dimension);

        const Node *min_node = nullptr;
        if (search < curr) {
            min_node = find_min(root->left, pair, !dimension);
        } else {
            min_node = find_min(root->right, pair, !dimension);
        }

        min_node = backtrack(min_node, root, pair, dimension);
        return min_node;
    }

public:
    explicit KDTree2D(std::vector<Pair> pairs) {
        // 从第一个维度开始
        root_ = buildTreeRecursive(pairs, 0, pairs.size(), false);
    }

    V find_closest(const T t, const K k) {
        const KVPair kv{t, k};

        const auto min = find_min(root_, kv, false);
        if (min == nullptr) {
            throw std::runtime_error("No such node");
        }

        return min->data.value;

    }

    void dlr_print() {
        dlr_recursion(root_, [](const Node *root) {
            const auto &[first, second, value] = root->data;
            // std::cout << first << "\t" << second << "\t" << value << std::endl;
        });
    }

    void ldr_print() {
        ldr_recursion(root_, [](const Node *root) {
            const auto &[first, second, value] = root->data;
            qDebug() << first << "\t" << second << "\t" << value;
        });
    }

    ~KDTree2D() {
        dlr_recursion(this->root_, [] (const Node *node) {delete node;});
    }

private:
    Node *root_;

};





#endif //KDNODE2D_H
