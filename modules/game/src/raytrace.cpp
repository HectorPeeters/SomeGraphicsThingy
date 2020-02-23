
#define QUAD_TREE_CHILD_COUNT 8

template <typename T>
class QuadTree
{
    QuadTree<T> *children;

    T value;
};
