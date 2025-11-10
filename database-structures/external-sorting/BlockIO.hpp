#include <string>
#include <ncurses.h>
#include <vector>
#include <fstream>

class BlockIO {
public:
    std::ifstream file;
    int disk_reads = 0, disk_writes = 0;
    int disk_operations = 0;

    BlockIO(const std::string &file_name) {
        file.open(file_name);
        if(!file) {
            mvprintw(0, 0, "Cannot open %s for writing", file_name.c_str());
        }
    }

    std::vector<std::string> read_records(std::ifstream &input_file, int blocking_factor) {
        std::vector<std::string> reading_buffer;
        std::string record;

        for (int i = 0; i < blocking_factor && std::getline(input_file, record); ++i) {
            reading_buffer.push_back(record);
        }

        if (!reading_buffer.empty()) {
            disk_reads++;
            disk_operations++;
            return reading_buffer;
        } else {
            return {"EOF"};  
        }
    }
    
    void write_records(std::ofstream &out_file, std::vector<std::string> &buffer) {
        for(std::string &record : buffer) {
            out_file << record << "\n";
        }
        disk_writes++;
        disk_operations++;
    }

    ~BlockIO() {
        if (file.is_open()) {
            file.close();
        }
    }
};