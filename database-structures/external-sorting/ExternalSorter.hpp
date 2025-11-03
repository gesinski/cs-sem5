#include "BlockIO.hpp"
#include <unistd.h>
#include <cstring> 
#include <algorithm>

class ExternalSorter {
private:
    const std::string FILE_NAME;
    int runs = 0;
    bool simulate_till_end = false;

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

    std::string create_temp_file(const std::string &prefix) {
        std::string tmpl = "/tmp/" + prefix + "XXXXXX"; 
        std::vector<char> buf(tmpl.begin(), tmpl.end());
        buf.push_back('\0');
        int fd = mkstemp(buf.data());
        if (fd == -1) {
            throw std::runtime_error("mkstemp failed");
        }
        close(fd);
        return std::string(buf.data());
    }

    void show_sorted_file(const std::vector<std::string> &buffer) {
        clear();
        mvprintw(0, 0, "Sorted records in the current run:");
        for(size_t i = 0; i < buffer.size(); i++) {
            mvprintw(i + 1, 0, "%s", buffer[i].c_str());
        }
        mvprintw(buffer.size() + 2, 0, "Press any key to continue...");
        refresh();
        getch();
    }

    void creating_runs(int blocking_factor, int buffers_num, BlockIO &file_manager) {
        bool eof_reached = false;
        std::vector<std::string> file_names;
        while(!eof_reached) {
            std::vector<std::string> buffer;
            runs++;
            for(int current_record = 0; current_record < blocking_factor * buffers_num; current_record++) {
                std::string record = file_manager.read_record();
                if(record == "EOF") {
                    eof_reached = true;
                    break;
                } else {
                    runs++;
                    buffer.push_back(record);
                }
            }

            std::sort(buffer.begin(), buffer.end());
            
            std::string tape_name = create_temp_file("tape");
            file_names.push_back(tape_name);
            std::ofstream tape(tape_name);

            for(std::string &record : buffer) {
                file_manager.write_record(tape, record);
            }

            tape.close();

            if(!simulate_till_end) {
                clear();
                mvprintw(0, 0, "Created run %d with %lu records.", runs, buffer.size());
                mvprintw(1, 0, "Press 's' to simulate till end, any other key to show sorted file...");
                refresh();
                int ch = getch();
                if(ch == 's' || ch == 'S') {
                    simulate_till_end = true;
                }
                else {
                    show_sorted_file(buffer);
                }
            } 
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

        BlockIO file_manager(FILE_NAME);
        
        creating_runs(blocking_factor, buffers_num, file_manager);

        merging();
    }
};