#include "FileManager.hpp"

std::string FileManager::read_btree_block(int page) {
    std::string block;
    return block;
}

void FileManager::write_btree_block(std::string &block, int page) {

}

std::string FileManager::read_records_block(int page) {
    std::string block;
    return block;
}

void FileManager::write_records_block(std::string &block, int page) {

}

FileManager::~FileManager() {
    if (records_file.is_open()) {
        records_file.close();
    }
    if (b_tree_file.is_open()) {
        b_tree_file.close();
    }
}