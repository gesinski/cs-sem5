#include "FileManager.hpp"

FileManager::FileManager(const std::string &main_file_name, const std::string &index_file_name,  const std::string &overflow_file_name) {
    main_file.open(main_file_name, std::ios::out | std::ios::binary | std::ios::trunc);
    char zero_page[PAGE_SIZE];
    std::memset(zero_page, 0, PAGE_SIZE);
    main_file.write(zero_page, PAGE_SIZE);
    main_file.close();
    main_file.open(main_file_name, std::ios::in | std::ios::out | std::ios::binary);

    index_file.open(index_file_name, std::ios::out | std::ios::binary | std::ios::trunc);
    char zero_index[INDEX_PAGE_SIZE];
    std::memset(zero_index, 0, INDEX_PAGE_SIZE);
    index_file.write(zero_index, INDEX_PAGE_SIZE);
    index_file.close();
    index_file.open(index_file_name, std::ios::in | std::ios::out | std::ios::binary);

    overflow_file.open(overflow_file_name, std::ios::out | std::ios::binary | std::ios::trunc);
    char zero_overflow[PAGE_SIZE];
    std::memset(zero_overflow, 0, PAGE_SIZE);
    overflow_file.write(zero_overflow, PAGE_SIZE);
    overflow_file.close();
    overflow_file.open(overflow_file_name, std::ios::in | std::ios::out | std::ios::binary);
}

struct index_record {
    unsigned int key;
    unsigned int page_number;
};

struct record {
    unsigned int key;
    char data[DATA_SIZE];
    unsigned int pointer;
};

struct index_record *convert_string_to_index_record(const std::string &str) {
    struct index_record *rec = new struct index_record[BLOCKING_FACTOR];
    std::stringstream ss(str);
    for(unsigned int i = 0; i < BLOCKING_FACTOR; i++) { 
        ss >> rec[i].key;
        ss >> rec[i].page_number;
    }
    return rec;
}

struct record *convert_string_to_record(const std::string &str) {
    struct record *rec = new struct record[BLOCKING_FACTOR];
    std::stringstream ss(str);
    for(unsigned int i = 0; i < BLOCKING_FACTOR; i++) {
        ss >> rec[i].key;
        ss.read(rec[i].data, DATA_SIZE);
        ss >> rec[i].pointer;
    }
    return rec;
}

std::string FileManager::read_index_page(unsigned int page_num) {
    std::string page(INDEX_PAGE_SIZE, '\0');
    std::streampos offset = (KEY_SIZE+POINTER_SIZE) * (page_num-1);

    index_file.seekg(offset);
    index_file.read(page.data(), INDEX_PAGE_SIZE);
    disk_reads++;

    return page;
}

void FileManager::write_index_page(struct index_record index_page[], unsigned int page_num) {        
    char page[INDEX_PAGE_SIZE];
    
    for(unsigned int i = 0; i < BLOCKING_FACTOR; i++) {
        std::memcpy(page + i*INDEX_PAGE_SIZE, &index_page->key, KEY_SIZE);
        std::memcpy(page + i*INDEX_PAGE_SIZE + KEY_SIZE, &index_page->page_number, POINTER_SIZE);
    }

    std::streampos offset = (KEY_SIZE+POINTER_SIZE) * (page_num-1);
    
    index_file.seekp(offset);
    index_file.write(page, INDEX_PAGE_SIZE);
    index_file.flush();
    disk_writes++;
}

std::string FileManager::read_page(unsigned int page_num, bool overflow) {
    std::string page(PAGE_SIZE, '\0');
    std::streampos offset = static_cast<std::streampos>(PAGE_SIZE) * (page_num-1);

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

void FileManager::write_page(struct record record_page[], unsigned int page_num, bool overflow) {
    char page[PAGE_SIZE];
    
    for(unsigned int i = 0; i < BLOCKING_FACTOR; i++) {
        std::memcpy(page + i*INDEX_PAGE_SIZE, &record_page->key, KEY_SIZE);
        std::memcpy(page + i*INDEX_PAGE_SIZE + KEY_SIZE, &record_page->data, DATA_SIZE);
        std::memcpy(page + i*INDEX_PAGE_SIZE + RECORD_SIZE, &record_page->pointer, POINTER_SIZE);
    }

    std::streampos offset = static_cast<std::streampos>(PAGE_SIZE) * (page_num-1);

    if(overflow) {
        overflow_file.seekp(offset);
        overflow_file.write(page, PAGE_SIZE);
        overflow_file.flush();
        disk_writes++;
    }
    else {
        main_file.seekp(offset);
        main_file.write(page, PAGE_SIZE);
        main_file.flush();
        disk_writes++;
    }
}

void init_page(char main_buf[], unsigned int &key, std::string &record) {
    std::memcpy(main_buf, &key, KEY_SIZE);
    std::memset(main_buf + KEY_SIZE, 0, DATA_SIZE);
    std::memcpy(main_buf + KEY_SIZE, record.data(), DATA_SIZE);
    std::memset(main_buf + RECORD_SIZE, 0, POINTER_SIZE);
    std::memset(main_buf + RECORD_SIZE+POINTER_SIZE, 0, PAGE_SIZE-(RECORD_SIZE+POINTER_SIZE));
}

bool FileManager::insert(unsigned int key, std::string record) {
    if(key < smallest_key) {
        smallest_key = key;

        unsigned int page_num = 1;
        unsigned int record_num = 0;

        struct index_record *index_page = convert_string_to_index_record(read_index_page(page_num));
        index_page[record_num].key = key;
        index_page[record_num].page_number = page_num;
        write_index_page(index_page, page_num);
            
        struct record *main_page = convert_string_to_record(read_page(page_num, false));
        
        unsigned int fetched_key = main_page[record_num].key;
        unsigned int fetched_key2 = main_page[record_num+1].key;

        if(fetched_key == 0) {
            main_page[record_num].key = key;
            std::memset(main_page[record_num].data, 0, DATA_SIZE);
            std::memcpy(main_page[record_num].data, record.data(), DATA_SIZE);
            write_page(main_page, page_num, false);

            return true;
        }
        else if(fetched_key2 == 0) {
            main_page[record_num+1].key = fetched_key;
            std::memset(main_page[record_num+1].data, 0, DATA_SIZE);
            std::memcpy(main_page[record_num+1].data, main_page[record_num].data, DATA_SIZE);
            main_page[record_num+1].pointer = main_page[record_num].pointer;

            main_page[record_num].key = key;
            std::memset(main_page[record_num].data, 0, DATA_SIZE);
            std::memcpy(main_page[record_num].data, record.data(), DATA_SIZE);
            main_page[record_num].pointer = 0;

            write_page(main_page, page_num, false);

            return true;
        } else {
            struct record *overflow_page = convert_string_to_record(read_page(page_num, true));

            for(unsigned int record_num = 0; record_num < BLOCKING_FACTOR; record_num++) {
                unsigned int overflow_key = overflow_page[record_num].key;
                if(overflow_key == 0) {
                    overflow_page[record_num].key = fetched_key;
                    std::memset(overflow_page[record_num].data, 0, DATA_SIZE);
                    std::memcpy(overflow_page[record_num].data, main_page[record_num].data, DATA_SIZE);
                    overflow_page[record_num].pointer = main_page[record_num].pointer;

                    write_page(overflow_page, 1, true);

                    main_page[record_num].key = key;
                    std::memset(main_page[record_num].data, 0, DATA_SIZE);
                    std::memcpy(main_page[record_num].data, record.data(), DATA_SIZE);
                    main_page[record_num].pointer = 0;
                    
                    write_page(main_page, page_num, false);
                    return true;
                }
            }
        }
    } else {
        unsigned int index_page_num = 1;
        struct index_record *index_page = convert_string_to_index_record(read_index_page(index_page_num));
        
        while(index_page->key != 0) {
            for(unsigned int i = 0; i < BLOCKING_FACTOR; i++) {
                unsigned int fetched_key = index_page[i].key, fetched_key2;
                if(i+1 < BLOCKING_FACTOR)
                    fetched_key2 = index_page[i+1].key;
                else 
                    fetched_key2 = 0;

                unsigned int page_num = index_page[i].page_number;
                if(key > fetched_key && key < fetched_key2) {
                    
                    struct record *page = convert_string_to_record(read_page(page_num-1, false));
                    
                    for(unsigned int record_num = 0; record_num < BLOCKING_FACTOR; record_num++) {
                        fetched_key = page[record_num].key;
                        if(fetched_key == 0) {
                            page[record_num].key = key;
                            std::memset(page[record_num].data, 0, DATA_SIZE);
                            std::memcpy(page[record_num].data, record.data(), DATA_SIZE);
                            write_page(page, page_num, false);
                            
                            return true;
                        } else if(key < fetched_key) {
                            unsigned int pointer = page[record_num].pointer;

                            struct record *overflow_page = convert_string_to_record(read_page(1, true));

                            if(pointer == 0) {
                                for(unsigned int ov_record_num = 0; ov_record_num < BLOCKING_FACTOR; ov_record_num++) {
                                    
                                    unsigned int overflow_key = overflow_page[ov_record_num].key;
                                    
                                    if(overflow_key == 0) {
                                        overflow_page[ov_record_num].key = key;
                                        std::memset(overflow_page[ov_record_num].data, 0, DATA_SIZE);
                                        std::memcpy(overflow_page[ov_record_num].data, record.data(), DATA_SIZE);
                                        write_page(overflow_page, 1, true);
                                    
                                        page[record_num].pointer = ov_record_num+1;
                                        write_page(page, page_num, false);

                                        return true;
                                    }
                                }
                            } else {
                                unsigned int overflow_pointer = overflow_page[pointer-1].pointer;
                                while(overflow_pointer != 0) {
                                    overflow_pointer = overflow_page[overflow_pointer-1].pointer;
                                }
                                for(unsigned int ov_record_num = overflow_pointer; ov_record_num < BLOCKING_FACTOR; ov_record_num++) {
                                    unsigned int overflow_key = overflow_page[ov_record_num].key;
                                                                    
                                    if(overflow_key == 0) {
                                        overflow_page[ov_record_num].key = key;
                                        std::memset(overflow_page[ov_record_num].data, 0, DATA_SIZE);
                                        std::memcpy(overflow_page[ov_record_num].data, record.data(), DATA_SIZE);
                                                                                
                                        overflow_page[pointer-1].pointer = ov_record_num+1;
                                        write_page(overflow_page, 1, true);
                                        
                                        return true;
                                    }
                                }
                            }
                        } else if(key == fetched_key) {
                            return false;
                        }
                    }
                    return true;
                } else if(fetched_key == 0) {
                    index_page[i].key = key;
                    index_page[i].page_number = page_num + 1;
                    write_index_page(index_page, index_page_num);

                    char main_buf[PAGE_SIZE];
                    init_page(main_buf, key, record);    
                    write_page(convert_string_to_record(main_buf), page_num, false);
                    
                    return true;
                } else if(key < fetched_key) {
                    //TODO
                } else if(key == fetched_key || key == fetched_key2) {
                    return false;
                }
                index_page[index_page_num].key = key;
                index_page[index_page_num].page_number = page_num;
                write_index_page(index_page, index_page_num);

                char main_buf[PAGE_SIZE];
                init_page(main_buf, key, record);    
                write_page(convert_string_to_record(main_buf), page_num, false);
            }
            index_page_num++;
            index_page = convert_string_to_index_record(read_index_page(index_page_num));
        }
        char buf[INDEX_PAGE_SIZE];
        std::memcpy(buf, &key, KEY_SIZE);
        std::memcpy(buf + KEY_SIZE, &index_page_num, POINTER_SIZE);
        write_index_page(convert_string_to_index_record(buf), index_page_num);

        char main_buf[PAGE_SIZE];
        init_page(main_buf, key, record);    
        write_page(convert_string_to_record(main_buf), index_page[index_page_num-1].page_number+1, false);
    }
    return true;
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
