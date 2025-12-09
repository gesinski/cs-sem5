#include "FileManager.hpp"

FileManager::FileManager(const std::string &main_file_name, const std::string &index_file_name,  const std::string &overflow_file_name) {
    main_file.open(main_file_name, std::ios::out | std::ios::binary | std::ios::trunc);
    main_file.close();
    main_file.open(main_file_name, std::ios::in | std::ios::out | std::ios::binary);

    index_file.open(index_file_name, std::ios::out | std::ios::binary | std::ios::trunc);
    index_file.close();
    index_file.open(index_file_name, std::ios::in | std::ios::out | std::ios::binary);

    overflow_file.open(overflow_file_name, std::ios::out | std::ios::binary | std::ios::trunc);
    overflow_file.close();
    overflow_file.open(overflow_file_name, std::ios::in | std::ios::out | std::ios::binary);
}

std::string FileManager::read_index_page(unsigned int page_num) {
    std::string page(INDEX_PAGE_SIZE, '\0');
    std::streampos offset = (KEY_SIZE+POINTER_SIZE) * (page_num-1);

    index_file.seekg(offset);
    index_file.read(page.data(), INDEX_PAGE_SIZE);
    disk_reads++;

    return page;
}

void FileManager::write_index_page(const char page[], unsigned int page_num) {
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

void FileManager::write_page(const char page[], unsigned int page_num, bool overflow) {
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

void init_index_page(char buf[], unsigned int &key, unsigned int &page_num) {
    std::memcpy(buf, &key, KEY_SIZE);
    std::memcpy(buf + KEY_SIZE, &page_num, POINTER_SIZE);
    std::memset(buf + (KEY_SIZE+POINTER_SIZE), 0, INDEX_PAGE_SIZE-(KEY_SIZE+POINTER_SIZE));
}

void init_page(char main_buf[], unsigned int &key, std::string &record) {
    std::memcpy(main_buf, &key, KEY_SIZE);
    std::memset(main_buf + KEY_SIZE, 0, VALUE_SIZE);
    std::memcpy(main_buf + KEY_SIZE, record.data(), VALUE_SIZE);
    std::memset(main_buf + RECORD_SIZE, 0, POINTER_SIZE);
    std::memset(main_buf + RECORD_SIZE+POINTER_SIZE, 0, PAGE_SIZE-(RECORD_SIZE+POINTER_SIZE));
}

bool FileManager::insert(unsigned int key, std::string record) {
    if(key < smallest_key) {
        smallest_key = key;

        unsigned int page_num = 1;
        std::string page = read_page(page_num, false);
        
        unsigned int fetched_key, fetched_key2;
        std::memcpy(&fetched_key, page.data(), KEY_SIZE);
        std::memcpy(&fetched_key2, page.data() + RECORD_SIZE + POINTER_SIZE, KEY_SIZE);

        std::string fetched_record(page.begin() + KEY_SIZE, page.begin() + VALUE_SIZE);

        if(fetched_key == 0) {
            char buf[INDEX_PAGE_SIZE];
            init_index_page(buf, key, page_num);
            write_index_page(buf, page_num);

            char main_buf[PAGE_SIZE];
            init_page(main_buf, key, record);
            write_page(main_buf, page_num, false);

            char overfl_buf[PAGE_SIZE];
            std::memset(overfl_buf, 0, PAGE_SIZE);
            write_page(overfl_buf, page_num, true);

            return true;
        }
        else if(fetched_key2 == 0) {
            std::memcpy(page.data() + RECORD_SIZE + POINTER_SIZE, &fetched_key, KEY_SIZE);
            std::memset(page.data() + RECORD_SIZE + POINTER_SIZE + KEY_SIZE, 0, VALUE_SIZE);
            std::memcpy(page.data() + RECORD_SIZE + POINTER_SIZE + KEY_SIZE, fetched_record.data(), VALUE_SIZE);

            std::memcpy(page.data(), &key, KEY_SIZE);
            std::memset(page.data() + KEY_SIZE, 0, VALUE_SIZE);
            std::memcpy(page.data() + KEY_SIZE, record.data(), VALUE_SIZE);

            write_page(page.data(), page_num, false);

            return true;
        } else {
            std::memcpy(page.data(), &key, KEY_SIZE);
            std::memset(page.data() + KEY_SIZE, 0, VALUE_SIZE);
            std::memcpy(page.data() + KEY_SIZE, record.data(), VALUE_SIZE);


            std::string overflow_page = read_page(page_num, true);

            for(unsigned int record_num = 0; record_num < BLOCKING_FACTOR; record_num++) {
                unsigned int overflow_key;
                std::memcpy(&overflow_key, overflow_page.data() + record_num*(RECORD_SIZE+POINTER_SIZE), KEY_SIZE);
                if(overflow_key == 0) {
                    std::memcpy(overflow_page.data() + record_num*(RECORD_SIZE+POINTER_SIZE), &fetched_key, KEY_SIZE);
                    std::memcpy(overflow_page.data() + record_num*(RECORD_SIZE+POINTER_SIZE) + KEY_SIZE, fetched_record.data(), VALUE_SIZE);

                    write_page(overflow_page.data(), page_num, true);

                    std::memcpy(page.data() + RECORD_SIZE, &record_num, POINTER_SIZE);
                    write_page(page.data(), page_num, false);
                    return true;
                }
            }
        }
    } else {
        unsigned int index_page_num = 1;
        std::string index_page = read_index_page(index_page_num);
        
        while(!index_page.empty()) {
            for(unsigned int i = 0; i < INDEX_PAGE_SIZE; i+=(KEY_SIZE + POINTER_SIZE)) {

                unsigned int fetched_key, fetched_key2;
                std::memcpy(&fetched_key, index_page.data() + i, KEY_SIZE);
                std::memcpy(&fetched_key2, index_page.data() + i + (KEY_SIZE + POINTER_SIZE), KEY_SIZE);

                if(key > fetched_key && key < fetched_key2) {
                    unsigned int page_num;
                    std::memcpy(&page_num, index_page.data() + i + KEY_SIZE, POINTER_SIZE);
                    
                    std::string page = read_page(page_num, false);
                    
                    for(unsigned int record_num = 1; record_num < BLOCKING_FACTOR; record_num++) {
                        std::memcpy(&fetched_key, page.data() + record_num*(RECORD_SIZE+POINTER_SIZE), KEY_SIZE);
                        if(fetched_key == 0) {
                            std::memcpy(page.data() + record_num*(RECORD_SIZE+POINTER_SIZE), &key, KEY_SIZE);
                            std::memcpy(page.data() + record_num*(RECORD_SIZE+POINTER_SIZE) + KEY_SIZE, record.data(), VALUE_SIZE);
                            
                            write_page(page.data(), page_num, false);
                            
                            return true;
                        } else if(key < fetched_key) {
                            unsigned int pointer;

                            std::memcpy(&pointer, page.data() + (record_num-1)*(RECORD_SIZE+POINTER_SIZE) - POINTER_SIZE, POINTER_SIZE);
                            std::string overflow_page = read_page(1, true);

                            if(pointer == 0) {
                                for(unsigned int ov_record_num = 0; ov_record_num < BLOCKING_FACTOR; ov_record_num++) {
                                    unsigned int overflow_key;
                                    
                                    std::memcpy(&overflow_key, overflow_page.data() + ov_record_num*(RECORD_SIZE+POINTER_SIZE), KEY_SIZE);
                                    
                                    if(overflow_key == 0) {
                                        std::memcpy(overflow_page.data() + ov_record_num*(RECORD_SIZE+POINTER_SIZE), &key, KEY_SIZE);
                                        std::memcpy(overflow_page.data() + ov_record_num*(RECORD_SIZE+POINTER_SIZE) + KEY_SIZE, record.data(), VALUE_SIZE);   
                                        write_page(overflow_page.data(), page_num, true);
                                        
                                        std::memcpy(page.data() + record_num*(RECORD_SIZE+POINTER_SIZE) - POINTER_SIZE, &ov_record_num, POINTER_SIZE);
                                        write_page(page.data(), page_num, false);
                                        
                                        return true;
                                    }
                                }
                            } else {
                                unsigned int overflow_pointer;
                                std::memcpy(&overflow_pointer, overflow_page.data() + pointer*(RECORD_SIZE+POINTER_SIZE), POINTER_SIZE);
                                while(overflow_pointer != 0) {
                                    std::memcpy(&overflow_pointer, overflow_page.data() + overflow_pointer*(RECORD_SIZE+POINTER_SIZE), POINTER_SIZE);
                                }
                                for(unsigned int ov_record_num = overflow_pointer+1; ov_record_num < BLOCKING_FACTOR; ov_record_num++) {
                                    unsigned int overflow_key;
                                    
                                    std::memcpy(&overflow_key, overflow_page.data() + ov_record_num*(RECORD_SIZE+POINTER_SIZE), KEY_SIZE);
                                    
                                    if(overflow_key == 0) {
                                        std::memcpy(overflow_page.data() + ov_record_num*(RECORD_SIZE+POINTER_SIZE), &key, KEY_SIZE);
                                        std::memcpy(overflow_page.data() + ov_record_num*(RECORD_SIZE+POINTER_SIZE) + KEY_SIZE, record.data(), VALUE_SIZE);   
                                        write_page(overflow_page.data(), page_num, true);
                                        
                                        std::memcpy(page.data() + overflow_pointer*(RECORD_SIZE+POINTER_SIZE) - POINTER_SIZE, &ov_record_num, POINTER_SIZE);
                                        write_page(page.data(), page_num, false);
                                        
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
                    unsigned int page_num;
                    std::memcpy( &page_num, index_page.data() + i - POINTER_SIZE, POINTER_SIZE);
                    std::memcpy(index_page.data() + i, &key, KEY_SIZE);
                    std::memcpy(index_page.data() + i + KEY_SIZE, &page_num, POINTER_SIZE);
                    write_index_page(index_page.data(), index_page_num);

                    char main_buf[PAGE_SIZE];
                    init_page(main_buf, key, record);    
                    write_page(main_buf, page_num, false);
                    
                    return true;
                } else if(key < fetched_key) {
                    //TODO
                } else if(key == fetched_key || key == fetched_key2) {
                    return false;
                }
            }
            index_page_num++;
            std::string index_page = read_index_page(index_page_num);
        }
        char buf[INDEX_PAGE_SIZE];
        init_index_page(buf, key, index_page_num);
        write_index_page(buf, index_page_num);

        char main_buf[PAGE_SIZE];
        init_page(main_buf, key, record);    
        write_page(main_buf, 1, false);

        char overfl_buf[PAGE_SIZE];
        std::memset(overfl_buf, 0, PAGE_SIZE);
        write_page(overfl_buf, 1, true);

        return true;
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
