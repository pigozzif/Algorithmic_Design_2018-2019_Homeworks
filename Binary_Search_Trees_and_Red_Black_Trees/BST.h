/**
 * This header file includes an implementation of a binary search tree class as developed by Federico Julian
 * Verdù Camerota and Federico Pigozzi for their Advanced Programming exam at SISSA, winter 2019. With respect
 * to the original version, some details have been changed: 1) The 'Tester' class, copy and move semantics, the 'balance' procedure and their
 * utility functions have been dismissed; 2) A set of members has been moved from private to protected visibility, to
 * allow for inheritance in the RedBlackTree class (see RedBlack.cc file).
 * For what concerns upgrades, different functionalities have been added. In particular:
 * 1) A 'remove' and auxiliary functions have been implemented. You can find them at lines 190-216 and 319-331 respectively.
 * 2) A bunch of useful private functions, including 'family' relations ('uncle', 'grandparent', 'is_right_child'),
 *    right and left rotations, and the 'transplant' routine. You can find them all at lines 61-189.
 */

#ifndef __BST_H__
#define __BST_H__


#include <functional>
#include <utility>
#include <stdexcept>
#include <iostream>
#include <iterator>
#include <initializer_list>

namespace internal {
    /**
     * BST_Node struct, represents a node in a BST.
     */
    template <class K, class V>
    struct BST_node;
    /**
     * BST_iterator class, made compliant with the STL. Allows in-order traversal of BSTs.
     */
    template <class K, class V>
    class BST_iterator;
    /**
     *BST_const_iterator class. Allows iteration through const BSTs.
     */
    template <class K, class V>
    class BST_const_iterator;
}

template <class K, class V, class Comp = std::less<K>>
class BST{

 public:
  //!Alias for the type of keys in the tree
	using key_type = K;
	//!Alias for the type of values associated to keys in the tree
	using value_type = V;
	//!Alias for the key-value pairs stored in the BST. The key is declared as const to prevent it from being changed once in the tree.
	using pair_type = std::pair<K, V>;

protected:
  //!Alias for the node type
  using node_type = internal::BST_node<K,V>;//This alias is left private since nodes are not intendend for user usage.
	//!Pointer to the root node of the BST
	node_type* root;
	//!Function object defining the comparison criteria for key_type objects.
	Comp compare;
	/**
   * Transplant function to replace x by y
   * @param x, the node to be replaced
   * @param y, the node to perform the substitution
   */
  void transplant(node_type* x, node_type* y) {
      if (y != nullptr) {  // update y's parent, if any
          y->parent = x->parent;
      }
      if (x == root) {  // x is the root, update 'root' member
          root = y;
      }
      else {  // x has a parent, attach y in place of x
          if (is_right_child(x)) {
              x->parent->right_child = y;
          }
          else {
              x->parent->left_child = y;
          }
      }
  }
   /**
    * "Family" relations among nodes
    * @param x the node under discussion
    */
  bool is_right_child(node_type* x) const noexcept {
      return x != nullptr && x->parent != nullptr && x->parent->right_child == x;
  }
  node_type* grandparent(node_type* x) const noexcept {
      if (x->parent == nullptr) { // if x does not have a parent, it cannot have a grandparent
          return nullptr;
      }
      else {
          return x->parent->parent;
      }
  }
  node_type* uncle(node_type* x) const noexcept {
      node_type* grand = grandparent(x);
      if (grand == nullptr) {
          return nullptr; // if x does not have a grandparent, it cannot have an uncle
      }
      if (is_right_child(x->parent)) {
          return grand->left_child;
      }
      else {
          return grand->right_child;
      }
  }
  node_type* sibling(node_type* x) const noexcept {
      if (x->parent == nullptr) {  // no sibling if the node is the root
          return nullptr;
      }
      // otherwise, simply return the child of parent on the other side of x
      if (is_right_child(x)) {
          return x->parent->left_child;
      }
      else {
          return x->parent->right_child;
      }
  }
  /**
   * Perform a rotation to the left with pivot in
   * @param x, the node
   */
  void left_rotate(node_type* x) {
      if (x == nullptr) {
          return; // no sense to rotate empty subtree
      }
      else if (x->right_child == nullptr) {
          return;  // no sense to left-rotate node with no right child
      }
      // update beta and y
      node_type* y = x->right_child;
      node_type* beta = y->left_child;
      y->left_child = x;
      y->parent = x->parent;
      x->right_child = beta;
      if (beta != nullptr) {
          beta->parent = x;
      }
      // update original x's parent, if possible
      if (x->parent != nullptr) {
          if (is_right_child(x)) {
              x->parent->right_child = y;
          }
          else {
              x->parent->left_child = y;
          }
      }
      x->parent = y;
      // update new root, if necessary
      if  (x == root) {
          root = y;
      }
  }
  /**
   * Perform a rotation to the right with pivot in
   * @param y, the node
   */
   void right_rotate(node_type* y) {
       if (y == nullptr) {
           return; // no sense to rotate empty subtree
       }
       else if (y->left_child == nullptr) {
           return;  // no sense to right-rotate node with no left child
       }
       // update beta and x
       node_type* x = y->left_child;
       node_type* beta = x->right_child;
       x->right_child = y;
       x->parent = y->parent;
       y->left_child = beta;
       if (beta != nullptr) {
           beta->parent = y;
       }
       // update original y's parent, if possible
       if (y->parent != nullptr) {
           if (is_right_child(y)) {
               y->parent->right_child = x;
           }
           else {
               y->parent->left_child = x;
           }
       }
       y->parent = x;
       // update new root, if necessary
       if (y == root) {
           root = x;
       }
   }
   /**
    * Remove a node from the tree
    * @param curr the node to start the deletion algorithm from
    */
   node_type* remove_aux(node_type* curr) {
      // if curr has two children
      if (curr->left_child && curr->right_child) {
          node_type* successor = curr->find_successor();
          curr->data = successor->data;
          return remove_aux(successor);
      }
      // if curr has only the left child
      else if (curr->left_child) {
          transplant(curr, curr->left_child);
          return curr->left_child;
      }
      // if curr has only the right child
      else if (curr->right_child) {
          transplant(curr, curr->right_child);
          return curr->right_child;
      }
      // if curr is a leaf
      else {
          transplant(curr, nullptr);
          return nullptr;
      }
   }
 private:
   /**
   * Return a pointer to the node having the smallest key.
   */
   node_type* get_min(node_type* current=nullptr) const noexcept;
   /**
    * Auxiliary for the above routine
    */
   void in_order_walk_aux(node_type* x) {
       if (x != nullptr) {
           in_order_walk_aux(x->left_child);
           //std::cout << x->data.first << ": " << x->data.second << std::endl;
           std::cout << "I am " << x->data.first;
           if (x->parent != nullptr) {
               std::cout << ", I am son of " << x->parent->data.first;
           }
           else {
               std::cout << ", I am son of nobody ";
           }
           if (x->left_child != nullptr) {
                 std::cout << ", my left child is " << x->left_child->data.first;
           }
           else {
                 std::cout << ", my left child is nobody ";
           }
           if (x->right_child != nullptr) {
               std::cout << " and my right child is " << x->right_child->data.first << std::endl;
           }
           else {
               std::cout << " and my right child is nobody" << std::endl;
           }
           in_order_walk_aux(x->right_child);
       }
   }

 public:
	/**
	 * Create an empty BST. The root pointer is set to nullptr and the compare function is
	 * default initialized.
	 */
	BST() : root{nullptr}, compare{} {}
  /**
   * Create a BST from std::initializer_list, the compare function is default initialized, nodes
   * are added by repeatedly calling insert
   * @param args an std::initializer_list of std::pair<K,V>
   */
  BST(const std::initializer_list<std::pair<K,V>> args) : root{}, compare{} {
      for (const auto& x : args) insert(x);
  }
	/**
	 * Default destructor
	 */
	virtual ~BST() noexcept = default;
	//!Alias for iterators
	using iterator = internal::BST_iterator<K,V>;
	//!Alias for const iterators
	using const_iterator = internal::BST_const_iterator<K,V>;
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
   * const begin and end functions. begin returns a const_iterator to the node with smallest key
   */
  const_iterator begin() const noexcept {return const_iterator{get_min()};}
  /**
   * returns a const_iterator to nullptr
   */
  const_iterator end() const noexcept {return const_iterator{nullptr};}
  /**
   * cbegin and cend behave like const begin and const end, but can be useful to force an algorithm
   * of the STL to not modify input iterators. cbegin returns a const_iterator to the node with the smallest key
   */
  const_iterator cbegin() const noexcept {return const_iterator{get_min()};}
  /**
   * cend returns a const_iterator to nullptr
   */
  const_iterator cend() const noexcept {return const_iterator{nullptr};}
	/**
	 * Insert a key-value pair in the BST composed by the given key and value.
	 * @param key the key in the pair
	 * @param value the value in the pair
	 */
	virtual void insert(const key_type& key, const value_type& value);
	/**
	 * Insert a key-value pair in the BST.
	 * @param pair the key-value pair to insert
	 */
	void insert(const pair_type& pair){
	    insert(pair.first, pair.second);
	}
  /**
   * Remove a key-value pair from the BST, and return a pointer to the 'substitute'
   * @param key the key to use for the deletion
   */
  virtual node_type* remove(const key_type& key) {
      iterator z{find(key)};
      // if the key is not in the tree, simply return
      if (z == end()) {
          return nullptr;
      }
      node_type* curr{&(*z)};
      return remove_aux(curr);  // call the auxiliary remove
  }
  /**
   * In-order walk of the tree
   */
  void in_order_walk() {
      in_order_walk_aux(root);
  }
	/**
	 * Remove all key-value pairs from the BST.
	 */
	void clear(node_type* curr) noexcept {
	    if (curr->left_child != nullptr) {
          clear(curr->left_child);
      }
      else if (curr->right_child != nullptr) {
          clear(curr->right_child);
      }
      else {
          delete[] curr;
      }
	}
	/**
   * Overload of the operator[], in const and non-const version
   */
	value_type& operator[] (const key_type&);
	const value_type& operator[] (const key_type&) const;
};

/*
 * Node struct
 */
namespace internal {
    template<class K, class V>
    struct BST_node {

	    using pair_type = typename BST<K,V>::pair_type;
	    using key_type = typename BST<K,V>::key_type;
	    using value_type = typename BST<K,V>::value_type;
	    using node_type = BST_node<K,V>;

	    //! Pointers to left and right child of the node
	    node_type* left_child;
      node_type* right_child;
	    //! Pointer to the parent of this node
	    node_type* parent;
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
	    BST_node(const key_type key, const value_type value, node_type* father)
	     : left_child{nullptr}, right_child{nullptr}, parent{father}, data{key, value}
	    {}
      /**
       * Main algorithm for finding the successor of a node
       */
      node_type* find_successor() {
          node_type* current = this;
          if (current->right_child != nullptr) {    //if current node has right child, go down right and then as much to the left as possible
              current = current->right_child;
              while (current->left_child) {
                  current = current->left_child;
              }
              return current;
          }
          else {     //otherwise, go up until you reach the root or you find a node that is not the right child of its parent
              node_type* p = current->parent;
              while (p != nullptr && current == p->right_child) {
                  current = p;
                  p = p->parent;
              }
              current = p;
              return current;
          }
      }
      /**
       * Overload of operator==
       */
      bool operator==(node_type* other) const {return other->data == data;}
      /**
	     * Default destructor for nodes
	     */
	    ~BST_node() noexcept = default;
    };
}

/*
 * Iterator class, made compliant with the STL. In particular, the iterator allows to traverse the tree
 * in-order, that is from the smallest to the greatest key.
 */
namespace internal {
template<class K, class V>
class BST_iterator : public std::iterator<std::forward_iterator_tag, std::pair<const K,V>>{

        using pair_type = typename BST<K,V>::pair_type;
        using node_type = BST_node<K,V>;
        //! a pointer to the node the iterator is currently over

    public:
        node_type* current;
        /**
         * Constructor
         * @param n the node to initialize current to
         */
        BST_iterator(node_type* n) : current{n} {}
        /**
         * dereferencing operator, returns an std::pair<const K, V> of the current node
         */
        node_type& operator*() const {return *current;}
        /**
         * pre-increment operator. Implements algorithm for retrieving
         * the successor of a node.
         */
        BST_iterator& operator++() {
            current = current->find_successor();
            return *this;
        }
        /**
         * Comparison operators
         */
        bool operator==(const BST_iterator& other) {return current == other.current;} //tests whether two iterators share the same current node
        bool operator!=(const BST_iterator& other) {return !(*this==other);}
};
}

/*
 * const_iterator class. Populated with the functions of the iterator class, with the
 * exception of the dereferencing operator that is const, as appropriate
 */
namespace internal {
template<class K, class V>
class BST_const_iterator : public BST_iterator<K,V> {
    using node_type = BST_node<K,V>;
    using base = BST_iterator<K,V>;
    using pair_type = typename BST<K,V>::pair_type;
     public:
        using base::BST_iterator;
	  const node_type& operator*() const {return base::operator*();}
	  using base::operator++;
	  using base::operator==;
	  using base::operator!=;
};
}

/*
 * get_min function
 */
template<class K, class V, class Comp>
typename BST<K,V,Comp>::node_type* BST<K,V,Comp>::get_min(node_type* current) const noexcept {
    if (current == nullptr) { // by default start from the root
        current = root;
    }
    if (current == nullptr) return nullptr;  //if the tree is empty, return nullptr
    while (current->left_child) {   //do down to the left as much as possible
        current = current->left_child;
    }
    return current;
}

/*
 * find function
 */
template<class K, class V, class Comp>
typename BST<K,V,Comp>::iterator BST<K,V,Comp>::find(const key_type key) const noexcept {
    node_type* current{root};
    while (current) {
        key_type curr_key = current->data.first;
        if (!compare(curr_key, key) && !compare(key, curr_key)) {   //if current node has sought-after key, return an iterator to it
            return iterator{current};
        }
        else if (compare(key, curr_key)) {    //if greater, proceed in the left subtree
            current = current->left_child;
        }
        else {    //if smaller, proceed in the right subtree
            current = current->right_child;
        }
    }
    return end();    //if not found, return end
}

/*
 * insert function (key_type, value_type version)
 */
template<class K, class V, class Comp>
void BST<K,V,Comp>::insert(const key_type& key, const value_type& value) {
    if (root == nullptr) { //check if the BST is empty
	      root = new node_type{key, value, nullptr};
	      return;
    }
    node_type *previous_node{root}; //initialize previous node to root
    node_type *current_node{root}; //initilize also the current node ptr to root
    while (current_node) {
        key_type current_key{current_node->data.first};
        if (!compare(current_key, key) && !compare(key, current_key)) { //if the key is already in the tree update the value
	          current_node->data.second = value;
	          return;
        }
        else if (compare(key, current_key)) { // if the new key is smaller go to left subtree
	          previous_node = current_node;
            current_node = current_node->left_child;
        }
        else {    //if new key is bigger go to in the right subtree
	          previous_node = current_node;
            current_node = current_node->right_child;
        }
    }
    auto& child = (compare(key, previous_node->data.first)) ? previous_node->left_child : previous_node->right_child;
    child = new node_type{key, value, previous_node};
}

/**
 * Overload of operator[] for BSTs, non-const version
 */
template<class K, class V, class Comp>
typename BST<K,V,Comp>::value_type& BST<K,V,Comp>::operator[](const key_type& arg_key) {
    iterator iter = find(arg_key);
    if (iter != end()) {
        return (*iter).second;
    }
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
        os << x.data.first << ": " << x.data.second << std::endl;    //iterate in order and print the key: value pairs
    }
    return os;
}

#endif
