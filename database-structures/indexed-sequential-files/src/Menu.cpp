#include "Menu.hpp"
#include <cstdint>
#include <cstring>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>

void Menu::print_main_options(Option current_option) {
    clear();
    mvprintw(0, 0, "Use UP/DOWN arrows to move, ENTER/RIGHT to select, q to quit\n\n");
    mvprintw(2, 0, "%s 1. Show record",
            (current_option == OPTION_1 ? ">" : " "));
    mvprintw(3, 0, "%s 2. Insert record",
            (current_option == OPTION_2 ? ">" : " "));
    mvprintw(4, 0, "%s 3. Delete record",
            (current_option == OPTION_3 ? ">" : " "));
    mvprintw(5, 0, "%s 4. Update record",
            (current_option == OPTION_4 ? ">" : " "));
    mvprintw(6, 0, "%s 5. Show records",
            (current_option == OPTION_5 ? ">" : " "));
    mvprintw(7, 0, "%s 6. Reorganize",
            (current_option == OPTION_6 ? ">" : " "));       
    mvprintw(8, 0, "%s 7. Get operations from file",
            (current_option == OPTION_7 ? ">" : " "));     
    refresh();
}

void Menu::input_mode() {
    echo();
    curs_set(1);
    clear();
}

void Menu::output_mode() {
    noecho();
    curs_set(0);
    clear();
}

void Menu::show_files(FileManager &file_manager) {
    const int LINES_PER_PAGE = 40;
    std::vector<std::string> all_lines;
    
    // Collect all lines to display
    if(file_manager.index_file.is_open()) {
        file_manager.index_file.clear();
        file_manager.index_file.seekg(0, std::ios::end);
        std::streamsize size = file_manager.index_file.tellg();
        file_manager.index_file.seekg(0, std::ios::beg);

        if(size > 0) {
            all_lines.push_back("=== Index file ===");
            size_t count = static_cast<size_t>(size) / sizeof(unsigned int);
            std::vector<unsigned int> data(count);
            file_manager.index_file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(count * sizeof(unsigned int)));
            
            for(size_t i = 0; i + 1 < data.size(); i+=2) {
                char buf[256];
                snprintf(buf, sizeof(buf), "Key: %u, PageNO: %u", data[i], data[i+1]);
                all_lines.push_back(buf);
            }
        } else {
            all_lines.push_back("=== Index file ===");
            all_lines.push_back("(empty)");
        }
    }

    // Main file
    if(file_manager.main_file.is_open()) {
        file_manager.main_file.clear();
        file_manager.main_file.seekg(0, std::ios::end);
        std::streamsize main_size = file_manager.main_file.tellg();
        file_manager.main_file.seekg(0, std::ios::beg);

        all_lines.push_back("");
        all_lines.push_back("=== Main file ===");
        
        if(main_size > 0) {
            unsigned int page_count = static_cast<unsigned int>(main_size / PAGE_SIZE);
            
            for(unsigned int p = 1; p <= page_count; p++) {
                char buf[256];
                snprintf(buf, sizeof(buf), "Page %u:", p);
                all_lines.push_back(buf);
                
                std::string page_data = file_manager.read_page(p, false);
                
                for(unsigned int r = 0; r < BLOCKING_FACTOR; r++) {
                    unsigned int offset = r * (RECORD_SIZE + POINTER_SIZE);
                    unsigned int key;
                    unsigned int pointer;
                    char text_buf[DATA_SIZE + 1];

                    std::memcpy(&key, page_data.data() + offset, KEY_SIZE);
                    std::memcpy(text_buf, page_data.data() + offset + KEY_SIZE, DATA_SIZE);
                    text_buf[DATA_SIZE] = '\0';
                    std::memcpy(&pointer, page_data.data() + offset + RECORD_SIZE, POINTER_SIZE);
                    
                    if(key == 0) {
                        snprintf(buf, sizeof(buf), "  Record %u: [EMPTY]", r+1);
                    } else {
                        snprintf(buf, sizeof(buf), "  Record %u: Key: %u, Data: %s, Pointer: %u", r+1, key, text_buf, pointer);
                    }
                    all_lines.push_back(buf);
                }
            }
        } else {
            all_lines.push_back("(empty)");
        }
    }

    // Overflow file
    if(file_manager.overflow_file.is_open()) {
        file_manager.overflow_file.clear();
        file_manager.overflow_file.seekg(0, std::ios::end);
        std::streamsize overflow_size = file_manager.overflow_file.tellg();
        file_manager.overflow_file.seekg(0, std::ios::beg);

        all_lines.push_back("");
        all_lines.push_back("=== Overflow file ===");
        
        if(overflow_size > 0) {
            unsigned int page_count = static_cast<unsigned int>(overflow_size / PAGE_SIZE);
            
            for(unsigned int p = 1; p <= page_count; p++) {
                char buf[256];
                
                std::string page_data = file_manager.read_page(p, true);
                
                for(unsigned int r = 0; r < BLOCKING_FACTOR; r++) {
                    unsigned int offset = r * (RECORD_SIZE + POINTER_SIZE);
                    unsigned int key;
                    unsigned int pointer;
                    char text_buf[DATA_SIZE + 1];

                    std::memcpy(&key, page_data.data() + offset, KEY_SIZE);
                    std::memcpy(text_buf, page_data.data() + offset + KEY_SIZE, DATA_SIZE);
                    text_buf[DATA_SIZE] = '\0';
                    std::memcpy(&pointer, page_data.data() + offset + RECORD_SIZE, POINTER_SIZE);
                    
                    if(key == 0) {
                        snprintf(buf, sizeof(buf), "  Record %u: [EMPTY]", r+1);
                    } else {
                        snprintf(buf, sizeof(buf), "  Record %u: Key: %u, Data: %s, Pointer: %u", r+1, key, text_buf, pointer);
                    }
                    all_lines.push_back(buf);
                }
            }
        } else {
            all_lines.push_back("(empty)");
        }
    }

    output_mode();
    int current_page = 0;
    int total_pages = (all_lines.size() + LINES_PER_PAGE - 1) / LINES_PER_PAGE;
    if(total_pages == 0) total_pages = 1;
    
    while(true) {
        clear();
        
        int start_line = current_page * LINES_PER_PAGE;
        int end_line = std::min(start_line + LINES_PER_PAGE, (int)all_lines.size());
        
        int y = 0;
        for(int i = start_line; i < end_line; i++) {
            mvprintw(y, 0, "%s", all_lines[i].c_str());
            y++;
        }
        
        mvprintw(LINES - 2, 0, "Page %d/%d | 'n' next | 'p' prev | 'q' quit", current_page + 1, total_pages);
        refresh();
        
        int ch = getch();
        if(ch == 'q' || ch == 'c') {
            break;
        } else if(ch == 'n' && current_page < total_pages - 1) {
            current_page++;
        } else if(ch == 'p' && current_page > 0) {
            current_page--;
        }
    }
}

void Menu::show_record(FileManager &file_manager) {
    input_mode();
    mvprintw(0, 0, "Enter key to show: ");
    char key_buf[12];
    getnstr(key_buf, 13);

    unsigned int key;
    std::stringstream ss(key_buf);
    ss >> key; 

    std::string record = file_manager.fetch(key);

    if(record != "-1") {
        output_mode();
        mvprintw(0, 0, "Fetched record:");
        mvprintw(1, 0, "Key: %u Record: %s", key, record.data());
        mvprintw(3, 0, "Disk reads: %d", file_manager.disk_reads);
        mvprintw(4, 0, "Disk writes: %d", file_manager.disk_writes);
        mvprintw(6, 0, "Press any key to continue...");
        refresh();
    } else {
        output_mode();
        mvprintw(0, 0, "Wanted record doesn't exist.");
        mvprintw(2, 0, "Press any key to continue...");
        refresh();
    }
    getch();
}

void Menu::reorganize_files(FileManager &file_manager) {
    file_manager.reorganize();

    output_mode();
    mvprintw(0, 0, "Record reorganized successfully.");
    mvprintw(1, 0, "Disk reads: %d", file_manager.disk_reads);
    mvprintw(2, 0, "Disk writes: %d", file_manager.disk_writes);
    mvprintw(3, 0, "Press 'v' to view files or any key to continue...");
    refresh();
        
    int button = getch();
    if(button == 'v') {
        show_files(file_manager);
    }
}

void Menu::insert_record(FileManager &file_manager) {
    input_mode();
    mvprintw(0, 0, "Enter key and record: ");

    char kr_buf[255];
    getnstr(kr_buf, 256);

    unsigned int key;
    std::string record;
    std::stringstream ss(kr_buf);
    ss >> key;
    ss >> std::ws;
    std::getline(ss, record);

    if(file_manager.insert(key, record)) {
        output_mode();
        mvprintw(0, 0, "Record inserted successfully.");
        mvprintw(1, 0, "Disk reads: %d", file_manager.disk_reads);
        mvprintw(2, 0, "Disk writes: %d", file_manager.disk_writes);
        mvprintw(3, 0, "Press 'v' to view files or any key to continue...");
        refresh();
        
        int button = getch();
        if(button == 'v') {
            show_files(file_manager);
        }

        if(file_manager.records_overflow == BLOCKING_FACTOR) {
            mvprintw(0, 0, "Reorganization is needed.");
            mvprintw(2, 0, "Press any key to continue...");
            refresh();
            getch();
            reorganize_files(file_manager);
        }
    } else {
        output_mode();
        mvprintw(0, 0, "You entered key that already exists.");
        mvprintw(2, 0, "Press any key to continue...");
        refresh();
        getch();
    }
}

void Menu::delete_record(FileManager &file_manager) {

}

void Menu::update_record(FileManager &file_manager) {

}

void Menu::show_records(FileManager &file_manager) {
    const int LINES_PER_PAGE = 40;

    std::vector<std::string> all_lines = file_manager.fetch_records();

    if(all_lines.size() == 0) {
        all_lines.push_back("No records have been added.");
    }

    output_mode();
    int current_page = 0;
    int total_pages = (all_lines.size() + LINES_PER_PAGE - 1) / LINES_PER_PAGE;
    if(total_pages == 0) total_pages = 1;
    
    while(true) {
        clear();
        
        int start_line = current_page * LINES_PER_PAGE;
        int end_line = std::min(start_line + LINES_PER_PAGE, (int)all_lines.size());
        
        int y = 0;
        for(int i = start_line; i < end_line; i++) {
            mvprintw(y, 0, "%s", all_lines[i].c_str());
            y++;
        }
        
        mvprintw(LINES - 2, 0, "Page %d/%d | 'n' next | 'p' prev | 'q' quit", current_page + 1, total_pages);
        refresh();
        
        int ch = getch();
        if(ch == 'q' || ch == 'c') {
            break;
        } else if(ch == 'n' && current_page < total_pages - 1) {
            current_page++;
        } else if(ch == 'p' && current_page > 0) {
            current_page--;
        }
    }
    output_mode();
    mvprintw(0, 0, "Disk operations needed for showing records:");
    mvprintw(2, 0, "Disk reads: %d", file_manager.disk_reads);
    mvprintw(3, 0, "Disk writes: %d", file_manager.disk_writes);
    mvprintw(5, 0, "Press any key to continue...");
    refresh();
    getch();
}

std::string Menu::input_file_name() {
    input_mode();
    mvprintw(0, 0, "Enter filename: ");

    char FILE_NAME[256];
    getnstr(FILE_NAME, 255);

    std::ifstream records(FILE_NAME);

    if(!records.is_open()) {
        output_mode();
        mvprintw(0, 0, "File '%s' does not exist.", FILE_NAME);
        mvprintw(2, 0, "Press any key to continue...");
        refresh();
        getch();
    } 
    else {
        output_mode();
        mvprintw(0, 0, "You entered file: %s", FILE_NAME);
        mvprintw(2, 0, "Press any key to continue...");
        refresh();
        getch();

        records.close();
    }

    return FILE_NAME;
}

void Menu::include_test_file(FileManager &file_manger) {
    std::string test_records_file = "test_file"; //input_file_name();
}

Menu::Menu() {
    initscr();
    output_mode();
    cbreak();
    keypad(stdscr, TRUE);
    scrollok(stdscr, FALSE); 
    mousemask(ALL_MOUSE_EVENTS, NULL);

    const std::string main_file_name = "main_file";
    const std::string index_file_name = "index_file";
    const std::string overflow_file_name = "overflow_file";
    
    FileManager file_manager(main_file_name, index_file_name, overflow_file_name);

    Option current_option = OPTION_1;
    print_main_options(current_option);

    int button;
    do {
        button = getch();
        switch(button) {
            case KEY_UP:
                if(current_option > OPTION_1)
                    current_option = static_cast<Option>(current_option - 1);
                break;
            case KEY_DOWN:
                if(current_option < OPTION_COUNT)
                    current_option = static_cast<Option>(current_option + 1);
                break;
            case 10:        // '\n'
            case 13:        // '\r'
            case KEY_ENTER:
            case KEY_RIGHT:
                switch(current_option) {
                    case OPTION_1: 
                        show_record(file_manager);
                        break;
                    case OPTION_2: 
                        insert_record(file_manager);
                        break;
                    case OPTION_3: 
                        delete_record(file_manager);
                        break;
                    case OPTION_4: 
                        update_record(file_manager);
                        break;
                    case OPTION_5: 
                        show_records(file_manager);
                        break;
                    case OPTION_6: 
                        reorganize_files(file_manager);
                        break;
                    case OPTION_7:
                        include_test_file(file_manager);
                        break;
                }
                print_main_options(current_option);
                break;
        }
        print_main_options(current_option);
    } while(button != 'q');
    endwin();
}
