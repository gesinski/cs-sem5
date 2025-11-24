#include <fstream>
#include <string>

class FileManager {
private:
    std::ifstream input_file;
    std::ifstream b_tree_file;
public:
    FileManager(const std::string& records_file_name, const std::string& btree_file_name);
    ~FileManager();
};