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
    // index file
    if(!file_manager.index_file.is_open()) {
        output_mode();
        mvprintw(0, 0, "Index file is not available.");
        mvprintw(2, 0, "Press any key to continue...");
        refresh();
        getch();
        return;
    }

    // determine file size
    file_manager.index_file.clear();
    file_manager.index_file.seekg(0, std::ios::end);
    std::streamsize size = file_manager.index_file.tellg();
    if(size <= 0) {
        output_mode();
        mvprintw(0, 0, "Index file is empty.");
        mvprintw(2, 0, "Press any key to continue...");
        refresh();
        getch();
        return;
    }
    file_manager.index_file.seekg(0, std::ios::beg);

    size_t count = static_cast<size_t>(size) / sizeof(unsigned int);
    if(count < 2) {
        output_mode();
        mvprintw(0, 0, "Index file has no entries.");
        mvprintw(2, 0, "Press any key to continue...");
        refresh();
        getch();
        return;
    }

    std::vector<unsigned int> data(count);
    file_manager.index_file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(count * sizeof(unsigned int)));

    output_mode();
    mvprintw(0, 0, "Index file:");
    int line_count = 1;
    for(size_t i = 0; i + 1 < data.size(); i+=2) {
        mvprintw(line_count, 0, " Key: %u, PageNO: %u", data[i], data[i+1]);
        line_count++;
    }

    // main file
    mvprintw(line_count, 0, "Main file:");
    line_count++;

    if(!file_manager.main_file.is_open()) {
        mvprintw(line_count, 0, "Main file is not available.");
        line_count++;
    } else {
        file_manager.main_file.clear();
        file_manager.main_file.seekg(0, std::ios::end);
        std::streamsize main_size = file_manager.main_file.tellg();
        file_manager.main_file.seekg(0, std::ios::beg);

        if(main_size <= 0) {
            mvprintw(line_count, 0, "Main file is empty.");
            line_count++;
        } else {
            unsigned int page_count = static_cast<unsigned int>(main_size / PAGE_SIZE);
            
            for(unsigned int p = 1; p <= page_count; p++) {
                mvprintw(line_count, 0, "Page %u:", p);
                line_count++;
                
                std::string page_data = file_manager.read_page(p, false);
                
                // Display records within this page (each record is RECORD_SIZE+POINTER_SIZE)
                for(unsigned int r = 0; r < BLOCKING_FACTOR; r++) {
                    unsigned int offset = r * (RECORD_SIZE + POINTER_SIZE);
                    unsigned int key;
                    unsigned int pointer;
                    char text_buf[VALUE_SIZE + 1];

                    std::memcpy(&key, page_data.data() + offset, KEY_SIZE);
                    
                    if(key != 0) {  // Only display non-empty records
                        std::memcpy(text_buf, page_data.data() + offset + KEY_SIZE, VALUE_SIZE);
                        text_buf[VALUE_SIZE] = '\0';
                        std::memcpy(&pointer, page_data.data() + offset + RECORD_SIZE, POINTER_SIZE);
                        
                        mvprintw(line_count, 0, "  Record %u: Key: %u, Data: %s, Pointer: %u", r, key, text_buf, pointer);
                        line_count++;
                    }
                }
            }
        }
    }

    // overflow file
    mvprintw(line_count, 0, "Overflow file:");
    line_count++;

    if(!file_manager.overflow_file.is_open()) {
        mvprintw(line_count, 0, "Overflow file is not available.");
        line_count++;
    } else {
        file_manager.overflow_file.clear();
        file_manager.overflow_file.seekg(0, std::ios::end);
        std::streamsize overflow_size = file_manager.overflow_file.tellg();
        file_manager.overflow_file.seekg(0, std::ios::beg);

        if(overflow_size <= 0) {
            mvprintw(line_count, 0, "Overflow file is empty.");
            line_count++;
        } else {
            unsigned int page_count = static_cast<unsigned int>(overflow_size / PAGE_SIZE);
            
            for(unsigned int p = 1; p <= page_count; p++) {
                mvprintw(line_count, 0, "Page %u:", p);
                line_count++;
                
                std::string page_data = file_manager.read_page(p, true);
                
                // Display records within this page
                for(unsigned int r = 0; r < BLOCKING_FACTOR; r++) {
                    unsigned int offset = r * (RECORD_SIZE + POINTER_SIZE);
                    unsigned int key;
                    unsigned int pointer;
                    char text_buf[VALUE_SIZE + 1];

                    std::memcpy(&key, page_data.data() + offset, KEY_SIZE);
                    std::memcpy(&pointer, page_data.data() + offset + RECORD_SIZE, POINTER_SIZE);
                    
                    std::memcpy(text_buf, page_data.data() + offset + KEY_SIZE, VALUE_SIZE);
                    text_buf[VALUE_SIZE] = '\0';
                    
                    if(key == 0) {
                        mvprintw(line_count, 0, "  Record %u: [EMPTY]", r);
                    } else {
                        mvprintw(line_count, 0, "  Record %u: Key: %u, Data: %s, Pointer: %u", r, key, text_buf, pointer);
                    }
                    line_count++;
                }
            }
        }
    }
    
    mvprintw(line_count + 1, 0, "Press 'c to continue...");
    refresh();
    while(getch() != 'c') {};
}

void Menu::show_record(FileManager &file_manager) {
    input_mode();
    mvprintw(0, 0, "Enter key to show: ");
    char key_buf[12];
    getnstr(key_buf, 13);

    unsigned int key;
    std::stringstream ss(key_buf);
    ss >> key; 

    //file_manager.fetch();
}

void Menu::reorganize_files(FileManager &file_manager) {

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
    scrollok(stdscr, TRUE); 

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
