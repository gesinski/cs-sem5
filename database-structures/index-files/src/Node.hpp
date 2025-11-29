class Node {
private:
    struct value{
        int key;
        int page_num;
    };
    int d;
    int pointers[];
public:
    Node(int page_num, int key, int *left, int *right){}
};