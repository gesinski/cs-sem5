#include "BlockIO.hpp"
#include <unistd.h>
#include <cstring> 
#include <algorithm>
#include <tuple>
#include <queue>
#include <filesystem>
#include <system_error>

class ExternalSorter {
private:
    const std::string FILE_NAME;
    int runs = 0, phases = 0;
    bool simulate_till_end = false;

    struct heap_item {
        std::string value;    
        int source;      
    };

    struct min_cmp {
        bool operator()(heap_item const &a, heap_item const &b) const {
            if (a.value == b.value)
                return a.source > b.source;
            return a.value > b.value;
        }
    };

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
            mvprintw(i + 1, 0, "%ld. %s", i, buffer[i].c_str());
        }
        mvprintw(buffer.size() + 2, 0, "Press any key to continue...");
        refresh();
        getch();
    }

    void check_for_simulation(const std::vector<std::string> &buffer) {
        if(!simulate_till_end) {
            clear();
            //mvprintw(0, 0, "Created run %d with %lu records.", runs, buffer.size());
            mvprintw(0, 0, "Press 's' to simulate till end, any other key to show sorted file...");
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

    std::vector<std::string> creating_runs(int blocking_factor, int buffers_num, BlockIO &file_manager) {
        std::vector<std::string> files_names;
        while(true) {
            std::vector<std::string> buffer;
            
            for(size_t i = 0; i < static_cast<size_t>(buffers_num); i++) {
                std::vector<std::string> reading_buffer = file_manager.read_records(file_manager.file);
                
                if(!reading_buffer.empty() && reading_buffer[0] == "EOF") {
                    break;
                } else {
                    for(const auto &record : reading_buffer)
                        buffer.push_back(record);
                }
            }

            if (buffer.empty()) {
                break;
            }

            std::sort(buffer.begin(), buffer.end());
            
            std::string tape_name = create_temp_file("tape");
            files_names.push_back(tape_name);
            std::ofstream tape(tape_name);

            file_manager.write_records(tape, buffer);
            runs++;

            tape.close();

            check_for_simulation(buffer);
        }
        return files_names;
    }

    void merging(int blocking_factor, int buffers_num, BlockIO &file_manager, std::vector<std::string> &files_names) {
        while(files_names.size() > 1) {
            int current_buffer = 0;
            std::vector<std::ifstream> run_files;

            buffers_num - 1 < 2 ? buffers_num = 3 : buffers_num;
            std::vector<std::string> group_input_names;
            while(current_buffer < buffers_num - 1) {
                if(files_names.size() == static_cast<size_t>(current_buffer)) {
                    break;
                } 

                group_input_names.push_back(files_names[current_buffer]);
                run_files.emplace_back(files_names[current_buffer]);
                current_buffer++;
            }
            files_names.erase(files_names.begin(), files_names.begin() + current_buffer);

            std::priority_queue<heap_item, std::vector<heap_item>, min_cmp> pq;
            std::string output_run_name = create_temp_file("tape");
            files_names.push_back(output_run_name);
            std::ofstream output_run(output_run_name);

            std::vector<std::string> buffer;
            std::vector<BlockIO::file_state> file_states(run_files.size());
            phases++;

            for(size_t k = 0; k < run_files.size(); k++) {
                std::string next_record = file_manager.read_record(run_files[k], file_states[k]);
                pq.push({next_record, static_cast<int>(k)});
            }

            while(!pq.empty()) {
                heap_item min_record = pq.top();
                pq.pop();

                buffer.push_back(min_record.value);

                int src = min_record.source;

                std::string next_record = file_manager.read_record(run_files[src], file_states[src]);

                if(!next_record.empty()) {
                    pq.push({next_record, src});
                }
            }

            file_manager.write_records(output_run, buffer);

            check_for_simulation(buffer);

            for(auto &f : run_files)
                f.close();
            output_run.close();

            for(const auto &infile : group_input_names) {
                std::error_code rem_ec;
                if(std::filesystem::exists(infile)) {
                    std::filesystem::remove(infile, rem_ec);
                    if(rem_ec) {
                        mvprintw(0, 0, "Warning: cannot remove temp file %s: %s", infile.c_str(), rem_ec.message().c_str());
                        refresh();
                    }
                }
            }
        }
        const std::string &final_run = files_names[0];
        const std::string dest_dir = "output";
        std::error_code ec;

        std::filesystem::path dest_path(dest_dir);
        if(std::filesystem::is_directory(dest_path)) {
            dest_path /= std::filesystem::path(final_run).filename();
        }

        std::filesystem::copy_file(final_run, dest_path, std::filesystem::copy_options::overwrite_existing, ec);

        if(ec) {
            mvprintw(0, 0, "Warning: cannot copy %s to %s: %s",
                    final_run.c_str(), dest_path.string().c_str(), ec.message().c_str());
        } else {
            std::error_code rem_ec;
            std::filesystem::remove(final_run, rem_ec);
            if(rem_ec) {
                mvprintw(1, 0, "Warning: cannot remove temp file %s: %s",
                        final_run.c_str(), rem_ec.message().c_str());
            }

            clear();
            mvprintw(0, 0, "Final output written to %s", dest_path.string().c_str());
        }

        refresh();
        getch();
    }
public:
    ExternalSorter(const std::string &file_name) : FILE_NAME(file_name){}

    void sort() {
        int blocking_factor, buffers_num;
        read_parameters(blocking_factor, buffers_num);

        BlockIO file_manager(FILE_NAME, blocking_factor);
        
        std::vector<std::string> files_names = creating_runs(blocking_factor, buffers_num, file_manager);

        merging(blocking_factor, buffers_num, file_manager, files_names);

        clear();
        mvprintw(0, 0, "Disk reads: %d", file_manager.disk_reads);
        mvprintw(1, 0, "Disk writes: %d", file_manager.disk_writes);
        mvprintw(2, 0, "Disk operations: %d", file_manager.disk_operations);
        mvprintw(3, 0, "Phases: %d", phases);
        refresh();
        getch();
    }
};