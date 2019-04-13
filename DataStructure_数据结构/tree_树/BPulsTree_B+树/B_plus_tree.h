#include <stdint.h>

typedef int bool;
#define true    1
#define false   0

typedef long addr;

//定义B+树的阶数
//#define DEGREE   125 
#define DEGREE   2
// B+树结点定义
typedef struct {
    bool is_valid;              // 有效位
    union {
        struct {
            int amount;     // 该结点中已保存的键值个数
            long keys[2*DEGREE];     // 键值数组
            addr children[2*DEGREE];   // 子结点位置数组
            addr left;          // 同一层中的左结点位置
            addr right;         // 同一层中的右结点位置
            bool is_leaf;       // 标志该结点是否叶结点
        };
        addr next;              // 下一个无效节点的位置(is_valid==False时有效)
    };
    unsigned char _pand[16];     // 填充至64字节
} BPlusNodeWithOutAddr;

typedef struct {
    BPlusNodeWithOutAddr b_node;
    addr b_position;
#define b_is_valid    b_node.is_valid
#define b_amount      b_node.amount
#define b_keys        b_node.keys
#define b_children    b_node.children
#define b_left        b_node.left
#define b_right       b_node.right
#define b_is_leaf     b_node.is_leaf
#define b_next        b_node.next
} BPlusNode;

typedef struct {
    char version[32];           // 版本信息
    int degree;                 // 树的阶数
    addr root_pos;              // 根节点的位置
    addr data_pos;              // 第一个数据节点的位置
    addr invalid_pos;           // 第一个无效节点位置
} BPlusTreeInfo;

#define VERSION     "B PLUS TREE:v0.1"
typedef struct {
    BPlusTreeInfo t_info;
    BPlusNode *t_root;          // 根节点
    char t_file[256];           // 数据文件
    FILE *t_fp;                 // 数据文件指针
#define t_version   t_info.version
#define t_degree    t_info.degree
#define t_root_pos  t_info.root_pos
#define t_data_pos  t_info.data_pos
#define t_invalid_pos   t_info.invalid_pos
} BPlusTree;

bool disk_alloc(BPlusTree *tree, BPlusNode *node);
void disk_free(BPlusTree *tree, BPlusNode *node);
bool disk_read(BPlusTree *tree, BPlusNode *node);
bool disk_write(BPlusTree *tree, BPlusNode *node);

#define DATA_FILE_IS_OK         0
#define DATA_FILE_NOT_EXIST     1
#define DATA_FILE_CANNOT_LOAD   2
int data_file_check(const char *file);

void _b_tree_split_child(BPlusTree *tree, BPlusNode *father, int index, BPlusNode *child);
bool _b_tree_insert(BPlusTree *tree, BPlusNode *node, long key);
void _b_tree_update_ancestor_key(BPlusTree *tree, long old_key, long new_key, addr child_pos);
bool _b_tree_merge_child(BPlusTree *tree, BPlusNode *father, int index, BPlusNode *child, BPlusNode *brother);
bool _b_tree_delete(BPlusTree *tree, BPlusNode *node, long key);

BPlusTree *b_tree_init(char *file);
bool b_tree_find(BPlusTree *tree, long key, addr *p_position);
bool b_tree_insert(BPlusTree *tree, long key);
bool b_tree_delete(BPlusTree *tree, long key);
void b_tree_show(BPlusTree *tree);
void b_tree_keys(BPlusTree *tree);
void b_tree_save_and_close(BPlusTree *tree);

