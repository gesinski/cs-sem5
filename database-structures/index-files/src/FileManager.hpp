#include <fstream>
#include <string>
#include <vector>

class FileManager {
private:
    std::ifstream records_file;
    std::ifstream b_tree_file;
    int record_block_size;
    int b_tree_block_size;
public:
    int current_record_disk_reads = 0;
    int current_record_disk_writes = 0;
    int current_btree_disk_reads = 0;
    int current_btree_disk_writes = 0;

    FileManager(const std::string &records_file_name, const std::string &btree_file_name) : 
            records_file(records_file_name), b_tree_file(btree_file_name) {};

    std::string read_btree_block(int page);

    void write_btree_block(std::string &block, int page);

    std::string read_records_block(int page);

    void write_records_block(std::string &block, int page);

    ~FileManager();
};