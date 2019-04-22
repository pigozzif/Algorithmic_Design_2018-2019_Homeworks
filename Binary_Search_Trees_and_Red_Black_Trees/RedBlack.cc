#include <ostream>
#include <BST2.h>

namespace internal {
    /**
     * enum class abstracting the nodes' color
     */
    enum class Color {red, black};
    /**
     * RedBlackNode struct, inherits from the BST_node struct and
     * extends it by adding a 'color' member
     */
    template<class K, class V>
    struct RedBlackNode : public BST_node<K,V> {
        // redefine aliases
        using pair_type=typename BST<K,V>::pair_type;
        using key_type=typename BST<K,V>::key_type;
        using value_type=typename BST<K,V>::value_type;
        using node_type=BST_node<K,V>;

        Color color;
        /**
         * Custom constructor, delegates the 'father' constructor and initializes the color
         */
        RedBlackNode(const key_type key, const value_type value, node_type* father, Color col) :
            BST_node<K,V>::BST_node{key, value, father}, color{col} {}
        /**
         * Default destructor
         */
        ~RedBlackNode() = default;
    };
}

// REMEMBER NULLPTR LEAVES ACCOUNT AS BLACK NODES
int main() {
    BST<int,double> bst{};
    for (auto elem : {6, 5, 4, 1, 2, 7, 8, 3, 9}) {
        bst.insert(std::pair<int,double>(elem, 0.0));
    }
    //internal::RedBlackNode<int,double> dummy{1, 0.0, nullptr, internal::Color::red};
    //std::cout << dummy.data.first << " " << dummy.data.second << " " << std::endl;
    //bst.remove(9);
    //std::cout << bst;
    //bst.test_rotate();
    //bst.in_order_walk();
    //std::cout << bst;
    return 0;
}
