#include "Node.hpp"

class BTree {
private:
    Node *root = nullptr;
public:
    void set_root(Node *n) { root = n; }

    Node *get_root() const { return root; }

    ~BTree() { delete root; }
};