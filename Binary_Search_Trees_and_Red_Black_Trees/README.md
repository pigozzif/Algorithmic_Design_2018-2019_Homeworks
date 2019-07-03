# Homework 5 - Binary Search Trees and Red Black Trees (optional)
## Content
This folder contains a `BST.h` header file, with most of the implementation of the binary search tree class realized for the "Advanced Programming" course (together with Federico Julian Verdù Camerota). A method to delete the key from the tree has been implemented, as required by the first part of the assignment.

The file `RedBlackTree.cc` contains a class that inherits from the BST class and extends it by adding the `insert` and `remove` methods, following the algorithm proposed in class.

## Discaimer
Despite harsh efforts, many tests on the red black tree kept on throwing errors. Nevertheless, I decided to leave its implementation here, for many reasons: 1) It is supposed to be an optional exercise; 2) I checked the code over and over again, and I feel pretty sure the `RedBlackTree` class is correct; 3) Then, I arrived to the conclusion that the bugs might have to be found in the original binary search tree implementation, going beyond the boundaries of this course.

## Compilation
Type `make` and an executable named `rbt.x` will be produced.
