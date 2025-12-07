#include <fstream>
#include <string>
#include <vector>

#define RECORD_SIZE 28
#define POINTER_SIZE 4
#define KEY_SIZE 4
#define VALUE_SIZE 24
#define PAGE_SIZE 32
#define INDEX_PAGE_SIZE 8

class FileManager {
private:
    std::fstream main_file;
    std::fstream index_file;
    std::fstream overflow_file;
    float alpha = 0.5;
public:
    int disk_reads = 0;
    int disk_writes = 0;

    FileManager(const std::string &main_file_name, const std::string &index_file_name,  const std::string &overflow_file_name) : 
            main_file(main_file_name), index_file(index_file_name), overflow_file(overflow_file_name){};

    std::string read_index_page(int page_num);

    void write_index_page(std::string &page, int page_num);

    std::string read_page(int page_num, bool overflow);

    void write_page(std::string &page, int page_num, bool overflow);

    void fetch();

    void insert();

    void show_records();

    void reorginize();

    // void delete();

    // void update();

    ~FileManager();
};