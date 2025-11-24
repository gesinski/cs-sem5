#include "FileManager.hpp"

FileManager::FileManager(const std::string& records_file_name, 
                const std::string& btree_file_name) 
                : input_file(records_file_name), b_tree_file(btree_file_name) {
                    
}

FileManager::~FileManager() {
    if (input_file.is_open()) {
        input_file.close();
    }
    if (b_tree_file.is_open()) {
        b_tree_file.close();
    }
}