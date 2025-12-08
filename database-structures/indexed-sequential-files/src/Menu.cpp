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

void Menu::show_files() {

}

void Menu::show_record(FileManager &file_manager) {

}

void Menu::insert_record(FileManager &file_manager) {

}

void Menu::delete_record(FileManager &file_manager) {

}

void Menu::update_record(FileManager &file_manager) {

}

void Menu::show_records(FileManager &file_manager) {

}

void Menu::reorganize_files(FileManager &file_manager) {

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
    idlok(stdscr, TRUE);

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