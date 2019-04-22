#ifndef __BST_H__
#define __BST_H__


#include <functional>
#include <utility>
#include <memory>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <iterator>
#include <initializer_list>


template<class K, class V, class Comp = std::less<K>>
class BST {

    public:
	  //!Alias for the type of keys in the tree
	  using key_type = K;
	  //!Alias for the type of values associated to keys in the tree
	  using value_type = V;
	  //!Alias for the key-value pairs stored in the BST. The key is declared as const to prevent it from being changed once in the tree.
	  using pair_type = std::pair<K, V>;

    private:
    // node struct
    struct BST_node {

     //! Pointers to left and right child of the node
     std::unique_ptr<BST_node> left_child, right_child;
     //! Pointer to the parent of this node
     BST_node* parent;
     //! Key-value pair stored in the node
     pair_type data;

     /**
      * Default constructor for BST_node
      */
     BST_node() = delete;    //delete default constructor
     /**
      * Create a new node having the given data stored in it and the given parent
      * @param key key of the key-value pair to store in the node
      * @param value value of the key-value pair to store in the node
      * @param father pointer to the parent of the node
      */
     BST_node(const key_type key, const value_type value, BST_node* father)
      : left_child{nullptr}, right_child{nullptr}, parent{father}, data{key, value}
     {}
     /**
      * Copy constructor
      */
     BST_node(const BST_node& other) : left_child{other.left_child.get()}, right_child{other.right_child.get()},
     parent{other.parent}, data{other.data.first, other.data.second} {}
     /**
      * Find the successor of a node
      */
     BST_node* find_successor() const {
         BST_node* current = this;
         if (right_child != nullptr) {    //if current node has right child, go down right and then as much to the left as possible
           current = current->right_child.get();
           while (current->left_child.get()) {
               current = current->left_child.get();
           }
           return current;
         }
         else {     //otherwise, go up until you reach the root or you find a node that is not the right child of its parent
           BST_node* p = current->parent;
           while (p != nullptr && current == p->right_child.get()) {
               current = p;
               p = p->parent;
           }
           current = p;
           return current;
     }
     /**
      * Default destructor for nodes
      */
     ~BST_node() noexcept = default;
    };

     using node_type = BST_node;//This alias is left private since nodes are not intendend for user usage.
     //!Pointer to the root node of the BST
     std::unique_ptr<node_type> root;
     //!Function object defining the comparison criteria for key_type objects.
     Comp compare;

     struct BST_iterator {
             node_type* current;
             /**
              * Constructor
              * @param n the node to initialize current to
              */
             BST_iterator(node_type* n) : current{n} {}
             /**
              * dereferencing operator, returns an std::pair<const K, V> of the current node
              */
             pair_type& operator*() const {return current->data;}
             /**
              * pre-increment operator. Implements algorithm for retrieving
              * the successor of a node.
              */
             BST_iterator& operator++() {
                 current = current.find_successor();
                 return *this;
     	       }
             /**
              * Comparison operators
              */
             bool operator==(const BST_iterator& other) {return current == other.current;} //tests whether two iterators share the same current node
             bool operator!=(const BST_iterator& other) {return !(*this==other);}
     };
     using iterator = BST_iterator;
	   /**
	    * Utility function to insert in the BST a full subtree. Elements of the subtree are
	    * inserted in the BST starting at the root and then recursively inserting the
	    * left and right subtrees.
	    * @param subtree to copy into the BST
	    */
	    void insert( const node_type& subtree);
	   /**
      * Return a pointer to the node having the smallest key.
      */
      node_type* get_min(node_type* current=nullptr) const noexcept;
     /**
      * Transplant function to replace x by y
      * @param x, the node to be replace
      * @param y, the node to substitue
      */
      void transplant(node_type& x, node_type& y);
      /**
       * "Family" relations among nodes
       * @param x the node under discussion
       */
       bool is_right_child(const node_type& x) const noexcept {return x.parent != nullptr && x.parent->right_child == x;}
       node_type* grandparent(const node_type& x) const noexcept {return x.parent->parent;}
       node_type* uncle(const node_type& x) const noexcept {
          if (is_right_child(*(x.parent))) {
              return grandparent(x).left_child.get();
          }
          else {
              return grandparent(x).right_child.get();
          }
      }

    public:

	    /**
	     * Create an empty BST. The root pointer is set to nullptr and the compare function is
	     * default initialized.
	     */
	    BST() = default;
	    /**
	     * Default destructor
	     */
	    ~BST() noexcept = default;

      /**
       * Returns an iterator to the node having a key equal to the input key, end()
       * if it is not found. Moves down the tree exploiting the ordering of the keys.
       * @param key the sought-after key
       */
      iterator find(const key_type key) const noexcept;
      /**
       * non-const begin and end functions. Allow the BST to support range for-loops.
       * begin returns an iterator to the node having the smallest key
       */
      iterator begin() noexcept {return iterator{get_min()};}
      /**
       * end returns an iterator to nullptr
       */
      iterator end() noexcept {return iterator{nullptr};}
	    /**
	     * Insert a key-value pair in the BST composed by the given key and value.
	     * @param key the key in the pair
	     * @param value the value in the pair
	     */
	     void insert(const key_type& key, const value_type& value);
	     /**
	      * Insert a key-value pair in the BST.
	      * @param pair the key-value pair to insert
	      */
	     void insert(const pair_type& pair){
           insert(pair.first, pair.second);
	     }
       /**
        * Remove a key-value pair from the BST.
        * @param key the key to use for the deletion
        */
       node_type& remove(const key_type& key);
	     /**
	      * Remove all key-value pairs from the BST.
	      */
	     void clear() noexcept {
	         root.reset(nullptr);
	     }
	     /**
        * Overload of the operator[], in const and non-const version
        */
	     value_type& operator[] (const key_type&);
	     const value_type& operator[] (const key_type&) const;
};

/*
 * get_min function
 */
template<class K, class V, class Comp>
typename BST<K,V,Comp>::node_type* BST<K,V,Comp>::get_min(node_type* current) const noexcept {
    if (current == nullptr) { // by default start from the root
        current = root.get();
    }
    if (current == nullptr) return nullptr;  //if the tree is empty, return nullptr
    while (current->left_child.get()) {   //do down to the left as much as possible
        current = current->left_child.get();
    }
    return current;
}

/*
 * transplant function
 */
template<class K, class V, class Comp>
void BST<K,V,Comp>::transplant(node_type& x, node_type& y) {
    if (x.parent == nullptr) {  // x is the root
        root.reset(&y);
    }
    else {
        if (is_right_child(x)) {
            x.parent->right = y;
        }
        else {
            x.parent->left = y;
        }
    }
    if (y != nullptr) {
        y.parent = x.parent;
    }
}

/*
 * find function
 */
template<class K, class V, class Comp>
typename BST<K,V,Comp>::iterator BST<K,V,Comp>::find(const key_type key) const noexcept {
    node_type* current{root.get()};
    while (current) {
        key_type curr_key = current->data.first;
        if (!compare(curr_key, key) && !compare(key, curr_key)) {   //if current node has sought-after key, return an iterator to it
            return iterator{current};
        }
        else if (compare(key, curr_key)) {    //if greater, proceed in the left subtree
            current = current->left_child.get();
        }
        else {    //if smaller, proceed in the right subtree
            current = current->right_child.get();
        }
    }
    return end();    //if not found, return end
}

/*
 * insert function (key_type, value_type version)
 */
template<class K, class V, class Comp>
void BST<K,V,Comp>::insert(const key_type& key, const value_type& value){

    if (root == nullptr){ //check if the BST is empty
	root.reset(new node_type{key, value, nullptr});
	return;
    }

    node_type *previous_node{root.get()}; //initialize previous node to root
    node_type *current_node{root.get()}; //initilize also the current node ptr to root
    while (current_node) {

	key_type current_key{current_node->data.first};
        if (!compare(current_key, key) && !compare(key, current_key)) { //if the key is already in the tree update the value
	    current_node->data.second = value;
	    return;
        }
        else if (compare(key, current_key)) { // if the new key is smaller go to left subtree
	    previous_node = current_node;
            current_node = current_node->left_child.get();
        }
        else {    //if new key is bigger go to in the right subtree
	    previous_node = current_node;
            current_node = current_node->right_child.get();
        }
    }
    auto& child = (compare(key, previous_node->data.first)) ? previous_node->left_child : previous_node->right_child;
    child.reset(new node_type{key, value, previous_node});
}

/*
 * insert function (node_type version)
 */
template<class K, class V, class Comp>
void BST<K,V,Comp>::insert( const node_type& subtree){

    insert(subtree.data); //copy data in target to the new tree
    if (subtree.left_child)
	  insert(*subtree.left_child); //copy left subtree
    if (subtree.right_child)
	  insert(*subtree.right_child); //copy right subtree
}

/*
 * Delete function
 */

template<class K, class V, class Comp>
typename BST<K,V,Comp>::node_type& BST<K,V,Comp>::remove(const key_type& key) {
    iterator z{find(key)};
    if (z != end()) {  // find() returns an iterator to nullptr if key not present
        throw std::invalid_argument("Deleting absent element");
    }
    node_type curr{*(z.current)};
    if (curr.left_child == nullptr) {  // current node lacks left child
        transplant(curr, *(curr.right_child.get()));
        return curr;
    }
    if (curr.right_child == nullptr) {  // current node lacks right child
        transplant(curr, *(curr.left_child.get()));
        return curr;
    }
    node_type* y{get_min(&curr)};  // y = minimum in subtree rooted at curr
    curr.data = y->data;
    return remove(*y);  // y lacks of left child
}

/**
 * Overload of operator[] for BSTs, non-const version
 */
template<class K, class V, class Comp>
typename BST<K,V,Comp>::value_type& BST<K,V,Comp>::operator[](const key_type& arg_key) {

    iterator iter = find(arg_key);
    if (iter != end())
	return (*iter).second;
    insert(pair_type{arg_key, value_type{}});
    return (*find(arg_key)).second;
}

/**
 * Overload of operator[] for BSTs, const version
 */
template<class K, class V, class Comp>
const typename BST<K,V,Comp>::value_type& BST<K,V,Comp>::operator[](const key_type& arg_key) const {
    iterator iter = find(arg_key);
    if (iter != end()) {
        return (*iter).second;
    }
    throw std::out_of_range{"const operator[] trying to access key not present in given BST"};
}

/**
 * Overload of the operator<< for BSTs, allows to print
 * the key: value pairs of the tree in-order.
 */
template<class K, class V, class Comp>
std::ostream& operator<<(std::ostream& os, const BST<K,V,Comp>& tree) {
    for (const auto& x : tree) {
        os << x.first << ": " << x.second << std::endl;    //iterate in order and print the key: value pairs
    }
    return os;
}

#endif
