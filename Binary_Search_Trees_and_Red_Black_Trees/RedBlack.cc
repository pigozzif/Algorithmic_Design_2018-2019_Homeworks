#include <ostream>
#include <unordered_map>
#include "BST.h"

namespace internal {
    /**
     * enum class abstracting the nodes' color
     */
    enum class Color {red, black};
}

/**
 * Red-black tree class. It inherits the public and protected members from the BST class. In addition,
 * we use an array of Color for the nodes' colors, such that array[key] = color_of_node_indexed_by_key.
 * Notice the search routine required by the assignment can be safely seen as the 'find' function
 * inherited from the BST.
 */
template<class K, class V, class Comp = std::less<K>>
class RedBlackTree : public BST<K,V,Comp> {
    // aliases, for convenience
    using base = BST<K,V,Comp>;
    using node_type = typename base::node_type;
    using key_type = typename base::key_type;
    using value_type = typename base::value_type;
    using pair_type = typename base::pair_type;
    using Color = internal::Color;

    std::unordered_map<K, Color> colors;  // to store the colors of the nodes; supports fast lookup, constant in the average case
    /**
     * Helper function to return the color of a given node.
     * Abstracts from the color management mechanism.
     * Returning by reference allows us to change the color
     */
    Color& color(node_type* x) noexcept {
        return colors[x->data.first];
    }
    /**
     * inserts a key, value pair just like in a BST, but with 'coloring' the node.
     * In addition, the function returns a pointer to the node
     */
    node_type* BSTinsert(const key_type& key, const value_type& value) {
        if (base::root == nullptr) { //check if the BST is empty
            base::root = new node_type{key, value, nullptr};
            color(base::root) = Color::black;  // black color the root
            return base::root;
        }
        node_type* previous_node{base::root}; //initialize previous node to root
        node_type* current_node{base::root}; //initialize also the current node ptr to root
        while (current_node) {
            key_type current_key{current_node->data.first};
            if (!base::compare(current_key, key) && !base::compare(key, current_key)) { //if the key is already in the tree update the value
                current_node->data.second = value;
                return current_node;
            }
            else if (base::compare(key, current_key)) { // if the new key is smaller go to left subtree
                previous_node = current_node;
                current_node = current_node->left_child;
            }
            else {    //if new key is bigger go to in the right subtree
                previous_node = current_node;
                current_node = current_node->right_child;
            }
        }
        // create new node and update parent
        node_type* child = new node_type{key, value, previous_node};
        if (base::compare(key, previous_node->data.first)) {
            previous_node->left_child = child;
        }
        else {
            previous_node->right_child = child;
        }
        color(child) = Color::red; // red color the new node
        return child;
    }
    /**
     * Fix the tree as in case 1
     */
    node_type* fix_case1(node_type* z) {
        // color z's uncle and parent black
        color(base::uncle(z)) = Color::black;
        color(z->parent) = Color::black;
        // red color z's grandpa, new z is z's grandpa
        node_type* grand = base::grandparent(z);
        color(grand) = Color::red;
        return grand;
    }
    /**
     * Fix the tree as in case 2
     */
    node_type* fix_case2(node_type* z, node_type* p) {
        // rotate left on z's parent, if z and uncle are left children
        if (base::is_right_child(z) && !base::is_right_child(p)) {  // if parent is not right child, uncle must be right child
            base::left_rotate(p);
        }
        // rotate right on z's parent, if z and uncle are right children
        else if (!base::is_right_child(z) && base::is_right_child(p)) {  // is parent is right child, uncle must be left child
            base::right_rotate(p);
        }
        return p;
    }
    /**
     * Fix the tree as in case 3. Notice the function does not return any node,
     * since we are assured it will solve the problem
     */
    void fix_case3(node_type* z, node_type* p, node_type* g) {
        // rotate on z's grandpa g
        if (!base::is_right_child(z)) {
            base::right_rotate(g);
        }
        else {
            base::left_rotate(g);
        }
        // invert z's parent and grandpa colors
        Color temp{color(g)};
        color(p) = color(g);
        color(g) = temp;
    }

  public:
    void test(const key_type key, const value_type value) {
        insert(key, value);
        base::in_order_walk();
    }
    /**
     * Default constructor. Delegates the base constructor, initializes size to 0 and
     * allocates memory for the colors' array
     */
    RedBlackTree() : base::BST{}, colors{} {}
    /**
     * Insert into the Red-Black tree a key-value pair, following the algorithm explained in class.
     * Notice it overrides the parent class's corresponding function.
     */
    void insert(const key_type& key, const value_type& value) override {
        // return pointer to the node just added as if it were a BST
        node_type* curr = BSTinsert(key, value);
        while (true) {
            // if the node is the root, black color it and we are done
            if (curr->parent == nullptr) {
                color(curr) = Color::black;  // to preserve the second property
                return;
            }
            // if the node's parent is black: done
            else if (color(curr->parent) == Color::black) {
                return;
            }
            else if (base::uncle(curr) != nullptr && color(base::uncle(curr)) == Color::red) {
                // CASE 1: uncle is red
                // removes the problem or pushes towards the root
                curr = fix_case1(curr);
            }
            else {
                // CASE 2: uncle is black, uncle and curr are both right or left children
                // brings to case 3
                curr = fix_case2(curr, curr->parent);
                // CASE 3: uncle is black, uncle and curr are right and left children
                // solves the problem
                fix_case3(curr, curr->parent, base::grandparent(curr));
            }
        }
    }
    /**
      * Fix the tree while removing as in case 1
      */
    node_type* fix_remove_case1(node_type* x, node_type* sibling) {
        // invert colors between x's sibling and x's parent
        Color temp{color(x->parent)};
        color(x->parent) = color(sibling);
        color(sibling) = temp;
        // rotate x's parent on x's side
        if (base::is_right_child(x)) {
             base::right_rotate(x->parent);
        }
        else {
            base::left_rotate(x->parent);
        }
        return x;
    }
    /**
      * Fix the tree while removing as in case 3, when the node x is a right child
      */
    node_type* fix_remove_case3_right(node_type* x, node_type* sibling) {
        // rotate x's sibling on the opposite side with respect to x
        base::left_rotate(sibling);
        // invert colors in both old and new sibling of x
        color(sibling) = Color::red;  // we know sibling is black
        // invert new sibling's color using a switch-case statement
        switch(color(base::sibling(x))) {
            case Color::red :
                color(base::sibling(x)) = Color::black;
            case Color::black :
                color(base::sibling(x)) = Color::red;
            default :
                break;
        }
        return x;
    }
    /**
      * Fix the tree while removing as in case 3, when the node x is a left child
      */
    node_type* fix_remove_case3_left(node_type* x, node_type* sibling) {
        // rotate x's sibling on the opposite side with respect to x
        base::left_rotate(sibling);
        // invert colors in both old and new sibling of x
        color(sibling) = Color::red;  // we know sibling is black
        // invert new sibling's color using a switch-case statement
        switch(color(base::sibling(x))) {
            case Color::red :
                color(base::sibling(x)) = Color::black;
            case Color::black :
                color(base::sibling(x)) = Color::red;
            default :
                break;
        }
        return x;
    }
    /**
      * Fix the tree while removing as in case 4, when the node x is a right child. Notice
      * we do not return a node, since case 4 is guaranteed to fix the tree
      */
    void fix_remove_case4_right(node_type* x, node_type* sibling, node_type* left_nephew) {
        // switch colors between x's sibling and parent
        Color temp{color(x->parent)};
        color(x->parent) = color(sibling);
        color(sibling) = temp;
        // black color the x's nephew on the opposide side with respect to x
        color(left_nephew) = Color::black;
        // rotate x's parent on x side
        base::right_rotate(x->parent);
    }
    /**
      * Fix the tree while removing as in case 4, when the node x is a left child. Notice
      * we do not return a node, since case 4 is guaranteed to fix the tree
      */
    void fix_remove_case4_left(node_type* x, node_type* sibling, node_type* right_nephew) {
        // switch colors between x's sibling and parent
        Color temp{color(x->parent)};
        color(x->parent) = color(sibling);
        color(sibling) = temp;
        // black color the x's nephew on the opposide side with respect to x
        color(right_nephew) = Color::black;
        // rotate x's parent on x side
        base::left_rotate(x->parent);
    }
    /**
      * Remove from a red-black tree the node having key 'key', following the algorithm
      * explained in class. Notice it overrides the parent class's corresponding function.
      */
    node_type* remove(const key_type& key) override {
        // first look for the key
        typename base::iterator z{base::find(key)};
        // if the key is not in the tree, simply return
        if (z == base::end()) {
            return nullptr;
        }
        node_type* y{&(*z)};
        // call the parent class's auxiliary remove and return pointer to the node
        // that substituted y
        node_type* x = base::remove_aux(y);
        // if y was red, the red-black tree properties are preserved
        if (color(y) == Color::red) {
            return y;
        }
        // y is black, the branches through x lost one black node
        else {
            node_type* sibling = base::sibling(x);
            // find the nephews of x
            node_type* right_nephew = sibling->right_child;
            node_type* left_nephew = sibling->left_child;
            while (x != base::root && color(x) != Color::red) {
                if (color(x) == Color::red) {
                    // CASE 0: x is red. Fixes the tree
                    // black color x; we increase by one the black height of its subtree and we are done
                    color(x) = Color::black;
                }
                else if (color(sibling) == Color::red) {
                    // CASE 1: x's sibling is red. Cannot occur twice in a row
                    x = fix_remove_case1(x, sibling);
                    // update sibling
                    sibling = x->parent->right_child;
                }
                else if (color(sibling) == Color::black) {
                    if (base::is_right_child(x) && color(left_nephew) == Color::red) {
                        // CASE 4: x's nephew on the opposite side with respect to x is red. Fixes the tree
                        fix_remove_case4_right(x, sibling, left_nephew);
                        break;
                    }
                    else if (!base::is_right_child(x) && color(right_nephew) == Color::red) {
                        // CASE 4: same as above but on the other side. Fixes the tree
                        fix_remove_case4_left(x, sibling, right_nephew);
                        break;
                    }
                    else if (base::is_right_child(x) && color(right_nephew) == Color::red) {
                        // CASE 3: among x's sibling and nephews, only the nephew on the x's side is red. Brings to case 4
                        x = fix_remove_case3_right(x, sibling);
                        // update sibling
                        sibling = x->parent->left_child;
                    }
                    else if (!base::is_right_child(x) && color(left_nephew) == Color::red) {
                        // CASE 3: same as above, but on the other side. Brings to case 4
                        x = fix_remove_case3_left(x, sibling);
                        // update sibling
                        sibling = x->parent->right_child;
                    }
                    else {
                        // CASE 2: x's sibling and nephews are black. Pushes the problem one step towards the root
                        // red color x's sibling
                        color(sibling) = Color::red;
                        x = x->parent;
                    }
                }
            }
        }
        // return the removed node
        return y;
    }
    /**
     * Default destructor
     */
    ~RedBlackTree() = default;
};

int main() {
    return 0;
}
