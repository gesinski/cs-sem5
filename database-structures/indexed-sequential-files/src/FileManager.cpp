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

    if(overflow)
        records_overflow = 0;
    
    for(unsigned int i = 0; i < BLOCKING_FACTOR; i++) {
        std::memcpy(page + i*(RECORD_SIZE+POINTER_SIZE), &record_page[i].key, KEY_SIZE);
        std::memcpy(page + i*(RECORD_SIZE+POINTER_SIZE) + KEY_SIZE, &record_page[i].data, DATA_SIZE);
        std::memcpy(page + i*(RECORD_SIZE+POINTER_SIZE) + RECORD_SIZE, &record_page[i].pointer, POINTER_SIZE);

        if(overflow && record_page[i].key != 0) {
            records_overflow++;
        }
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

std::string FileManager::fetch(unsigned int key) {
    disk_reads = 0;
    disk_writes = 0;

    index_file.clear();
    index_file.seekg(0, std::ios::end);
    std::streamsize index_file_size = index_file.tellg();
    unsigned int index_page_num = static_cast<unsigned int>(index_file_size / INDEX_PAGE_SIZE);

    for(unsigned int index_page_count = 0; index_page_count < index_page_num; index_page_count++) {
        struct index_record *index_page = convert_string_to_index_page(read_index_page(index_page_count+1));
        
        for(unsigned int index_record = 0; index_record < BLOCKING_FACTOR; index_record++) {

            unsigned int fetched_key = index_page[index_record].key;
            if(key < fetched_key && index_record == 0) {
                return "-1";
            } else if(key >= fetched_key) {
                if(index_record < BLOCKING_FACTOR-1) {
                    fetched_key = index_page[index_record+1].key;

                    if(key > fetched_key && fetched_key != 0) {
                        continue;
                    } else {
                        struct record *main_page = convert_string_to_page(read_page(index_page[index_record].page_number, false));

                        for(unsigned int main_record = 0; main_record < BLOCKING_FACTOR; main_record++) {
                            fetched_key = main_page[main_record].key;

                            if(key == fetched_key) {
                                return main_page[main_record].data;
                            } else if(key > fetched_key) {
                                if(key < main_page[main_record+1].key) {
                                    struct record *overflow_page = convert_string_to_page(read_page(1, true));
                                    unsigned int overflow_pointer = main_page[main_record].pointer;
                                    unsigned int count = 0;
                                    while(count < BLOCKING_FACTOR) {
                                        if(overflow_pointer == 0) 
                                            return "-1";
                                        if(key == overflow_page[overflow_pointer-1].key) {
                                            return overflow_page[overflow_pointer-1].data;
                                        }
                                        overflow_pointer = overflow_page[overflow_pointer-1].pointer;
                                        count++;
                                    }
                                    return "-1";
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return "-1";
}

bool FileManager::insert(unsigned int key, std::string record) {
    // debug log
    // {
    //     std::ofstream dbg("insert_debug.log", std::ios::app);
    //     dbg << "insert called key=" << key << " record='" << record << "' smallest_key=" << smallest_key << "\n";
    // }
    disk_reads = 0;
    disk_writes = 0;

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
        
        while(index_page[0].key != 0) {
            for(unsigned int i = 0; i < BLOCKING_FACTOR; i++) {
                unsigned int fetched_key = index_page[i].key, fetched_key2;
                if(i+1 < BLOCKING_FACTOR)
                    fetched_key2 = index_page[i+1].key;
                else {
                    unsigned int next_index_page = index_page_num + 1;
                    index_file.seekg(0, std::ios::end);
                    std::streampos size = index_file.tellg();

                    if ((next_index_page - 1) * INDEX_PAGE_SIZE >= size) {
                        fetched_key2 = 0;   
                    } else {
                        index_page = convert_string_to_index_page(read_index_page(next_index_page));
                        fetched_key2 = index_page[0].key;
                    }

                    index_page = convert_string_to_index_page(read_index_page(index_page_num));
                }

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
                        } else if(key == fetched_key2) {
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
                        } else if(key > fetched_key && record_num == BLOCKING_FACTOR-1) {
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
                                    
                                        page[record_num].pointer = ov_record_num+1;
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
    return false;
}

std::vector<std::string> FileManager::fetch_records() {
    disk_reads = 0;
    disk_writes = 0;
    
    index_file.clear();
    index_file.seekg(0, std::ios::end);
    std::streamsize index_file_size = index_file.tellg();
    unsigned int index_pages_num = static_cast<unsigned int>(index_file_size / INDEX_PAGE_SIZE);

    std::vector<std::string> records;
    unsigned int records_count = 0;

    struct record *overflow_page = convert_string_to_page(read_page(1, true));

    for(unsigned int no_index_page = 1; no_index_page-1 < index_pages_num; no_index_page++) {

        struct index_record *index_page = convert_string_to_index_page(read_index_page(no_index_page));

        for(unsigned int no_index = 0; no_index < BLOCKING_FACTOR; no_index++) {
            unsigned int page_pointer = index_page[no_index].page_number;

            if(page_pointer > 0) {

                struct record *main_page = convert_string_to_page(read_page(page_pointer, false));

                for(unsigned int no_record = 0; no_record < BLOCKING_FACTOR; no_record++) {
                    unsigned int key = main_page[no_record].key;
                    if(key > 0) {
                        char buf[256];
                        unsigned int pointer;
                        char text_buf[DATA_SIZE + 1];

                        std::memcpy(text_buf, main_page[no_record].data, DATA_SIZE);
                        text_buf[DATA_SIZE] = '\0';
                        std::memcpy(&pointer, &main_page[no_record].pointer, POINTER_SIZE);
                                    
                        records_count++;
                        if(key != 0) 
                            snprintf(buf, sizeof(buf), "Record %u: Key: %u, Data: %s, Pointer: %u", records_count, key, text_buf, pointer);
                            
                        records.push_back(buf);

                        while(pointer != 0) {
                            key = overflow_page[pointer-1].key;

                            if(key > 0) {
                                std::memcpy(text_buf, overflow_page[pointer-1].data, DATA_SIZE);
                                text_buf[DATA_SIZE] = '\0';
                                std::memcpy(&pointer, &overflow_page[pointer-1].pointer, POINTER_SIZE);

                                records_count++;
                                if(key != 0) 
                                    snprintf(buf, sizeof(buf), "Record %u: Key: %u, Data: %s, Pointer: %u", records_count, key, text_buf, pointer);
                                    
                                records.push_back(buf);
                            }
                        }
                    }
                }
            }
        }
    }

    return records;
}

void FileManager::reorganize() {
    disk_reads = 0;
    disk_writes = 0;

    std::fstream new_main_file;
    new_main_file.open("new_main_file", std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    new_main_file.close();
    new_main_file.open("new_main_file", std::ios::in | std::ios::out | std::ios::binary);

    std::fstream new_index_file;
    new_index_file.open("new_index_file", std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    new_index_file.close();
    new_index_file.open("new_index_file", std::ios::in | std::ios::out | std::ios::binary);

    main_file.clear();
    main_file.seekg(0, std::ios::end);
    std::streamsize main_file_size = main_file.tellg();
    main_file.seekg(0);

    unsigned int no_main_records = static_cast<unsigned int>(main_file_size / (RECORD_SIZE+POINTER_SIZE));
    unsigned int new_page_used_slots = 0;
    unsigned int free_slots = (unsigned int)std::ceil(alpha*BLOCKING_FACTOR);
    unsigned int new_page_count = 1;
    unsigned int index_page_records = 0;
    unsigned int index_page_count = 0;
    unsigned int ov_pointer = 0;
    unsigned int page_count = 1;

    struct index_record *index_page = new struct index_record[BLOCKING_FACTOR];
    for (unsigned int k = 0; k < BLOCKING_FACTOR; k++) {
        index_page[k].key = 0;
        index_page[k].page_number = 0;
    }
    
    struct record *new_page = new struct record[BLOCKING_FACTOR];
    for (unsigned int k = 0; k < BLOCKING_FACTOR; k++) {
        new_page[k].key = 0;
        std::memset(new_page[k].data, 0, DATA_SIZE);
        new_page[k].pointer = 0;
    }

    struct record *overflow_page = convert_string_to_page(read_page(1, true));
    struct record *page;

    unsigned int current_main_record = 0;
    while(current_main_record < no_main_records) {

        unsigned int current_page_record =  current_main_record % BLOCKING_FACTOR;

        if(current_page_record == 0 && ov_pointer == 0) {
            page = convert_string_to_page(read_page(page_count, false));
            page_count++;
        }

        {
        std::ofstream dbg("insert_debug.log", std::ios::app);
        dbg << "current_page_record called key=" << page[current_page_record].key << "Ov pointer" <<ov_pointer << "\n";
    }
        
        if(page[current_page_record].key == 0 && ov_pointer == 0) {
            current_main_record++;
            continue;
        }

        if(new_page_used_slots == free_slots) {

            new_page_used_slots = 0;

            index_page[index_page_records].key = new_page[0].key;
            index_page[index_page_records].page_number = new_page_count;
            index_page_records++;

            if(index_page_records == BLOCKING_FACTOR) {

                index_page_records = 0;

                char index_page_buf[INDEX_PAGE_SIZE];
                std::memset(index_page_buf, 0, INDEX_PAGE_SIZE);
                        
                for(unsigned int i = 0; i < BLOCKING_FACTOR; i++) {
                    std::memcpy(index_page_buf + i*(KEY_SIZE+POINTER_SIZE), &index_page[i].key, KEY_SIZE);
                    std::memcpy(index_page_buf + i*(KEY_SIZE+POINTER_SIZE) + KEY_SIZE, &index_page[i].page_number, POINTER_SIZE);
    {
        std::ofstream dbg("insert_debug.log", std::ios::app);
        dbg << "index1 called key=" << index_page[i].key << " page number='" << index_page[i].page_number << "' index_page_count=" << index_page_count << "\n";
    }
                }
                    
                std::streampos offset = static_cast<std::streampos>(INDEX_PAGE_SIZE) * (index_page_count);

                new_index_file.seekp(offset);
                new_index_file.write(index_page_buf, INDEX_PAGE_SIZE);
                new_index_file.flush();
                disk_writes++;

                index_page_count++;

                delete[] index_page;
                index_page = new struct index_record[BLOCKING_FACTOR];
                for (unsigned int k = 0; k < BLOCKING_FACTOR; k++) {
                    index_page[k].key = 0;
                    index_page[k].page_number = 0;
                }
            }

            char page_buf[PAGE_SIZE];
            std::memset(page_buf, 0, PAGE_SIZE);
                    
            for(unsigned int i = 0; i < BLOCKING_FACTOR; i++) {
                std::memcpy(page_buf + i*(RECORD_SIZE+POINTER_SIZE), &new_page[i].key, KEY_SIZE);
                std::memcpy(page_buf + i*(RECORD_SIZE+POINTER_SIZE) + KEY_SIZE, &new_page[i].data, DATA_SIZE);
                std::memcpy(page_buf + i*(RECORD_SIZE+POINTER_SIZE) + RECORD_SIZE, &new_page[i].pointer, POINTER_SIZE);
    {
        std::ofstream dbg("insert_debug.log", std::ios::app);
        dbg << "main1 called key=" << new_page[i].key << " record='" << new_page[i].data << "' new_page_count=" << new_page_count << "\n";
    }
            }

            std::streampos offset = static_cast<std::streampos>(PAGE_SIZE) * (new_page_count-1);
                    
            new_main_file.seekp(offset);
            new_main_file.write(page_buf, PAGE_SIZE);
            new_main_file.flush();
            disk_writes++;
            new_page_count++;

            delete[] new_page;
            new_page = new struct record[BLOCKING_FACTOR];
            for (unsigned int k = 0; k < BLOCKING_FACTOR; k++) {
                new_page[k].key = 0;
                std::memset(new_page[k].data, 0, DATA_SIZE);
                new_page[k].pointer = 0;
            }
        }
        if(ov_pointer != 0) {
            new_page[new_page_used_slots].key = overflow_page[ov_pointer-1].key;
            std::memset(new_page[new_page_used_slots].data, 0, DATA_SIZE);
            std::memcpy(new_page[new_page_used_slots].data, overflow_page[ov_pointer-1].data, DATA_SIZE);
            new_page[new_page_used_slots].pointer = 0;
            new_page_used_slots++;

            {
        std::ofstream dbg("insert_debug.log", std::ios::app);
        dbg << "ovferlow key key=" <<  overflow_page[ov_pointer-1].key << "\n";
    }

            ov_pointer = overflow_page[ov_pointer-1].pointer;
        } else if(page[current_page_record].key != 0){
            new_page[new_page_used_slots].key = page[current_page_record].key;
            std::memset(new_page[new_page_used_slots].data, 0, DATA_SIZE);
            std::memcpy(new_page[new_page_used_slots].data, page[current_page_record].data, DATA_SIZE);
            new_page[new_page_used_slots].pointer = 0;
            new_page_used_slots++;
            current_main_record++;

            ov_pointer = page[current_page_record].pointer;
        }
    }

    if(new_page_used_slots > 0) {
        if(new_page[0].key != 0) {
            char page_buf[PAGE_SIZE];
            std::memset(page_buf, 0, PAGE_SIZE);
            for(unsigned int i = 0; i < new_page_used_slots; i++) {
                std::memcpy(page_buf + i*(RECORD_SIZE+POINTER_SIZE), &new_page[i].key, KEY_SIZE);
                std::memcpy(page_buf + i*(RECORD_SIZE+POINTER_SIZE) + KEY_SIZE, &new_page[i].data, DATA_SIZE);
                std::memcpy(page_buf + i*(RECORD_SIZE+POINTER_SIZE) + RECORD_SIZE, &new_page[i].pointer, POINTER_SIZE);// debug log
        {
            std::ofstream dbg("insert_debug.log", std::ios::app);
            dbg << "main2 called key=" << new_page[i].key << " record='" << new_page[i].data << "' new_page_count=" << new_page_count << "\n";
        }
            }
            std::streampos offset = static_cast<std::streampos>(PAGE_SIZE) * (new_page_count-1);
            new_main_file.seekp(offset);
            new_main_file.write(page_buf, PAGE_SIZE);
            new_main_file.flush();
            disk_writes++;
        }
    } 

    if(index_page_records > 0 || new_page[0].key != 0) {
        if(new_page[0].key != 0) {
            index_page[index_page_records].key = new_page[0].key;
            index_page[index_page_records].page_number = new_page_count;
            index_page_records++;
            {
        std::ofstream dbg("insert_debug.log", std::ios::app);
        dbg << "index new_page[0].key called key=" << new_page[0].key << " page number='" << new_page_count << "' index_page_count=" << index_page_count << "\n";
    }
        }
        char index_page_buf[INDEX_PAGE_SIZE];
        std::memset(index_page_buf, 0, INDEX_PAGE_SIZE);
        for(unsigned int i = 0; i < index_page_records; i++) {
            std::memcpy(index_page_buf + i*(KEY_SIZE+POINTER_SIZE), &index_page[i].key, KEY_SIZE);
            std::memcpy(index_page_buf + i*(KEY_SIZE+POINTER_SIZE) + KEY_SIZE, &index_page[i].page_number, POINTER_SIZE);
            {
        std::ofstream dbg("insert_debug.log", std::ios::app);
        dbg << "index2 called key=" << index_page[i].key << " page number='" << index_page[i].page_number << "' index_page_count=" << index_page_count << "\n";
    }
        }
        std::streampos offset = static_cast<std::streampos>(index_page_count * INDEX_PAGE_SIZE);
        new_index_file.seekp(offset);
        new_index_file.write(index_page_buf, INDEX_PAGE_SIZE);
        new_index_file.flush();
        disk_writes++;
    }
    {
            std::ofstream dbg("insert_debug.log", std::ios::app);
            dbg << "\n";
        }

    main_file.close();
    index_file.close();

    std::remove("main_file");      
    std::remove("index_file");

    std::rename("new_main_file", "main_file");
    std::rename("new_index_file", "index_file");

    main_file.open("main_file", std::ios::in | std::ios::out | std::ios::binary);
    index_file.open("index_file", std::ios::in | std::ios::out | std::ios::binary);

    char zero_overflow[PAGE_SIZE];
    std::memset(zero_overflow, 0, PAGE_SIZE);
    overflow_file.seekp(0);
    overflow_file.write(zero_overflow, PAGE_SIZE);
    overflow_file.flush();
    records_overflow = 0;
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
