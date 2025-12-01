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
    mvprintw(6, 0, "%s 5. Show file",
            (current_option == OPTION_5 ? ">" : " "));
    mvprintw(7, 0, "%s 6. Get operations from file",
            (current_option == OPTION_6 ? ">" : " "));      
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

void Menu::show_record(BTree &b_tree, FileManager &file_manager) {

}

void Menu::insert_record(BTree &b_tree, FileManager &file_manager) {

}

void Menu::delete_record(BTree &b_tree, FileManager &file_manager) {

}

void Menu::update_record(BTree &b_tree, FileManager &file_manager) {

}

void Menu::show_btree(BTree &b_tree, FileManager &file_manager) {

}

void Menu::get_operations_from_file(BTree &b_tree, FileManager &file_manager) {

}

void Menu::print_start_options(StartOption current_option) {
    clear();
    mvprintw(0, 0, "Use UP/DOWN arrows to move, ENTER/RIGHT to select\n\n");
    mvprintw(2, 0, "%s 1. Insert records from file",
            (current_option == START_OPTION_1 ? ">" : " "));
    mvprintw(3, 0, "%s 2. Insert root record",
            (current_option == START_OPTION_2 ? ">" : " "));
    refresh();
}

int Menu::start_options() {
    StartOption current_option = START_OPTION_1;
    print_start_options(current_option); 

    while(true) {
        int button = getch();;
        switch(button) {
            case KEY_UP:
                if(current_option > START_OPTION_1)
                    current_option = static_cast<StartOption>(current_option - 1);
                break;
            case KEY_DOWN:
                if(current_option < START_OPTION_COUNT)
                    current_option = static_cast<StartOption>(current_option + 1);
                break;
            case 10:        // '\n'
            case 13:        // '\r'
            case KEY_ENTER:
            case KEY_RIGHT:
                switch(current_option) {
                    case START_OPTION_1: 
                        return 1;
                        break;
                    case START_OPTION_2: 
                        return 2;
                        break;
                }
                break;
        }
        print_start_options(current_option);
    }
}

std::string Menu::insert_file_name() {
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


Menu::Menu() {
    initscr();
    output_mode();
    cbreak();
    keypad(stdscr, TRUE);

    const std::string BTREE_FILE_NAME = "b-tree";
    std::string RECORDS_FILE_NAME = "records";
    if(start_options() == 1) {
        RECORDS_FILE_NAME = "initial_records"; //insert_file_name();
    }
    else {
        clear();
        input_mode();
        mvprintw(0, 0, "Enter root record: ");
        char root_record[256];
        getnstr(root_record, 255);  

        //long long page = 1;

        std::string s(root_record);
        std::istringstream iss(s);

        unsigned long long tmp = 0;
        if(!(iss >> tmp)) {
            tmp = 0;
        }
        unsigned int key = static_cast<uint32_t>(tmp);

        std::string last, first;
        if(!(iss >> last)) {
            last.clear();
            first.clear();
        } else {
            std::string rest;
            std::getline(iss, rest);

            auto trim_inplace = [](std::string &t) {
                t.erase(t.begin(), std::find_if(t.begin(), t.end(), [](unsigned char ch){ return !std::isspace(ch); }));
                t.erase(std::find_if(t.rbegin(), t.rend(), [](unsigned char ch){ return !std::isspace(ch); }).base(), t.end());
            };

            trim_inplace(rest);
            first = rest;
        }

        std::string fullname;
        if (last.empty()) fullname = first;
        else if (first.empty()) fullname = last;
        else fullname = last + " " + first;

        char record_out[RECORD_SIZE];
        std::memcpy(record_out, &key, sizeof(key));

        if (fullname.size() > 28) fullname.resize(28);
        std::memset(record_out + 4, ' ', 28);
        if (!fullname.empty())
            std::memcpy(record_out + 4, fullname.data(), fullname.size());

        std::ifstream records(RECORDS_FILE_NAME);
        if(!records) {
            std::ofstream create_file(RECORDS_FILE_NAME);
            if(!create_file) {
                mvprintw(0, 0, "Cannot create %s", RECORDS_FILE_NAME.c_str());
                refresh();
                getch();
                return;
            }
            create_file.write(record_out, RECORD_SIZE);
            create_file.close();
        } else {
            records.close();
            std::ofstream clear_file(RECORDS_FILE_NAME, std::ios::trunc);
            clear_file.write(record_out, RECORD_SIZE); 
            clear_file.close();
        }

        output_mode();
        mvprintw(0, 0, "You opened file: %s", RECORDS_FILE_NAME.c_str());
        mvprintw(1, 0, "%u", key);
        mvprintw(2, 0, "Press any key to continue...");
        refresh();
        getch();
    }

    //unsigned int d = 2; // tree order
    FileManager file_manager(RECORDS_FILE_NAME, BTREE_FILE_NAME);
    BTree b_tree;

    b_tree.create_btree(file_manager);

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
                        show_record(b_tree, file_manager);
                        break;
                    case OPTION_2: 
                        insert_record(b_tree, file_manager);
                        break;
                    case OPTION_3: 
                        delete_record(b_tree, file_manager);
                        break;
                    case OPTION_4: 
                        update_record(b_tree, file_manager);
                        break;
                    case OPTION_5: 
                        show_btree(b_tree, file_manager);
                        break;
                    case OPTION_6: 
                        get_operations_from_file(b_tree, file_manager);
                        break;
                }
                print_main_options(current_option);
                break;
        }
        print_main_options(current_option);
    } while(button != 'q');
    endwin();
}