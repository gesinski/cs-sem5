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

struct index_record *convert_string_to_index_page(const std::string &str) {
    struct index_record *rec = new struct index_record[BLOCKING_FACTOR];
    
    for(unsigned int i = 0; i < BLOCKING_FACTOR; i++) {
        unsigned int offset = i * (KEY_SIZE + POINTER_SIZE);
        
        std::memcpy(&rec[i].key, str.data() + offset, KEY_SIZE);
        
        std::memcpy(&rec[i].page_number, str.data() + offset + KEY_SIZE, POINTER_SIZE);
    }
    
    return rec;
}

struct record *convert_string_to_page(const std::string &str) {
    struct record *rec = new struct record[BLOCKING_FACTOR];
    
    for(unsigned int i = 0; i < BLOCKING_FACTOR; i++) {
        unsigned int offset = i * (RECORD_SIZE + POINTER_SIZE);
        
        std::memcpy(&rec[i].key, str.data() + offset, KEY_SIZE);
        
        std::memcpy(rec[i].data, str.data() + offset + KEY_SIZE, DATA_SIZE);
        
        std::memcpy(&rec[i].pointer, str.data() + offset + KEY_SIZE + DATA_SIZE, POINTER_SIZE);
    }
    
    return rec;
}

std::string FileManager::read_index_page(unsigned int page_num) {
    std::string page(INDEX_PAGE_SIZE, '\0');
    std::streampos offset = static_cast<std::streampos>(INDEX_PAGE_SIZE) * (page_num - 1);

    index_file.seekg(offset);
    index_file.read(page.data(), INDEX_PAGE_SIZE);
    disk_reads++;

    return page;
}

void FileManager::write_index_page(struct index_record index_page[], unsigned int page_num) {        
    char page[INDEX_PAGE_SIZE];
    std::memset(page, 0, INDEX_PAGE_SIZE);
    
    for(unsigned int i = 0; i < BLOCKING_FACTOR; i++) {
        std::memcpy(page + i*(KEY_SIZE+POINTER_SIZE), &index_page[i].key, KEY_SIZE);
        std::memcpy(page + i*(KEY_SIZE+POINTER_SIZE) + KEY_SIZE, &index_page[i].page_number, POINTER_SIZE);
    }
   
    std::streampos offset = static_cast<std::streampos>(INDEX_PAGE_SIZE) * (page_num - 1);

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
    std::memset(page, 0, PAGE_SIZE);
    
    for(unsigned int i = 0; i < BLOCKING_FACTOR; i++) {
        std::memcpy(page + i*(RECORD_SIZE+POINTER_SIZE), &record_page[i].key, KEY_SIZE);
        std::memcpy(page + i*(RECORD_SIZE+POINTER_SIZE) + KEY_SIZE, &record_page[i].data, DATA_SIZE);
        std::memcpy(page + i*(RECORD_SIZE+POINTER_SIZE) + RECORD_SIZE, &record_page[i].pointer, POINTER_SIZE);
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
    // debug log
    // {
    //     std::ofstream dbg("insert_debug.log", std::ios::app);
    //     dbg << "insert called key=" << key << " record='" << record << "' smallest_key=" << smallest_key << "\n";
    // }

    if(key < smallest_key) {
        smallest_key = key;

        unsigned int page_num = 1;
        unsigned int record_num = 0;

        struct index_record *index_page = convert_string_to_index_page(read_index_page(page_num));
        index_page[record_num].key = key;
        index_page[record_num].page_number = page_num;
        write_index_page(index_page, page_num);
            
        struct record *main_page = convert_string_to_page(read_page(page_num, false));
        
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
            struct record *overflow_page = convert_string_to_page(read_page(page_num, true));

            for(unsigned int record_num = 0; record_num < BLOCKING_FACTOR; record_num++) {
                unsigned int overflow_key = overflow_page[record_num].key;
                if(overflow_key == 0) {
                    overflow_page[record_num].key = fetched_key;
                    std::memset(overflow_page[record_num].data, 0, DATA_SIZE);
                    std::memcpy(overflow_page[record_num].data, main_page[0].data, DATA_SIZE);
                    overflow_page[record_num].pointer = main_page[0].pointer;

                    write_page(overflow_page, 1, true);

                    main_page[0].key = key;
                    std::memset(main_page[0].data, 0, DATA_SIZE);
                    std::memcpy(main_page[0].data, record.data(), DATA_SIZE);
                    main_page[0].pointer = record_num + 1;
                    
                    write_page(main_page, page_num, false);
                    return true;
                }
            }
        }
    } else {
        unsigned int index_page_num = 1;
        struct index_record *index_page = convert_string_to_index_page(read_index_page(index_page_num));
        
        while(index_page[index_page_num-1].key != 0) {
            for(unsigned int i = 0; i < BLOCKING_FACTOR; i++) {
                unsigned int fetched_key = index_page[i].key, fetched_key2;
                if(i+1 < BLOCKING_FACTOR)
                    fetched_key2 = index_page[i+1].key;
                else 
                    fetched_key2 = 0;

                unsigned int page_num = index_page[i].page_number;
                if(key > fetched_key && (key < fetched_key2 || fetched_key2 == 0)) {
                    
                    struct record *page = convert_string_to_page(read_page(page_num, false));
                    
                    for(unsigned int record_num = 0; record_num < BLOCKING_FACTOR; record_num++) {
                        fetched_key = page[record_num].key;
                        if(fetched_key == 0) {
                            page[record_num].key = key;
                            std::memset(page[record_num].data, 0, DATA_SIZE);
                            std::memcpy(page[record_num].data, record.data(), DATA_SIZE);
                            write_page(page, page_num, false);
                            
                            return true;
                        } else if(key < fetched_key) {
                            unsigned int pointer = page[record_num-1].pointer;

                            struct record *overflow_page = convert_string_to_page(read_page(1, true));

                            if(pointer == 0) {
                                for(unsigned int ov_record_num = 0; ov_record_num < BLOCKING_FACTOR; ov_record_num++) {
                                    
                                    unsigned int overflow_key = overflow_page[ov_record_num].key;
                                    
                                    if(overflow_key == 0) {
                                        overflow_page[ov_record_num].key = key;
                                        std::memset(overflow_page[ov_record_num].data, 0, DATA_SIZE);
                                        std::memcpy(overflow_page[ov_record_num].data, record.data(), DATA_SIZE);
                                        write_page(overflow_page, 1, true);
                                    
                                        page[record_num-1].pointer = ov_record_num+1;
                                        write_page(page, page_num, false);

                                        return true;
                                    }
                                }
                            } else {
                                unsigned int head = pointer;

                                if (head != 0 && overflow_page[head - 1].key == key) {
                                    return false;
                                }

                                int free_slot = -1;
                                for (unsigned int ov_record_num = 0; ov_record_num < BLOCKING_FACTOR; ov_record_num++) {
                                    if (overflow_page[ov_record_num].key == 0) {
                                        free_slot = static_cast<int>(ov_record_num);
                                        break;
                                    }
                                }

                                if (free_slot == -1) {
                                    return false;
                                }

                                if (head == 0 || key < overflow_page[head - 1].key) {
                                    overflow_page[free_slot].key = key;
                                    std::memset(overflow_page[free_slot].data, 0, DATA_SIZE);
                                    std::memcpy(overflow_page[free_slot].data, record.data(), DATA_SIZE);
                                    overflow_page[free_slot].pointer = head; 

                                    page[record_num-1].pointer = free_slot + 1; 

                                    write_page(overflow_page, 1, true);
                                    write_page(page, page_num, false);
                                    return true;
                                }

                                unsigned int prev = head;
                                unsigned int curr = overflow_page[prev - 1].pointer;

                                while (curr != 0 && overflow_page[curr - 1].key < key) {
                                    prev = curr;
                                    curr = overflow_page[prev - 1].pointer;
                                }
                                if (curr != 0 && overflow_page[curr - 1].key == key) {
                                    return false;
                                }

                                overflow_page[free_slot].key = key;
                                std::memset(overflow_page[free_slot].data, 0, DATA_SIZE);
                                std::memcpy(overflow_page[free_slot].data, record.data(), DATA_SIZE);
                                overflow_page[free_slot].pointer = curr; 

                                overflow_page[prev - 1].pointer = free_slot + 1;

                                write_page(overflow_page, 1, true);
                                write_page(page, page_num, false);
                                return true;
                            }
                        } else if(key == fetched_key) {
                            return false;
                        } else if(key > fetched_key && record_num == BLOCKING_FACTOR-1 && fetched_key2 == 0) {
                            int free_index_slot = -1;
                            for (unsigned int j = 0; j < BLOCKING_FACTOR; j++) {
                                if (index_page[j].key == 0) {
                                    free_index_slot = static_cast<int>(j);
                                    break;
                                }
                            }

                            if (free_index_slot != -1) {
                                main_file.clear();
                                main_file.seekg(0, std::ios::end);
                                std::streamsize main_size = main_file.tellg();
                                unsigned int new_main_page = static_cast<unsigned int>(main_size / PAGE_SIZE) + 1;

                                index_page[free_index_slot].key = key;
                                index_page[free_index_slot].page_number = new_main_page;
                                write_index_page(index_page, index_page_num);

                                struct record *new_main_page_arr = new struct record[BLOCKING_FACTOR];
                                for (unsigned int k = 0; k < BLOCKING_FACTOR; k++) {
                                    new_main_page_arr[k].key = 0;
                                    std::memset(new_main_page_arr[k].data, 0, DATA_SIZE);
                                    new_main_page_arr[k].pointer = 0;
                                }
                                new_main_page_arr[0].key = key;
                                std::memcpy(new_main_page_arr[0].data, record.data(), std::min(record.size(), (size_t)DATA_SIZE));
                                new_main_page_arr[0].pointer = 0;

                                write_page(new_main_page_arr, new_main_page, false);
                                delete[] new_main_page_arr;

                                // std::ofstream dbg("insert_debug.log", std::ios::app);
                                // dbg << "Created new main page " << new_main_page << " for key " << key << " (index slot " << free_index_slot << ")\n";

                                return true;
                            }
                        }
                    }
                } else if(key == fetched_key || key == fetched_key2) {
                    return false;
                }
            }
            index_page_num++;
            index_page = convert_string_to_index_page(read_index_page(index_page_num));
        }
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
