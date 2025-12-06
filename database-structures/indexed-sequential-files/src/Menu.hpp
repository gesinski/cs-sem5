#include <ncurses.h>
#define RECORD_SIZE 32

class Menu {
private:
    enum Option {
        OPTION_1 = 1,
        OPTION_2,
        OPTION_3,
        OPTION_4,
        OPTION_5,
        OPTION_6,
        OPTION_COUNT = 6
    };

    enum StartOption {
        START_OPTION_1 = 1,
        START_OPTION_2,
        START_OPTION_COUNT = 2
    };

    void print_main_options(Option current_option);

    void input_mode();

    void output_mode();

    void show_record(BTree &b_tree, FileManager &file_manager);

    void insert_record(BTree &b_tree, FileManager &file_manager);

    void delete_record(BTree &b_tree, FileManager &file_manager);

    void update_record(BTree &b_tree, FileManager &file_manager);

    void show_btree(BTree &b_tree, FileManager &file_manager);

    void get_operations_from_file(BTree &b_tree, FileManager &file_manager);

    void print_start_options(StartOption current_option);

    int start_options();

    std::string insert_file_name();

public:
    Menu();
};