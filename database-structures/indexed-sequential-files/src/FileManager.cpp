#include "FileManager.hpp"

std::string FileManager::read_index_page(int page_num) {
    std::string page;
    std::streampos offset = INDEX_PAGE_SIZE * (page_num-1);

    index_file.seekg(offset);
    index_file.read(page.data(), INDEX_PAGE_SIZE);
    disk_reads++;

    return page;
}

void FileManager::write_index_page(std::string &page, int page_num) {
    std::streampos offset = INDEX_PAGE_SIZE * (page_num-1);
    
    index_file.seekp(offset);
    index_file.write(page.data(), offset);
    disk_writes++;
}

std::string FileManager::read_page(int page_num, bool overflow) {
    std::string page;
    std::streampos offset = PAGE_SIZE * (page_num-1);

    if(overflow) {
        overflow_file.seekg(offset);
        overflow_file.read(page.data(), PAGE_SIZE);
        disk_reads++;
    }
    else {
        main_file.seekg(offset);
        main_file.read(page.data(), PAGE_SIZE);
        disk_reads++;
    }

    return page;
}

void FileManager::write_page(std::string &page, int page_num, bool overflow) {
    std::streampos offset = PAGE_SIZE * (page_num-1);

    if(overflow) {
        overflow_file.seekp(offset);
        overflow_file.write(page.data(), PAGE_SIZE);
        disk_writes++;
    }
    else {
        main_file.seekp(offset);
        main_file.write(page.data(), PAGE_SIZE);
        disk_writes++;
    }
}

FileManager::~FileManager() {
    if (main_file.is_open()) {
        main_file.close();
    }
    if (index_file.is_open()) {
        index_file.close();
    }
    if (overflow_file.is_open()) {
        overflow_file.close();
    }
}