#include <ncurses.h>
#include <string>
#include <vector>
#include <fstream>
#include "generator.hpp"

class Menu {
private:
    enum Option {
        OPTION_1 = 1,
        OPTION_2,
        OPTION_3,
        OPTION_COUNT = 3
    };

    void print_main_options(Option current_option) {
        clear();
        mvprintw(0, 0, "Use UP/DOWN arrows to move, ENTER/RIGHT to select, q to quit\n\n");

        mvprintw(2, 0, "%s 1. Generate records",
                (current_option == OPTION_1 ? ">" : " "));
        mvprintw(3, 0, "%s 2. Enter records",
                (current_option == OPTION_2 ? ">" : " "));
        mvprintw(4, 0, "%s 3. Get records from file",
                (current_option == OPTION_3 ? ">" : " "));

        refresh();
    }

    void option_generate_records() {
        const std::string file_name = "generated_records";
        std::ofstream records_out(file_name);

        if(!records_out) {
            mvprintw(6, 0, "Cannot open %s for writing", file_name.c_str());
        } else {
            generate_records(records_out); 
            mvprintw(6, 0, "File '%s' generated successfully.", file_name.c_str());
            records_out.close();
        }

        refresh();
        getch();
    }

    void option_enter_records() {
        const std::string file_name = "input_records";
        std::ofstream records_out(file_name);

        if(!records_out) {
            mvprintw(6, 0, "Cannot open %s for writing", file_name.c_str());
        } else {
            echo();
            curs_set(1);
            clear();
            mvprintw(0, 0, "Enter names (write q to STOP):\n");

            std::string name;
            int row = 1;
            while(true) {
                char buffer[256];
                mvprintw(row, 0, "> ");
                getnstr(buffer, 255);  
                name = buffer;

                if(name == "q" || name == "Q") 
                    break;

                records_out << name << '\n';  
                row++;
            }
            noecho();
            curs_set(0);
            clear();
            mvprintw(0, 0, "Names saved to %s.", file_name.c_str());
            mvprintw(2, 0, "Press any key to continue...");
            refresh();
            getch();
        }
    }

    void option_file_records() {
        echo();
        curs_set(1);
        clear();
        mvprintw(0, 0, "Enter filename: ");
        char file_name[256];
        getnstr(file_name, 255);  
        noecho();
        curs_set(0);

        // clear();
        // mvprintw(0, 0, "You entered file: %s", filename);
        // mvprintw(2, 0, "Press any key to return to menu...");
        // refresh();
        // getch();
    }

public:
    Menu() {
        initscr();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);
        curs_set(0);

        int button;
        Option current_option = OPTION_1;

        print_main_options(current_option);

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
                            option_generate_records();
                            break;
                        case OPTION_2: 
                            option_enter_records();
                            break;
                        case OPTION_3: 
                            option_file_records();
                            break;
                    }

                    print_main_options(current_option);
                    break;
            }
            print_main_options(current_option);

        } while(button != 'q');

        endwin();
    }
};