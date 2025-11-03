#include "generator.hpp"
#include <vector>
#include <string>
#include <ncurses.h>
#include <random>

std::vector<std::string> load_file(const std::string& file_name) {
    std::ifstream file(file_name);
    std::vector<std::string> lines;
    std::string line;

    if(!file.is_open()) {
        mvprintw(6, 0, "Cannot open first_names.txt or last_names.txt for writing");
        return lines;
    }

    lines.reserve(10000); 
    while(std::getline(file, line)) {
        if(!line.empty())
            lines.push_back(line);
    }
    return lines;
}

void generate_records(std::ofstream &records, int records_num) {
    auto first_names = load_file("first_names.txt");
    auto last_names  = load_file("last_names.txt");

    if(first_names.empty() || last_names.empty()) {
        mvprintw(6, 0, "One of the input files is empty");
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist_first(0, first_names.size() - 1);
    std::uniform_int_distribution<size_t> dist_last(0, last_names.size() - 1);

    for(int i = 0; i < records_num; ++i) {
        const std::string& first = first_names[dist_first(gen)];
        const std::string& last  = last_names[dist_last(gen)];
        records << last << " " << first << "\n";
    }

    records.flush();
}