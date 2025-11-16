#include <string>
#include <ncurses.h>
#include <vector>
#include <fstream>
#define MAX_RECORD_LENGTH 32

class BlockIO {
public:
    std::ifstream file;
    int blocking_factor;
    int disk_reads = 0, disk_writes = 0;
    int disk_operations = 0;

    struct file_state {
        std::vector<std::string> block;
        size_t index = 0;  
        bool eof = false;
    };

    BlockIO(const std::string &file_name, const int blocking_factor) : blocking_factor(blocking_factor) {
        file.open(file_name);
        if(!file) {
            mvprintw(0, 0, "Cannot open %s for reading", file_name.c_str());
            refresh();
            getch();
        }
    }

    std::vector<std::string> read_records(std::ifstream &input_file) {
        std::vector<std::string> reading_buffer;
        std::string record;

        for(int i = 0; i < blocking_factor && std::getline(input_file, record); ++i) {
            reading_buffer.push_back(record);
        }

        if(!reading_buffer.empty()) {
            disk_reads++;
            disk_operations++;
            return reading_buffer;
        } else {
            return {"EOF"};  
        }
    }

    std::vector<std::string> read_starting_records( std::vector<std::ifstream> &run_files) {
        std::vector<std::string> buffer;

        for(size_t start = 0; start < run_files.size(); start += blocking_factor) {

            size_t end = std::min(start + blocking_factor, run_files.size());

            for(size_t i = start; i < end; i++) {
                std::string record;
                if(std::getline(run_files[i], record)) {
                    buffer.push_back(record);
                }
                else
                    return buffer;
            }
            disk_reads++;
            disk_operations++;
        }

        return buffer;
    }

    void read_block(std::ifstream &input_file, file_state &state) {
        state.block.clear();
        state.index = 0;

        std::string record;
        int count = 0;

        while(count < blocking_factor && std::getline(input_file, record)) {
            state.block.push_back(record);
            count++;
        }

        if(count == 0) {
            state.eof = true;
            return;
        }

        disk_reads++;
        disk_operations++;
    }

    std::string read_record(std::ifstream &input_file, file_state &state) {
        if(state.index >= state.block.size()) {
            if (state.eof)
                return "";

            read_block(input_file, state);

            if(state.eof)
                return "";
        }

        return state.block[state.index++];
    }
    
    void write_records(std::ofstream &out_file, const std::vector<std::string> &buffer){
        std::string block;
        block.reserve(blocking_factor * MAX_RECORD_LENGTH); 

        int count = 0;

        for(const std::string &record : buffer) {
            block += record + "\n";
            count++;

            if (count == blocking_factor) {
                out_file << block; 
                disk_writes++;
                disk_operations++;

                block.clear();
                count = 0;
            }
        }

        if(!block.empty()) {
            out_file << block;
            disk_writes++;
            disk_operations++;
        }
    }

    ~BlockIO() {
        if (file.is_open()) {
            file.close();
        }
    }
};