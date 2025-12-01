#include "Node.hpp"
#include "FileManager.hpp"

class BTree {
private:
    Node *root = nullptr;
public:
    void set_root(Node *n) { root = n; }

    Node *get_root() const { return root; }

    int search(unsigned int key); 

    void update();

    void insert();

    void remove();

    void create_btree(FileManager &file_manager);

    Node *get_node_from_string(std::string page);

    std::string create_string_from_node(Node node);

    ~BTree() { delete root; }
};