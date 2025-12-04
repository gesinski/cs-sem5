#include "Node.hpp"
#include "FileManager.hpp"

class BTree {
private:
    Node *root = nullptr;
public:
    unsigned int d;

    void set_root(Node *n) { root = n; }

    Node *get_root() const { return root; }

    int search(unsigned int key, FileManager &file_manager); 

    void compensation();

    bool insert(unsigned int key, int block_num, FileManager &file_manager);

    void update();

    void remove();

    void create_btree(FileManager &file_manager, unsigned int _d);

    Node *get_node_from_string(std::string page);

    std::string create_string_from_node(Node node);

    ~BTree() { delete root; }
};