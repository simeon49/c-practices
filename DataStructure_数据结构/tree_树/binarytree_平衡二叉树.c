#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct _node {
    int key;
    int value;
    int depth;      //树的深度
    struct _node *left, *right;
} Node;

typedef struct _tree {
    Node *root;
    int count;
} Tree;

#define MAX(d1, d2) ((d1)>(d2)?(d1):(d2))
int depth(Node *node)
{
    if (node == NULL)
        return -1;
    return node->depth;
}

Node *single_rotate_left(Node *node)
{
    Node *left = node->left;
    node->left = left->right;
    left->right = node;

    node->depth = MAX(depth(node->left), depth(node->right)) + 1;
    left->depth = MAX(depth(left->left), node->depth) + 1;
    return left;
}

Node *single_rotate_right(Node *node)
{
    Node *right = node->right;
    node->right  = right->left;
    right->left = node;

    node->depth = MAX(depth(node->left), depth(node->right)) + 1;
    right->depth = MAX(node->depth, depth(right->right)) + 1;
    return right;
}

Node *double_rotate_left(Node *node)
{
    node->left = single_rotate_right(node->left);
    return single_rotate_left(node);
}

Node *double_rotate_right(Node *node)
{
   node->right = single_rotate_left(node->right);
   return single_rotate_right(node);
}

Tree *create_tree()
{
    Tree *tree = (Tree *)malloc(sizeof(Tree));
    bzero(tree, sizeof(Tree));
    
    tree->root = NULL;
    return tree;
}

Node *_insert(Node *node, int key, int value)
{
    if (node == NULL) {
        node = (Node *)malloc(sizeof(Node));
        bzero(node, sizeof(Node));
        node->key = key;
        node->value = value;
        node->depth = 0;
        printf("new key: %d\n", node->key);
    } else if (key < node->key) {
        node->left = _insert(node->left, key, value);
        printf("[%d]->left=%d\n", node->key, node->left->key);
        printf("  l_depth:%d r_depth:%d\n", depth(node->left), depth(node->right));
        if (depth(node->left) - depth(node->right) == 2) {
            if (key < node->left->key) {  // LL
                printf(" [%d] LL\n", node->key);
                node = single_rotate_left(node);
                printf("   root[%d]\n", node->key);
            } else {  //LR
                printf(" [%d] LR\n", node->key);
                node = double_rotate_left(node);
                printf("   root[%d]\n", node->key);
            }
        }
    } else if (key > node->key) {
        node->right = _insert(node->right, key, value);
        printf("[%d]->right=%d\n", node->key, node->right->key);
        if (depth(node->right) - depth(node->left) == 2) {
            if (key > node->right->key) { // RR
                printf(" [%d] RR\n", node->key);
                node = single_rotate_right(node);
                printf("   root[%d]\n", node->key);
            } else {   //RL
                printf(" [%d] RL\n", node->key);
                node = double_rotate_right(node);
                printf("   root[%d]\n", node->key);
            }
        }
    }
    node->depth = MAX(depth(node->left), depth(node->right)) + 1;
    return node;
}

int insert_key(Tree *tree, int key, int value)
{
    tree->root = _insert(tree->root, key, value);
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

Node *_remove_key(Node *node, int key)
{
    if (node == NULL)
        return NULL;
    if (key < node->key) {
        node->left = _remove_key(node->left, key);
        if (depth(node->right) - depth(node->left) == 2) {
            if (node->right->left != NULL && (depth(node->right) > depth(node->left))) {    //RL
                node = double_rotate_right(node);
            } else {    //RR
                node = single_rotate_right(node);
            }
        }
    } else if (key > node->key) {
        node->right = _remove_key(node->right, key);
        if (depth(node->left) - depth(node->left) == 2) {
            if (node->left->right != NULL && (depth(node->left) > depth(node->right))) {    //LR
                node = double_rotate_left(node);
            } else {
                node = single_rotate_left(node);
            }
        }
    } else {
        //两个儿子都有 找到右子树中值最小的节点,把右子树中最小节点的值赋值给本节点,删除右子树中最小值的节点
        if (node->left != NULL && node->right != NULL) {
            // 找到右节点中最小的节点
            Node *temp = node->right;
            while (temp->left != NULL)
                temp = temp->left;
            if (temp == node->right) {
                temp->left = node->left;
                free(node);
                node = temp;
            } else {
                node->key = temp->key;
                node->value = temp->value;
                node = _remove_key(node, key);
                if (depth(node->left), depth(node->right) == 2) {
                    if (node->left->right != NULL && (depth(node->left) > depth(node->right))) {    //LR
                        node = double_rotate_left(node);
                    } else {
                        node = single_rotate_left(node);
                    }
                }
            }
        //只有左儿子
        } else if (node->left != NULL) {
            Node *temp = node;
            node = node->left;
            free(temp);
            node = NULL;
        //只有右儿子
        } else if (node->right != NULL) {
            Node *temp = node;
            node = node->right;
            free(temp);
            node = NULL;
        } else {
            free(node);
            node = NULL;
        }
    }
    if (node != NULL)
        node->depth = MAX(depth(node->left), depth(node->right)) + 1;
    return node;
}

void remove_key(Tree *tree, int key)
{
    tree->root = _remove_key(tree->root, key);
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

    int value;
    assert(get_key(tree, 14, &value) == 0);
    
    remove_key(tree, 58);
    remove_key(tree, 2);
    printf("After remove key 55 and 7\n");
    
    pre_order(tree);
    in_order(tree);
    post_order(tree);
 
    printf("DestoryTree.\n");
    destroy_tree(tree);
    return 0;
}

