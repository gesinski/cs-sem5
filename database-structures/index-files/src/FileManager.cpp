#include "FileManager.hpp"

std::string read_btree_block(int page) {

}

void write_btree_block(std::string &block, int page) {

}

std::string read_records_block(int page) {

}

void write_records_block(std::string &block, int page) {

}

FileManager::~FileManager() {
    if (records_file.is_open()) {
        records_file.close();
    }
    if (b_tree_file.is_open()) {
        b_tree_file.close();
    }
}