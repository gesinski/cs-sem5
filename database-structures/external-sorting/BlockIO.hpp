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

    std::string read_record(std::ifstream &input_file) {
        std::string record;
        if(std::getline(input_file, record)) {
            disk_reads++;
            disk_operations++;
            return record;
        } else {
            return "EOF";
        }
    }
    
    void write_record(std::ofstream &out_file, std::string &record) {
        out_file << record << "\n";
        disk_writes++;
        disk_operations++;
    }

    ~BlockIO() {
        if (file.is_open()) {
            file.close();
        }
    }
};