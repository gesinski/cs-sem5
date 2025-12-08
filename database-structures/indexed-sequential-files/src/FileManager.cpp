#include "FileManager.hpp"

std::string FileManager::read_index_page(unsigned int page_num) {
    std::string page(INDEX_PAGE_SIZE, '\0');
    std::streampos offset = (KEY_SIZE+POINTER_SIZE) * (page_num-1);

    index_file.seekg(offset);
    index_file.read(page.data(), INDEX_PAGE_SIZE);
    disk_reads++;

    return page;
}

void FileManager::write_index_page(char page[], unsigned int page_num) {
    std::streampos offset = (KEY_SIZE+POINTER_SIZE) * (page_num-1);
    
    index_file.seekp(offset);
    index_file.write(page, INDEX_PAGE_SIZE);
    disk_writes++;
}

std::string FileManager::read_page(unsigned int page_num, bool overflow) {
    std::string page(PAGE_SIZE, '\0');
    std::streampos offset = (RECORD_SIZE+POINTER_SIZE) * (page_num-1);

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

void FileManager::write_page(char page[], unsigned int page_num, bool overflow) {
    std::streampos offset = (RECORD_SIZE+POINTER_SIZE) * (page_num-1);

    if(overflow) {
        overflow_file.seekp(offset);
        overflow_file.write(page, PAGE_SIZE);
        disk_writes++;
    }
    else {
        main_file.seekp(offset);
        main_file.write(page, PAGE_SIZE);
        disk_writes++;
    }
}

void FileManager::insert(unsigned int key, std::string record) {
    if(key < smallest_key) {
        smallest_key = key;
        //...
    } else {
        std::string index_page = read_index_page(1);
        if(index_page.empty()) {
            for(unsigned int i = 0; i < INDEX_PAGE_SIZE; i+=(KEY_SIZE + POINTER_SIZE)) {
                //unsigned int fetched_key;
            }
        } else {
            char buf[32];
            unsigned int page_num = 1;

            std::memcpy(buf, &key, KEY_SIZE);
            std::memset(buf + 4, ' ', KEY_SIZE+POINTER_SIZE);
            std::memcpy(buf + 4, &page_num, POINTER_SIZE);

            write_index_page(buf, page_num);
            //insert to main_file
        }
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