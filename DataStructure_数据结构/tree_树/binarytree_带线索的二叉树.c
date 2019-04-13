#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _node{
    int key;
    int value;
    struct _node *left, *right; //左右孩子指针 or 前驱/后续指针
    unsigned char ltag, rtag;   //左右标志 0:指针指向孩子 1:指针指向前驱or后继
} Node;

typedef struct _tree {
    Node *head;
    int count;
} Tree;

Tree *create_tree() 
{
    Tree *tree = (Tree *)malloc(sizeof(Tree));
    bzero(tree, sizeof(Tree));
    tree->head = (Node *)malloc(sizeof(Node));
    bzero(tree->head, sizeof(Node));
    tree->head->ltag = 0;
    tree->head->left = NULL;
    tree->head->rtag = 1;
    tree->head->right = NULL;
    tree->count = 0;
    return tree;
}

int insert_key(Tree *tree, int key, int value)
{
    printf("[%d] %d\n", key, value);
    Node *node = (Node *)malloc(sizeof(Node));
    bzero(node, sizeof(node));
    node->key = key;
    node->value = value;
    node->ltag = 1; //因为新插入的节点是叶子节点 不存在孩子节点
    node->rtag = 1;
    // 根节点
    if (tree->head->left == NULL) {
        node->left = tree->head;
        node->right = tree->head;
        tree->head->left = node;
        tree->head->right = node;
    }
    // 子节点
    else {
        Node *father = NULL;
        Node *clid = tree->head->left;
        while (clid != NULL) {
            // 左节点上的值
            if (node->key < clid->key) {
                father = clid;
                if (father->ltag == 0)
                    clid = father->left;
                else
                    clid = NULL;
                if (clid == NULL) {
                    node->left = father->left;  //新节点的前驱为父节点的前驱
                    node->right = father;       //新节点的后续为父节点
                    father->left = node;        //父节点的左节点为新节点
                    father->ltag = 0;
                    break;
                }
            }
            // 右节点上的值
            else if (node->key > clid->key) {
                father = clid;
                if (father->rtag == 0)
                    clid = father->right;
                else
                    clid = NULL;
                if (clid == NULL) {
                    node->left = father;        //新节点的前驱为父亲节点
                    node->right = father->right;//新节点的后续为父亲的后续
                    father->right = node;       //父节点的右节点为新节点
                    father->rtag = 0;
                    break;
                }
            }
            //树中不能存在相同的节点
            else {
                free(node);
                return 0;
            }
        }
    }
    tree->count ++;
    return 0;
}

//中序遍历
void in_order(const Tree *tree)
{
    printf("InOrder:\n");
    Node *node = tree->head->left;
    while (node != tree->head) {
        while (node->ltag == 0)
            node = node->left;
        printf("%d ", node->key);
        while (node->rtag == 1 && node->right != tree->head) {
            node = node->right;
            printf("%d ", node->key);
        }
        node = node->right;
    }
    printf("\n");
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
    printf("CountTree: %d\n", tree->count);

   // int value;
   // assert(get_key(tree, 14, &value) == 0);
   // 
   // remove_key(tree, 58);
   // remove_key(tree, 2);
   // printf("After remove key 55 and 7 countTree: %d\n", count_tree(tree));

   // pre_order(tree);
    in_order(tree);
   // post_order(tree);
 
   // printf("DestoryTree.\n");
   // destroy_tree(tree);
    return 0;
}
