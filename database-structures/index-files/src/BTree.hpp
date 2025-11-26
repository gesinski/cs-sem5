#include "Node.hpp"
#include "FileManager.hpp"

class BTree {
private:
    Node *root = nullptr;
public:
    void set_root(Node *n) { root = n; }

    Node *get_root() const { return root; }

    void create_btree(FileManager &file_manager);

    ~BTree() { delete root; }
};