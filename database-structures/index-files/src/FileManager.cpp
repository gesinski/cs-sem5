#include "FileManager.hpp"

FileManager::~FileManager() {
    if (records_file.is_open()) {
        records_file.close();
    }
    if (b_tree_file.is_open()) {
        b_tree_file.close();
    }
}