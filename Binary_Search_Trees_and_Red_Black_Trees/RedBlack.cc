#include <ostream>
#include <BST.h>

namespace internal {
    /**
     * enum class abstracting the nodes' color
     */
    enum class Color {red, black};
}

/**
 * Red black tree class. It inherits the public and protected members from the BST class. In addition,
 * we use an array of Color for the nodes' colors, such that array[key] = color_of_node_indexed_by_key.
 * Of course, this limits us to integral-like keys, but turned out to be the only feasible solution.
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

    int size;  // length of the colors array
    Color* colors; // array to store the colors of the nodes
    /**
     * utility function to reallocate the colors array if a key >= size appears
     */
    void check_and_alloc(const key_type& key) {
        if (size > key) return;
        Color* new_colors{new Color[key + 1]};
        for (int i=0; i < size; ++i) {
            new_colors[i] = colors[i];
        }
        colors = std::move(new_colors);
        size = key + 1;
    }
    /**
     * inserts a key, value pair just like in a BST, but with 'coloring' the node.
     * In addition, the function returns a pointer to the node
     */
    node_type* BSTinsert(const key_type& key, const value_type& value) {
        if (base::root == nullptr) { //check if the BST is empty
            base::root = new node_type{key, value, nullptr};
            check_and_alloc(key);
            colors[key] = Color::black;  // black color the root
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
        check_and_alloc(key);
        colors[key] = Color::red; // red color it
        return child;
    }
    /**
     * Fix the tree as in case 1.
     */
    node_type* fix_case1(node_type* z) {
        // black color z's uncle and parent black
        colors[base::uncle(z)->data.first] = Color::black;
        colors[z->parent->data.first] = Color::black;
        // red color z's grandpa, new z is z's grandpa
        node_type* grand = base::grandparent(z);
        colors[grand->data.first] = Color::red;
        return grand;
    }
    /**
     * Fix the tree as in case 2
     */
    node_type* fix_case2(node_type* z) {
        // rotate left on z's parent, new z is z's parent
        node_type* p = z->parent;
        base::left_rotate(p);
        return p;
    }
    /**
     * Fix the tree as in case 3. Notice the function does not return any node,
     * since we are assured it will solve the problem
     */
    void fix_case3(node_type* z) {
        // invert z's parent and grandpa colors
        node_type* grand = base::grandparent(z);
        colors[z->parent->data.first] = Color::black;
        colors[grand->data.first] = Color::red;
        // rotate on z's grandpa
        if (base::is_right_child(z)) {
            base::right_rotate(grand);
        }
        else {
            base::left_rotate(grand);
        }
    }

  public:
    /**
     * Default constructor. Delegates the base constructor, initializes size to 0 and
     * allocates memory for the colors' array
     */
    RedBlackTree() : base::BST{}, size{0}, colors{new Color[size]} {}
    /**
     * Insert into the Red-Black tree a key, value pair, following the algorithm explained in class
     */
    void insert(const key_type& key, const value_type& value) override {
        // return pointer to the node as if it were a BST
        node_type* curr = BSTinsert(key, value);
        if (curr->parent == nullptr) return; // if the node is the root, the tree is trivially balanced
        else if (base::uncle(curr) == nullptr) {  // if the node has no uncle, either the parent or grandparent is the root, then black color it
            colors[curr->data.first] = Color::black;
            return;
        }
        // if the node has an uncle, we can always choose among cases 1, 2, 3
        while (curr->parent != nullptr && base::grandparent(curr) != nullptr && colors[curr->parent->data.first] == Color::red) {
            switch(colors[base::uncle(curr)->data.first]) {
                case Color::red :
                    // CASE 1: uncle is red
                    // removes the problem or pushes towards the root
                    curr = fix_case1(curr);
                case Color::black :
                    if (base::is_right_child(curr) && base::is_right_child(base::uncle(curr))) {
                        // CASE 2: uncle is black, uncle and curr are right children
                        // brings to case 3
                        curr = fix_case2(curr);
                    }
                    // CASE 3: uncle is black, uncle and curr are right and left children
                    // solves the problem
                    fix_case3(curr);
                default : break;
            }
        }
        // make sure second property is not violated
        colors[base::root->data.first] = Color::black;
    }
    /**
     * Default destructor
     */
    ~RedBlackTree() = default;
};

// REMEMBER NULLPTR LEAVES ACCOUNT AS BLACK NODES
int main() {
    //BST<int,double> bst{};
    RedBlackTree<int,double> rbt{};
    for (auto elem : {6, 5, 4, 1, 2, 7, 8, 3, 9}) {
        //std::cout << "#####" << std::endl;
        rbt.insert(elem, 0.0);
        //std::cout << rbt;
    }
    //RedBlackTree<int,double> rbt{};
    //internal::RedBlackNode<int,double> dummy{1, 0.0, nullptr, internal::Color::red};
    //std::cout << dummy.data.first << " " << dummy.data.second << " " << std::endl;
    //bst.remove(9);
    std::cout << rbt;
    //bst.test_rotate();
    //bst.in_order_walk();
    //std::cout << bst;
    return 0;
}
