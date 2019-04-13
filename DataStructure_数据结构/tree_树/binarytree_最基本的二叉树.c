#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct _node {
    int key;
    int value;
    struct _node *left, *right;
} Node;

typedef struct _tree {
    Node *root;
    int count;
} Tree;

Tree *create_tree()
{
    Tree *tree = (Tree *)malloc(sizeof(Tree));
    bzero(tree, sizeof(Tree));
    
    tree->root = NULL;
    tree->count = 0;
    return tree;
}

int insert_key(Tree *tree, int key, int value)
{
    Node *node = (Node *)malloc(sizeof(Node));
    bzero(node, sizeof(Node));
    node->key = key;
    node->value = value;

    if (tree->root == NULL) {
        tree->root = node;
    } else {
        Node *father = NULL;
        Node *clid = tree->root;
        while (clid != NULL) {
            // 左节点上的值
            if (key < clid->key) {
                father = clid;
                clid = clid->left;
                if (clid == NULL) {
                    father->left = node;
                    break;
            // 右节点上的值
            } else if (key > clid->key) {
                father = clid;
                clid = clid->right;
                if (clid == NULL) {
                    father->right = node;
                    break;
                }
            // 树中不能存在相同的节点
            } else {
                free(node);
                return 0;
            }
        } 
    }
    tree->count ++;
    return 0;
}

void _pre_order(const Node *node)
{
    if (node != NULL) {
        printf("%d ", node->key);
        _pre_order(node->left);
        _pre_order(node->right);
    }
}

// 前序：先根结点后左孩子最后右孩子
void pre_order(const Tree *tree)
{
    printf("PreOrder:\n");
    _pre_order(tree->root);
    printf("\n");
}

void _in_order(const Node *node)
{
    if (node != NULL) {
        _in_order(node->left);
        printf("%d ", node->key);
        _in_order(node->right);
    }
}

// 中序：先左孩子后根结点最后右孩子
void in_order(const Tree *tree)
{
    printf("InOrder:\n");
    _in_order(tree->root);
    printf("\n");
}

void _post_order(const Node *node)
{
    if (node != NULL) {
        _post_order(node->left);
        _post_order(node->right);
        printf("%d ", node->key);
    }
}

// 后序：先左孩子后右孩子最后根结点
void post_order(const Tree *tree)
{
    printf("PostOrder:\n");
    _post_order(tree->root);
    printf("\n");
}

int _find(const Tree *tree, int key, Node **p_node, Node **p_father)
{
    Node *node = tree->root;
    Node *father = NULL;
    while (node != NULL) {
        if (node->key == key) {
            if (p_node != NULL)
                *p_node = node;
            if (p_father != NULL)
                *p_father = father;
            return 0;
        } else if (key > node->key) {
            father = node;
            node = node->right;
        } else {
            father = node;
            node = node->left;
        }
    }
    return -1;
}

int get_key(const Tree *tree, int key, int *p_value)
{
    Node *node;
    if (_find(tree, key, &node, NULL) < 0) {
        return -1;
    }
    *p_value = node->value;
    return 0;
}
// 获取节点后代中key值最大的子节点 及这个节点的父节点
void _get_maximum_progeny(Node *node, Node **p_maximum_node, Node **p_maximum_father)
{
    Node *father = NULL;
    while (node->right != NULL) {
        father = node;
        node = node->right;
    }
    *p_maximum_node = node;
    *p_maximum_father = father;
}

void _free_node(Node *node)
{
    free(node);
}

void remove_key(Tree *tree, int key)
{
    Node *node, *father;
    if (_find(tree, key, &node, &father) != 0) {
        return;
    }
    if (node->left != NULL) {
        Node *left_maximum_node, *left_maximum_father;
        _get_maximum_progeny(node->left, &left_maximum_node, &left_maximum_father);
        if (father == NULL) {
            tree->root = left_maximum_node;
        } else {
            if (node->key > father->key)
                father->right = left_maximum_node;
            else
                father->left = left_maximum_node;
        }
        if (left_maximum_node != node->right)
            left_maximum_node->right = node->right;
        if (left_maximum_node != node->left)
            left_maximum_node->left = node->left;
        if (left_maximum_father != NULL)
            left_maximum_father->right = NULL;
    } else {
        if (father == NULL) {
            tree->root = node->right;
        } else {
            if (node->key > father->key)
                father->right = node->right;
            else
                father->left = node->right;
        }
    }
    _free_node(node);
    tree->count --;
}

// 删除节点包含其子节点
void _destroy_leaf(Node *node)
{
    if (node != NULL) {
        _destroy_leaf(node->left);
        _destroy_leaf(node->right);
        _free_node(node);
    }
}

void destroy_tree(Tree *tree)
{
    _destroy_leaf(tree->root);
    free(tree);
}

int count_tree(const Tree *tree)
{
    return tree->count;
}

int main()
{
    int array[]={7,4,2,3,15,35,6,45,58,20,1,14,56,57,55};
    printf("CreateTree:\n");
    Tree *tree = create_tree();
    int i, count = sizeof(array)/sizeof(array[0]);
    
    printf("Insert: %d\n", count);
    for (i = 0; i < count; i ++) {
        insert_key(tree, array[i], array[i]);
    }
    printf("CountTree: %d\n", count_tree(tree));

    int value;
    assert(get_key(tree, 14, &value) == 0);
    
    remove_key(tree, 58);
    remove_key(tree, 2);
    printf("After remove key 55 and 7 countTree: %d\n", count_tree(tree));

    pre_order(tree);
    in_order(tree);
    post_order(tree);
 
    printf("DestoryTree.\n");
    destroy_tree(tree);
    return 0;
}

