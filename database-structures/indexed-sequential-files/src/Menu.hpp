#include <ncurses.h>
#include "FileManager.hpp"

class Menu {
private:
    enum Option {
        OPTION_1 = 1,
        OPTION_2,
        OPTION_3,
        OPTION_4,
        OPTION_5,
        OPTION_6,
        OPTION_7,
        OPTION_COUNT = 7
    };

    void print_main_options(Option current_option);

    void input_mode();

    void output_mode();

    void show_files();

    void show_record(FileManager &file_manager);

    void insert_record(FileManager &file_manager);

    void delete_record(FileManager &file_manager);

    void update_record(FileManager &file_manager);

    void show_records(FileManager &file_manager);

    void reorganize_files(FileManager &file_manager);

    std::string input_file_name();

    void include_test_file(FileManager &file_manager);

public:
    Menu();
};