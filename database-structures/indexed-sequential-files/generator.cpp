#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <arpa/inet.h> 

std::vector<std::string> load_file(const std::string& file_name) {
    std::ifstream file(file_name);
    std::vector<std::string> lines;
    std::string line;

    if(!file.is_open()) {
        std::cout << "Cannot open " << file_name << " for reading\n";
        return lines;
    }

    while(std::getline(file, line)) {
        if(!line.empty())
            lines.push_back(line);
    }
    return lines;
}

void write_record(std::ofstream& out, char op, unsigned int id, const std::string& name) {
    char buf[29];
    std::memset(buf, 0, sizeof(buf));

    std::memcpy(buf, &op, 1);
    std::memcpy(buf + 1, &id, sizeof(id));

    std::string fullname = name;
    if(fullname.size() > 24) fullname.resize(24);

    std::memset(buf + 5, ' ', 24);
    std::memcpy(buf + 5, fullname.data(), fullname.size());

    out.write(buf, sizeof(buf));
}

int main(int argc, char* argv[]) {
    const std::string out_file = (argc >= 2) ? argv[1] : "test_file";

    auto first_names = load_file("../external-sorting/first_names.txt");
    auto last_names  = load_file("../external-sorting/last_names.txt");

    if(first_names.empty() || last_names.empty()) {
        std::cerr << "One of the input files is empty\n";
        return 2;
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<size_t> dist_first(0, first_names.size() - 1);
    std::uniform_int_distribution<size_t> dist_last(0, last_names.size() - 1);
    std::uniform_int_distribution<unsigned int> dist_id(1, 100);

    std::ofstream out(out_file, std::ios::binary | std::ios::trunc);
    if(!out) {
        std::cerr << "Cannot open " << out_file << " for writing\n";
        return 3;
    }

    /* ================= INSERT 30 UNIQUE ================= */

    std::vector<unsigned int> pool(100);
    std::iota(pool.begin(), pool.end(), 1);
    std::shuffle(pool.begin(), pool.end(), gen);

    std::vector<unsigned int> existing_ids;

    for (int i = 0; i < 30; ++i) {
        unsigned int id = pool[i];
        existing_ids.push_back(id);

        std::string fullname =
            last_names[dist_last(gen)] + " " +
            first_names[dist_first(gen)];

        write_record(out, 'i', id, fullname);
        std::cout << "i " << id << " " << fullname << "\n";
    }

    /* ================= DELETE 3 EXISTING ================= */

    std::shuffle(existing_ids.begin(), existing_ids.end(), gen);
    for (int i = 0; i < 3; ++i) {
        unsigned int id = existing_ids.back();
        existing_ids.pop_back();

        write_record(out, 'd', id, "");
        std::cout << "d " << id << "\n";
    }

    /* ================= INSERT 1 NEW ================= */

    unsigned int new_id;
    do {
        new_id = dist_id(gen);
    } while (std::find(existing_ids.begin(), existing_ids.end(), new_id) != existing_ids.end());

    existing_ids.push_back(new_id);

    std::string fullname =
        last_names[dist_last(gen)] + " " +
        first_names[dist_first(gen)];

    write_record(out, 'i', new_id, fullname);
    std::cout << "i " << new_id << " " << fullname << "\n";

    /* ================= UPDATE 3 EXISTING ================= */

    std::shuffle(existing_ids.begin(), existing_ids.end(), gen);
    for (int i = 0; i < 3; ++i) {
        unsigned int id = existing_ids[i];

        std::string new_name =
            last_names[dist_last(gen)] + " " +
            first_names[dist_first(gen)];

        write_record(out, 'u', id, new_name);
        std::cout << "u " << id << " " << new_name << "\n";
    }

    /* ================= DELETE 1 EXISTING ================= */

    unsigned int id = existing_ids.back();
    write_record(out, 'd', id, "");
    std::cout << "d " << id << "\n";

    std::cout << "\nDone. Records written to " << out_file << "\n";
    return 0;
}

