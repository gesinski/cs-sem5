#include <vector>
#include <cstdint>

class Node {
public:
    std::vector<unsigned int> key;
    std::vector<int> records_block_num;
    int page_num;
    std::vector<int> pointers; 

    Node(std::vector<unsigned int> k = {}, std::vector<int> block = {}, int page = -1, std::vector<int> ptrs = {})
      : key(std::move(k)), records_block_num(std::move(block)), page_num(page), pointers(std::move(ptrs)) {}
};