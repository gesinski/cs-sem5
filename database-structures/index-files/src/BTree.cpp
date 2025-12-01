#include "BTree.hpp"

int BTree::search(unsigned int key) {
    Node *r = get_root();

    if (!r) return -1; 
    if (r->page_num < 0) return -1;

    
    return r->page_num;
 }

 void BTree::create_btree(FileManager &file_manager) {
    Node *root = new Node();
    set_root(root);
 }