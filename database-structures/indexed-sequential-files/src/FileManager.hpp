#include <fstream>
#include <string>
#include <cstdint>
#include <bits/stdc++.h>
#include <vector>

#define RECORD_SIZE 28
#define POINTER_SIZE 4
#define KEY_SIZE sizeof(unsigned int)
#define DATA_SIZE 24
#define PAGE_SIZE 320
#define INDEX_PAGE_SIZE 80
#define BLOCKING_FACTOR 10

class FileManager {
private:
    float alpha = 0.7;
    unsigned int smallest_key = 4294967295;
public:
    std::fstream main_file;
    std::fstream index_file;
    std::fstream overflow_file;
    int disk_reads = 0;
    int disk_writes = 0;
    int records_overflow = 0;

    FileManager(const std::string &main_file_name, const std::string &index_file_name,  const std::string &overflow_file_name);

    std::string read_index_page(unsigned int page_num);

    void write_index_page(struct index_record[], unsigned int page_num);

    std::string read_page(unsigned int page_num, bool overflow);

    void write_page(struct record[], unsigned int page_num, bool overflow);

    void fetch();

    bool insert(unsigned int key, std::string record);

    void fetch_records();

    void reorganize();

    // void delete();

    // void update();

    ~FileManager();
};
