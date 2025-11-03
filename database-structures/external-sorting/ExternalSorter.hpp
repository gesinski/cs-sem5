#include "BlockIO.hpp"

class ExternalSorter {
private:
    const std::string FILE_NAME;
    int runs = 0;

    void read_parameters(int &blocking_factor, int &buffers_num) {
        echo();
        curs_set(1);
        clear();

        mvprintw(0, 0, "Enter blocking factor: ");
        char buffer1[16];
        mvprintw(1, 0, "> ");
        getnstr(buffer1, 15);
        blocking_factor = std::stoi(buffer1);

        mvprintw(2, 0, "Enter number of buffers: ");
        char buffer2[16];
        mvprintw(3, 0, "> ");
        getnstr(buffer2, 15);
        buffers_num = std::stoi(buffer2);

        noecho();
        curs_set(0);
        clear();

        mvprintw(0, 0, "Blocking factor: %d", blocking_factor);
        mvprintw(1, 0, "Buffers number: %d", buffers_num);
        mvprintw(2, 0, "Press any key to start sorting...");
        refresh();
        getch();
    }

    std::vector<std::string> write_buffer(const std::vector<std::string> &buffer) {
        ;
    }

    void creating_runs(int blocking_factor, int buffers_num, BlockIO &input_file_manager) {
        bool eof_reached = false;
        while(!eof_reached) {
            std::vector<std::string> buffer;
            runs++;
            for(int current_record = 0; current_record < blocking_factor * buffers_num; current_record++) {
                std::string record = input_file_manager.read_record();
                if(record == "EOF") {
                    eof_reached = true;
                    break;
                } else {
                    runs++;
                    buffer.push_back(record);
                }
            }
            std::vector<std::string> file_names = write_buffer(buffer);
        }
    }

    void merging() {
        ;
    }
public:
    ExternalSorter(const std::string &file_name) : FILE_NAME(file_name){}

    void sort() {
        int blocking_factor, buffers_num;
        read_parameters(blocking_factor, buffers_num);

        BlockIO input_file_manager(FILE_NAME);
        
        creating_runs(blocking_factor, buffers_num, input_file_manager);

        merging();
    }
};