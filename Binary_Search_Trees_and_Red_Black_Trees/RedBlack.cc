#include <ostream>
#include <BST.h>

namespace internal {
    /**
     * enum class abstracting the nodes' color
     */
    enum class Color {red, black};
}

/**
 * Red-black tree class. It inherits the public and protected members from the BST class. In addition,
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
     * Helper function to return the color of a given node
     */
    Color get_color(node_type* x) const noexcept {
        if (x == nullptr) {
            return Color::black;  // by convention, red-black null leaves are black
        }
        return colors[x->data.first];
    }
    /**
     * Helper function to 'recolor' a node
     */
    void set_color(node_type* x, Color new_color) {
        colors[x->data.first] = new_color;
    }
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
        // color z's uncle and parent black
        set_color(base::uncle(z), Color::black);
        set_color(z->parent, Color::black);
        // red color z's grandpa, new z is z's grandpa
        node_type* grand = base::grandparent(z);
        set_color(grand, Color::red);
        return grand;
    }
    /**
     * Fix the tree as in case 2
     */
    node_type* fix_case2(node_type* z, node_type* p) {
        // rotate left on z's parent
        if (base::is_right_child(z) && !base::is_right_child(p)) {  // if parent is not right child, uncle must be right child
            std::cout << "case 3, right and left" << std::endl;
            base::left_rotate(p);
            //z = z->left_child;
        }
        else if (!base::is_right_child(z) && base::is_right_child(p)) {  // is parent is right child, uncle must be left child
            std::cout << "case 3, left and right" << std::endl;
            base::right_rotate(p);
            //z = z->right_child;
        }
        return p;//z;
    }
    /**
     * Fix the tree as in case 3. Notice the function does not return any node,
     * since we are assured it will solve the problem
     */
    void fix_case3(node_type* z, node_type* p, node_type* g) {
        // rotate on z's grandpa
        if (!base::is_right_child(z)) {
            std::cout << "case 4, left" << std::endl;
            base::right_rotate(g);
        }
        else {
            std::cout << "case 4, right" << std::endl;
            base::left_rotate(g);
        }
        // invert z's parent and grandpa colors
        Color temp{color(g)};
        set_color(p, color(g));
        set_color(g, temp);
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
    RedBlackTree() : base::BST{}, size{0}, colors{new Color[size]} {}
    /**
     * Insert into the Red-Black tree a key-value pair, following the algorithm explained in class
     */
    void insert(const key_type& key, const value_type& value) override {
        // return pointer to the node as if it were a BST
        node_type* curr = BSTinsert(key, value);
        while (true) {
            // if the node is the root, black color it and we are done
            if (curr->parent == nullptr) {
                std::cout << "case 0" << std::endl;
                set_color(curr, Color::black);  // to preserve the second property
                return;
            }
            // if the node's parent is black: done
            else if (color(curr) == Color::black) {
                std::cout << "case 1" << std::endl;
                return;
            }
            else if (base::uncle(curr) != nullptr && color(base::uncle(curr)) == Color::red) {
                std::cout << "case 2" << std::endl;
                // CASE 1: uncle is red
                // removes the problem or pushes towards the root
                curr = fix_case1(curr);
            }
            else {
                //std::cout << "case 3" << std::endl;
                node_type* p = curr->parent;
                node_type* g = base::grandparent(curr);
                // CASE 2: uncle is black, uncle and curr are both right or left children
                // brings to case 3
                curr = fix_case2(curr, p);
                // CASE 3: uncle is black, uncle and curr are right and left children
                // solves the problem
                fix_case3(curr, p, g);
            }
        }
    }
    void delete_case1(node_type* x) {
        if (x->parent != nullptr) {
            node_type* s = base::sibling(x);
            if (color(s) == Color::red) {
                set_color(x->parent, Color::red);
                set_color(sibling, Color::black);
            }
            if (!base::is_right_child(x)) {
                base::left_rotate(x->parent);
            }
            else {
                base::right_rotate(x->parent);
            }
        }
        delete_case2(x, s);
    }
    void delete_case2(node_type* x, node_type* s) {
        if ((color(x->parent) == Color::black) && (color(s) == Color::black) &&
      (color(s->left_child) == Color::black) && (color(s->right_child) == Color::black)) {
            set_color(s, Color::red);
            delete_case1(x->parent);
        }
        else {
            delete_case3(x, s);
        }
    }
    void delete_case3(node_type* x, node_type* s) {
      if ((color(x->parent) == Color::red) && (color(s) == Color::black) &&
    (color(s->left_child) == Color::black) && (color(s->right_child) == Color::black)) {
          set_color(s, Color::red);
          set_color(x->parent, Color::black);
      }
      else {
          delete_case4(x, s);
      }
    }
    void delete_case4(node_type* x, node_type* s) {
        if (color(s) == Color::black) {
            if ((!base::is_right_child(x)) && (color(s->right_child) == Color::black)
              && (color(s->left_child) == Color::red)) {
                set_color(s, Color::red);
                set_color(s->left_child, Color::black);
                base::right_rotate(s);
            }
            else if ((base::is_right_child(x)) && (color(s->left_child) == Color::black)
              && (color(s->right_child) == Color::red)) {
                set_color(s, Color::red);
                set_color(s->right_child, Color::black);
                base::left_rotate(s);
            }
        }
        delete_case5(x, s);
    }
    void delete_case5(node_type* x, node_type* s) {
        set_color(s, color(x->parent));
        set_color(x->parent, Color::black);
        if (!base::is_right_child(x)) {
            set_color(s->right_child, Color::black);
            base::left_rotate(x->parent);
        }
        else {
            set_color(s->left_child, Color::black);
            base::right_rotate(x->parent);
        }
    }
    void remove(const key_type& key) override {
        typename base::iterator z{base::find(key)};
        // if the key is not in the tree, simply return
        if (z == base::end()) {
            return;
        }
        node_type* y{&(*z)};
        node_type* x = base::remove_aux(y);  // call the auxiliary remove and return pointer to 'substitute'
        if (color(y) == Color::black) {
            if (color(x) == Color::red) {
                set_color(x, Color::black);
            }
            else {
                delete_case1(x);
            }
        }
        if (x == nullptr) {return;}
        std::cout << "passed it!" << std::endl;
        switch(colors[y->data.first]) {
            case Color::red :
                return;
            case Color::black :
                if (colors[x->data.first] == Color::red) {
                    colors[x->data.first] = Color::black;
                    return;
                }
                else {
                    bool x_is_right = base::is_right_child(x);
                    node_type* sibling = (x_is_right) ? x->parent->left_child : x->parent->right_child;
                    if (colors[sibling->data.first] == Color::red) {
                        colors[sibling->data.first] = Color::black;
                        colors[x->parent->data.first] = (colors[x->parent->data.first] == Color::red) ? Color::black : Color::red;
                        if (base::is_right_child(x)) {
                            base::right_rotate(x->parent);
                        }
                        else {
                            base::left_rotate(x->parent);
                        }
                    }
                    else if (colors[sibling->data.first] == Color::black && (sibling->left_child != nullptr || colors[sibling->left_child->data.first] == Color::black) && (sibling->right_child != nullptr || colors[sibling->right_child->data.first] == Color::black)) {
                        colors[sibling->data.first] = Color::red;
                    }
                    else if (colors[sibling->data.first] == Color::black) {
                        if ((x_is_right && colors[sibling->right_child->data.first] == Color::red && colors[sibling->left_child->data.first] == Color::black) || (!x_is_right && colors[sibling->left_child->data.first] == Color::red && colors[sibling->right_child->data.first] == Color::black)) {
                            if (x_is_right) {
                                base::left_rotate(sibling);
                            }
                            else {
                                base::right_rotate(sibling);
                            }
                            colors[sibling->data.first] = Color::red;
                            node_type* new_sibling = (x_is_right) ? x->parent->left_child : x->parent->right_child;
                            colors[new_sibling->data.first] = (colors[new_sibling->data.first] == Color::red) ? Color::black : Color::red;
                        }
                        else {
                            colors[sibling->data.first] = Color::red;
                            colors[x->parent->data.first] = (colors[x->parent->data.first] == Color::red) ? Color::black : Color::red;
                            if (x_is_right) {
                                colors[sibling->left_child->data.first] = Color::black;
                                base::right_rotate(x->parent);
                            }
                            else {
                                colors[sibling->right_child->data.first] = Color::black;
                                base::left_rotate(x->parent);
                            }
                        }
                    }
                }
            default : break;
        }
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
    for (auto elem : {6, 2, 1, 4, 3, 5}) {//{6, 5, 4, 1, 2, 7, 8, 3, 9}) {
        //std::cout << "#####" << std::endl;
        rbt.insert(elem, 0.0);
        //std::cout << rbt;
    }
    //RedBlackTree<int,double> rbt{};
    //internal::RedBlackNode<int,double> dummy{1, 0.0, nullptr, internal::Color::red};
    //std::cout << dummy.data.first << " " << dummy.data.second << " " << std::endl;
    //for (auto elem : {6, 5, 4, 1, 2, 7, 8, 3, 9}) {
    //    rbt.remove(elem);
    //}
    rbt.remove(5);
    std::cout << rbt;
    //rbt.test_rotate();
    //bst.in_order_walk();
    //std::cout << bst;
    return 0;
}
