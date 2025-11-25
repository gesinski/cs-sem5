class Node {
private:
    int page_num;
    int key;
    int *left;
    int *right;
public:
    Node(int page_num, int key, int *left, int *right) : page_num(page_num), key(key), left(left), right(right) {}
};