#include "BTree.hpp"

 Node *BTree::get_node_from_string(std::string page) {
   Node *node = new Node();
   return node;
 }

int BTree::search(unsigned int seek_key, FileManager &file_manager) {
   Node *current_node = get_root();

   int new_node_page = current_node->page_num;
   while(!current_node && current_node->page_num != -1) {
      current_node = get_node_from_string(file_manager.read_btree_block(new_node_page));

      int i = 0;
      for(unsigned int x : current_node->key) {
         if(x == seek_key)
            return current_node->records_block_num[i];
         else if(x > seek_key) {
            break;
         } 
         i++;
      }
      new_node_page = current_node->pointers[i];
   }
    
   return -1;
}

bool compensation_possible() {
   return false;
}

void compensation() {
   return;
}

Node *make_split() {
   Node *new_node = new Node();
   return new_node;
}

bool BTree::insert(unsigned int inserted_key, int block_num, FileManager &file_manager) {
   if(search(inserted_key, file_manager) == -1) 
      return false;
   Node *current_page = get_root();
   while(true) {
      if(current_page->key.size() < 2*d) {
         int i = 0;
         for(unsigned int x : current_page->key) {
            if(inserted_key < x) {
               current_page->key.insert(current_page->key.begin()+i, inserted_key);
               break;
            }
            i++;
         }
      }
      else if(compensation_possible()) {
         //compensation();
         current_page->key.insert(current_page->key.begin(), inserted_key);
         break;
      }
      current_page = make_split();
   }
   return true;
}

void BTree::create_btree(FileManager &file_manager, unsigned int _d) {

   Node *root = new Node();
   set_root(root);
}