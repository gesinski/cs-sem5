#include "Menu.hpp"

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

// void option_generate_records() {
// const std::string FILE_NAME = "generated_records";
// std::ofstream records_out(FILE_NAME);
// if(!records_out) {
//         mvprintw(6, 0, "Cannot open %s for writing", FILE_NAME.c_str());
//     } else {
//         input_mode();
//         mvprintw(0, 0, "Enter how many records generate: ");
//         mvprintw(1, 0, "> ");
//         char buffer[16];
//         getnstr(buffer, 15);
//         generate_records(records_out, std::stoi(buffer)); 
        
//         records_out.close();
//         output_mode();
        
//         mvprintw(0, 0, "File '%s' generated successfully. Press any key to continue...", FILE_NAME.c_str());
        
//         refresh();
//         getch();
//         ExternalSorter sorter(FILE_NAME);
//         sorter.sort();
//     }
// }

// void option_enter_records() {
//     const std::string FILE_NAME = "input_records";
//     std::ofstream records_out(FILE_NAME);
//     if(!records_out) {
//         mvprintw(6, 0, "Cannot open %s for writing", FILE_NAME.c_str());
//     } else {
//         input_mode();
//         mvprintw(0, 0, "Enter names (write q to STOP):\n");
//         std::string name;
//         int row = 1;
//         while(true) {
//             char buffer[256];
//             mvprintw(row, 0, "> ");
//             getnstr(buffer, 255);  
//             name = buffer;
//             if(name == "q" || name == "Q") 
//                 break;
//             records_out << name << '\n';  
//             row++;
//         }
//         records_out.close();
//         output_mode();
//         mvprintw(0, 0, "Names saved to %s.", FILE_NAME.c_str());
//         mvprintw(2, 0, "Press any key to continue...");
//         refresh();
//         getch();
//         ExternalSorter sorter(FILE_NAME);
//         sorter.sort();
//     }
// }

std::string Menu::insert_records_from_file(BTree &b_tree) {
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
        mvprintw(1, 0, "B-tree generated successfully");
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

    BTree b_tree;
    const std::string BTREE_FILE_NAME = "b-tree";
    std::string RECORDS_FILE_NAME = "records";
    if(start_options() == 1) {
        RECORDS_FILE_NAME = insert_records_from_file(b_tree);
    }
    else {
        clear();
        input_mode();
        mvprintw(0, 0, "Enter root record: ");
        char root_record[256];
        getnstr(root_record, 255);  

        //get record and transform it to node        

        std::ifstream records(RECORDS_FILE_NAME);
        if(!records) {
            std::ofstream create_file(RECORDS_FILE_NAME);
            if(!create_file) {
                mvprintw(0, 0, "Cannot create %s", RECORDS_FILE_NAME.c_str());
                refresh();
                getch();
                return;
            }
            create_file.close();
        } else {
            records.close();
            std::ofstream clear_file(RECORDS_FILE_NAME, std::ios::trunc);
            clear_file.close();
        }

        output_mode();
        mvprintw(0, 0, "You opened file: %s", RECORDS_FILE_NAME.c_str());
        mvprintw(2, 0, "Press any key to continue...");
        refresh();
        getch();

    }
    FileManager file_manager(RECORDS_FILE_NAME, BTREE_FILE_NAME);

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