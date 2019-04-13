#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "B_plus_tree.h"

bool disk_alloc(BPlusTree *tree, BPlusNode *node)
{
    bzero(node, sizeof(BPlusNode));
    if (tree->t_invalid_pos != 0) {
        node->b_position = tree->t_invalid_pos;
        disk_read(tree, node);
        tree->t_invalid_pos = node->b_next;
        printf("[alloc] position: 0x%x\n", node->b_position);
    } else {
        fseek(tree->t_fp, 0, SEEK_END);
        node->b_position = ftell(tree->t_fp);
        printf("[alloc] position: 0x%x (new addr)\n", node->b_position);
    }
    node->b_is_valid = true;
    return true;
}

void disk_free(BPlusTree *tree, BPlusNode *node)
{
    printf("[free] position: 0x%x\n", node->b_position);
    node->b_is_valid = false;
    node->b_next = tree->t_invalid_pos;
    fseek(tree->t_fp, node->b_position, SEEK_SET);
    fwrite(node, sizeof(BPlusNodeWithOutAddr), 1, tree->t_fp);
    tree->t_invalid_pos = node->b_position;
}

bool disk_read(BPlusTree *tree, BPlusNode *node)
{
    fseek(tree->t_fp, node->b_position, SEEK_SET);
    fread(node, sizeof(BPlusNodeWithOutAddr), 1, tree->t_fp);
    return true;
}

bool disk_write(BPlusTree *tree, BPlusNode *node)
{
    fseek(tree->t_fp, node->b_position, SEEK_SET);
    fwrite(node, sizeof(BPlusNodeWithOutAddr), 1, tree->t_fp);
    return true;
}

int data_file_check(const char *file)
{
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        printf("[data_file_check] %s file not exist.\n", file);
        return DATA_FILE_NOT_EXIST;
    }
    fseek(fp, 0, SEEK_END);
    long lenght = ftell(fp);
    if (lenght < sizeof(BPlusTreeInfo)) {
        printf("[data_file_check] %s file wrang format.\n", file);
        fclose(fp);
        return DATA_FILE_CANNOT_LOAD;
    }
    BPlusTreeInfo info;
    fseek(fp, 0, SEEK_SET);
    fread(&info, sizeof(BPlusTreeInfo), 1, fp);
    if (strncmp(info.version, VERSION, 11) != 0) {
        printf("[data_file_check] %s file wrang format.\n", file);
        fclose(fp);
        return DATA_FILE_CANNOT_LOAD;
    }
    if (strcmp(info.version, VERSION) != 0) {
        printf("[data_file_check] %s data file version %s not match.\n", file, &info.version[12]);
        fclose(fp);
        return DATA_FILE_CANNOT_LOAD;
    }
    if (info.degree != DEGREE) {
        printf("[data_file_check] %s data file degree %d not match.\n", file, info.degree);
        fclose(fp);
        return DATA_FILE_CANNOT_LOAD;
    }
    fclose(fp);
    return DATA_FILE_IS_OK;
}

BPlusTree *b_tree_init(char *file)
{
    int check_res = data_file_check(file);
    if (check_res == DATA_FILE_CANNOT_LOAD) {
        exit(0);
    }
    BPlusTree *tree = (BPlusTree *)malloc(sizeof(BPlusTree));
    assert(tree != NULL);
    bzero(tree, sizeof(BPlusTree));
    tree->t_root = (BPlusNode *)malloc(sizeof(BPlusNode));
    assert(tree->t_root != NULL);
    bzero(tree->t_root, sizeof(BPlusNode));
    strncpy(tree->t_file, file, sizeof(tree->t_file)-1);
    if (check_res == DATA_FILE_NOT_EXIST) {
        // 写入B+树信息
        tree->t_fp = fopen(tree->t_file, "wb+");
        assert(tree->t_fp != NULL);
        strncpy(tree->t_version, VERSION, sizeof(tree->t_version) - 1);
        tree->t_degree = DEGREE;
        tree->t_root_pos = 0;
        tree->t_data_pos = 0;
        tree->t_invalid_pos = 0;
        fseek(tree->t_fp, 0, SEEK_SET);
        fwrite(tree, sizeof(BPlusTreeInfo), 1, tree->t_fp);
    } else {
        // 读入B+树信息
        tree->t_fp = fopen(tree->t_file, "rb+");
        assert(tree->t_fp != NULL);
        fread(tree, sizeof(BPlusTreeInfo), 1, tree->t_fp);
    }
    // 存在根节点则读取,否则写入空节点
    if (tree->t_root_pos != 0) {
        tree->t_root->b_position = tree->t_root_pos;
        disk_read(tree, tree->t_root);
    } else {
        disk_alloc(tree, tree->t_root);
        tree->t_root->b_amount = 0;
        tree->t_root->b_is_leaf = false;
        disk_write(tree, tree->t_root);
        tree->t_root_pos = tree->t_root->b_position;   //第一个有效节点总是根节点
    }
    return tree;
}

void b_tree_show(BPlusTree *tree)
{
    int deep = 0;
    BPlusNode _node;
    long first_child_position;
    BPlusNode *node = &_node;
    printf("\t============ B Plus Tree ==========\n");
    //按层遍历
    memcpy(node, tree->t_root, sizeof(BPlusNode));
    while (1) {
        if (node->b_amount == 0 || node->b_is_leaf == true) {
            first_child_position = 0;
        } else {
            first_child_position = node->b_children[0];
        }
        printf(" [%03d] ", deep ++);
        while (1) {
            int i;
            printf("{");
            for (i = 0; i < node->b_amount; i ++) {
                printf("%d ", node->b_keys[i]);
            }
            printf("} ");
            node->b_position = node->b_right;
            if (node->b_position == 0)
                break;
            disk_read(tree, node);
        }
        printf("\n");
        if (first_child_position == 0)
            break;
        node->b_position = first_child_position;
        disk_read(tree, node);
    }
}

void b_tree_keys(BPlusTree *tree)
{
    BPlusNode _node;
    BPlusNode *node = &_node;
    printf("\t========== B Plus Tree Keys ========\n");
    // tree->t_data_pos
    node->b_position = tree->t_data_pos;
    int i, n = 0;
    while (node->b_position != 0) {
        disk_read(tree, node);
        for (i = 0; i < node->b_amount; i ++) {
            if (n % 10 == 0)
                printf("\n   ");
            printf("%5ld ", node->b_keys[i]);
            n ++;
        }
        node->b_position = node->b_right;
    }
    printf("\n  [total]: %d\n", n);
}

bool b_tree_find(BPlusTree *tree, long key, addr *b_position)
{
    BPlusNode _node;
    BPlusNode *node = &_node;
    addr pos;
    memcpy(node, tree->t_root, sizeof(BPlusNode));
    while (1) {
        int i;
        pos = 0;
        for (i = node->b_amount - 1; i >= 0; i --) {
            if (key >= node->b_keys[i]) {
                pos = node->b_children[i];
                break;
            }
        }
        if (node->b_is_leaf) {
            if (pos != 0 && key == node->b_keys[i]) {
                printf("[find] find this key: %d in position: 0x%x\n", key, pos);
                *b_position = pos;
                return true;
            } else {
                printf("[find] no this key: %d\n", key);
                return false;
            }
        }
        if (pos == 0) {
            printf("[find] no this key: %d\n", key);
            return false;
        }
        node->b_position = pos;
        disk_read(tree, node);    
    }
    return false;
}

void _b_tree_split_child(BPlusTree *tree, BPlusNode *father, int index, BPlusNode *child)
{
    BPlusNode _node;
    BPlusNode *child_new = &_node;
    disk_alloc(tree, child_new);
    int i;
    // 移动degree->2*degree数据到新点上
    for (i = 0; i < tree->t_degree; i ++) {
        child_new->b_children[i] = child->b_children[i + tree->t_degree];
        child_new->b_keys[i] = child->b_keys[i + tree->t_degree];
    }
    child_new->b_amount = tree->t_degree;
    child->b_amount = tree->t_degree;
    child_new->b_is_leaf = child->b_is_leaf;
    child_new->b_right = child->b_right;
    child_new->b_left = child->b_position;
    child->b_right = child_new->b_position;
    // 调整father
    for (i = father->b_amount - 1; i > index; i --) {
        father->b_children[i + 1] = father->b_children[i];
        father->b_keys[i + 1] = father->b_keys[i];
    }
    father->b_keys[index + 1] = child_new->b_keys[0];
    father->b_children[index + 1] = child_new->b_position;
    father->b_amount ++;
    disk_write(tree, father);
    disk_write(tree, child);
    disk_write(tree, child_new);
}

bool _b_tree_insert(BPlusTree *tree, BPlusNode *node, long key)
{
    int n;
    BPlusNode _node;
    BPlusNode *child = &_node;

    // 非叶子节点
    if (!node->b_is_leaf) {
        for (n = node->b_amount - 1; n >=0; n --) {
            if (key >= node->b_keys[n]) {
                child->b_position = node->b_children[n];
                disk_read(tree, child);
                if (child->b_amount == 2*tree->t_degree) {
                    //printf(" before split amount: %d\n", node->b_amount);
                    _b_tree_split_child(tree, node, n, child);
                    //printf(" after split amount: %d\n", node->b_amount);
                    if (key >= node->b_keys[n + 1]) {
                        child->b_position = node->b_children[n + 1];
                        disk_read(tree, child);
                    }
                }
                _b_tree_insert(tree, child, key);
                return true;
            }
        }
        // key 比所有keys都小 则将该key插入children[0]中
        node->b_keys[0] = key;
        disk_write(tree, node);
        child->b_position = node->b_children[0];
        disk_read(tree, child);
        if (child->b_amount == 2*tree->t_degree) {
            _b_tree_split_child(tree, node, n, child);
        }
        _b_tree_insert(tree, child, key);
    }
    // 数据最终保存在叶子节点上
    else {
        // 查找数据插入位置
        n = node->b_amount - 1;
        while (n >= 0) {
            if (key > node->b_keys[n]) {
                break;
            }
            if (key == node->b_keys[n]) {
                printf("[insert] ignore duplicate key: %d\n", key);
                return false;
            }
            n --;
        }
        int i;
        //printf("insert after %d\n", n);
        for (i = node->b_amount - 1; i > n; i --) {
            node->b_keys[i + 1] = node->b_keys[i];
            node->b_children[i + 1] = node->b_children[i];
        }
        //printf("insert at %d\n", n+1);
        // 插入数据
        node->b_keys[n + 1] = key;
        node->b_children[n + 1] = 0x5f5f5f5f;   // 指向数据在磁盘上的实际地址
        node->b_amount ++;
        disk_write(tree, node);
        printf("[insert] key: %d success\n", key);
    }
    return true;
}

bool b_tree_insert(BPlusTree *tree, long key)
{
    BPlusNode *root = tree->t_root;
    // 根满
    if (root->b_amount == 2*tree->t_degree) {
        BPlusNode _node;
        BPlusNode *child = &_node;
        disk_alloc(tree, child);
        memcpy(child, root, sizeof(BPlusNodeWithOutAddr));
        root->b_is_valid = true;
        root->b_amount = 1;
        root->b_keys[0] = child->b_keys[0];
        root->b_children[0] = child->b_position;
        disk_write(tree, child);
        _b_tree_split_child(tree, root, 0, child);
        return _b_tree_insert(tree, root, key);
   
    // 半满
    }  else if (root->b_amount > 0) {
        return _b_tree_insert(tree, root, key);
    
    // 空树
    } else {
        BPlusNode _node;
        BPlusNode *child = &_node;
        disk_alloc(tree, child);
        child->b_amount = 1;
        child->b_keys[0] = key;
        child->b_children[0] = 0x5f5f5f5f;   // 指向数据在磁盘上的实际地址
        child->b_is_leaf = true;
        root->b_amount = 1;
        root->b_keys[0] = key;
        root->b_children[0] = child->b_position;
        disk_write(tree, root);
        disk_write(tree, child);
        tree->t_data_pos = root->b_children[0]; // 第一个数据节点
        return true;
    }
}

bool _b_tree_merge_child(BPlusTree *tree, BPlusNode *father, int right_index, BPlusNode *left_child, BPlusNode *right_child)
{
    // 将右半满孩子合并到左半满孩子上
    int n, i;
    n = left_child->b_amount;
    for (i = 0; i < right_child->b_amount; i ++) {
        left_child->b_keys[n + i] = right_child->b_keys[i];
        left_child->b_children[n + i] = right_child->b_children[i];
    }
    left_child->b_amount += right_child->b_amount;
    left_child->b_right = right_child->b_right;
    disk_free(tree, right_child);
    for (i = right_index; i < father->b_amount; i ++) {
        father->b_keys[i] = father->b_keys[i + 1];
        father->b_children[i] = father->b_children[i + 1];
    }
    father->b_amount --;
    disk_write(tree, left_child);
    disk_write(tree, father);
    //printf("after merge child:\n");
    //b_tree_show(tree);
    return true;
}

void _b_tree_update_ancestor_key(BPlusTree *tree, long old_key, long new_key, addr child_pos)
{
    printf("[update ancestor] old_key: %d, new_key: %d\n", old_key, new_key);
    BPlusNode _node;
    BPlusNode *node = &_node;
    memcpy(node, tree->t_root, sizeof(BPlusNode));
    while (1) {
        int i;
        for (i = node->b_amount - 1; i >= 0; i --) {
            if (old_key >= node->b_keys[i]) {
                if (old_key == node->b_keys[i]) {
                    node->b_keys[i] = new_key;
                    disk_write(tree, node);
                    if (node->b_children[i] == child_pos)
                        goto end;
                }
                node->b_position = node->b_children[i];
                break;
            }
        }
        disk_read(tree, node);
        if (node->b_is_leaf)
            break;
    }
end:
    // 根节点在磁盘上的数据会被更新 需要重新加载
    disk_read(tree, tree->t_root);
}

bool _b_tree_delete(BPlusTree *tree, BPlusNode *node, long key)
{
    int index, i;
    // 非叶子节点
    if (!node->b_is_leaf) {
        BPlusNode _node;
        BPlusNode *child = &_node;
        for (index = node->b_amount - 1; index >= 0; index --) {
            if (key >= node->b_keys[index]) {
                break;
            }
        }
        if (index < 0) {
            printf("[delete] fail, not find key %d.\n", key);
            return false;
        }
        child->b_position = node->b_children[index];
        disk_read(tree, child);
        // 孩子节点数 大于半满
        if (child->b_amount > tree->t_degree) {
            //printf(" 孩子节点数 大于半满:\n");
            //b_tree_show(tree);
            return _b_tree_delete(tree, child, key);
        }
        // 孩子数等于半满
        BPlusNode _brother_node;
        BPlusNode *brother = &_brother_node;
        BPlusNode *left_child, *right_child;
        int right_index;
        brother->b_position = node->b_children[index==0?1:index-1];
        disk_read(tree, brother);
        if (index == 0) {
            //printf("child is left\n");
            left_child = child;
            right_child = brother;
            right_index = 1;
        } else {
            //printf("child is right\n");
            left_child = brother;
            right_child = child;
            right_index = index;
        }
        // 合并两个半满的孩子
        if (left_child->b_amount == tree->t_degree && right_child->b_amount == tree->t_degree) {
            _b_tree_merge_child(tree, node, right_index, left_child, right_child);
            return _b_tree_delete(tree, left_child, key);
        // 兄弟大于半满 从兄弟的孩子中给child节点 平衡一下两边的孩子数 使child节点的孩子数大于半满
        } else {
            int i, n, count;
            long old_key, new_key;
            count = (brother->b_amount - tree->t_degree + 1) / 2; //最少移动一个
            old_key = right_child->b_keys[0];
            // child是左孩子
            if (right_index != index) {
                n = left_child->b_amount;
                for (i = 0; i < count; i ++) {
                    left_child->b_keys[n + i] = right_child->b_keys[i];
                    left_child->b_children[n + i] = right_child->b_children[i];
                }
                left_child->b_amount += count;
                right_child->b_amount -= count;
                for (i = 0; i < right_child->b_amount; i ++) {
                    right_child->b_keys[i] = right_child->b_keys[i + count];
                    right_child->b_children[i] = right_child->b_children[i + count];
                }
            // child是右孩子
            } else {
                n = right_child->b_amount;
                for (i = right_child->b_amount - 1; i >= 0; i --) {
                    right_child->b_keys[i + count] = right_child->b_keys[i];
                    right_child->b_children[i + count] = right_child->b_children[i];
                }
                n = left_child->b_amount - count;
                for (i = 0; i < count; i ++) {
                    right_child->b_keys[i] = left_child->b_keys[n + i];
                    right_child->b_children[i] = left_child->b_children[n + i];
                }
                right_child->b_amount += count;
                left_child->b_amount -= count;
            }
            disk_write(tree, left_child);
            disk_write(tree, right_child);
            //printf(" 兄弟大于半满:\n");
            //b_tree_show(tree);
            new_key = right_child->b_keys[0];
            // 改变右孩子父亲,曾父..对应的key
            _b_tree_update_ancestor_key(tree, old_key, new_key, right_child->b_position);
            return _b_tree_delete(tree, child, key);
        }

    
    // 叶子节点
    } else {
        for (index = 0; index < node->b_amount; index ++) {
            if (key == node->b_keys[index])
                break;
        }
        if (index < node->b_amount) {
            //printf(" 从叶子节点删除数据:\n");
            //b_tree_show(tree);
            long old_key, new_key;
            old_key = node->b_keys[0];
            printf("[delete] success, key %d at pos 0x%x.\n", key, node->b_children[index]);
            for (i = index; i < node->b_amount; i ++) {
                node->b_keys[i] = node->b_keys[i + 1];
                node->b_children[i] = node->b_children[i + 1];
            }
            node->b_amount --;
            disk_write(tree, node);
            if (index == 0) {   // 删除的key 为叶子节点的一个孩子 则更新祖先节点对应的key值
                new_key = node->b_keys[0];
                _b_tree_update_ancestor_key(tree, old_key, new_key, node->b_position);
            }
        } else {
            printf("[delete] fail, key %d not find.\n", key);
            return false;
        }
    }
    return true;
}

bool b_tree_delete(BPlusTree *tree, long key)
{
    BPlusNode *root = tree->t_root;
    if (root->b_amount > 1) {
        return _b_tree_delete(tree, root, key);
    } else if (root->b_amount == 1) {
        BPlusNode _node;
        BPlusNode *child = &_node;
        child->b_position = root->b_children[0];
        disk_read(tree, child);
        // 孩子为非叶子节点
        if (!child->b_is_leaf) {
            memcpy(root, child, sizeof(BPlusNodeWithOutAddr));
            disk_free(tree, child);
            disk_write(tree, root);
            return _b_tree_delete(tree, root, key);
        
        // 孩子为叶子节点
        } else {
            int n, i;
            for (n = 0; n < child->b_amount; n ++) {
                if (key == child->b_keys[n]) {
                    break;
                }
            }
            if (n < child->b_amount) {
                printf("[delete] success, key %d at pos 0x%x.\n", key, child->b_children[n]);
                for (i = n; i < child->b_amount; i ++) {
                    child->b_keys[i] = child->b_keys[i + 1];
                    child->b_children[i] = child->b_children[i + 1];
                }
                child->b_amount --;
                if (child->b_amount == 0) {
                    disk_free(tree, child);
                    root->b_amount = 0;
                    tree->t_data_pos = 0;
                    disk_write(tree, root);
                } else {
                    if (n == 0) {   // 删除的key 为叶子节点的一个孩子
                        root->b_keys[0] = child->b_keys[0];
                        disk_write(tree, root);
                    }
                    disk_write(tree, child);
                }
                child->b_amount --;
            } else {
                printf("[delete] fail, key %d not find.\n", key);
                return false;
            }
        }
    } else {
        printf("[delete] fail, tree is empty.\n");
        return false;
    }
    return true;
}

void b_tree_save_and_close(BPlusTree *tree)
{
    fseek(tree->t_fp, 0, SEEK_SET);
    fwrite(tree, sizeof(BPlusTreeInfo), 1, tree->t_fp);
    fclose(tree->t_fp);
    free(tree->t_root);
    free(tree);
}

int main(int argc, char **argv)
{
    int i;
    long key;
    addr pos;
    printf("sizeof(BPlusTreeInfo)=%d\n", sizeof(BPlusTreeInfo));
    printf("sizeof(BPlusNodeWithOutAddr)=%d\n", sizeof(BPlusNodeWithOutAddr));
    if (argc < 2) {
        printf("Usage: %s [data_file]\n", argv[0]);
       exit(0);
    }
    BPlusTree *tree = b_tree_init(argv[1]);
    printf("\n\n======  Start Insert  ======\n");
    for (i = 0; i < 10000; i ++) {
        key = rand() % 10000;
        printf("\n> try insert key: %ld\n", key);
        b_tree_insert(tree, key);
        //b_tree_show(tree);
    }
      
    printf("\n\n======  Start Delete  ======\n");
    for (i = 0; i < 3000; i ++) {
        key = rand() % 10000;
        printf("\n< try delete key: %ld\n", key);
        b_tree_delete(tree, key);
        //b_tree_show(tree);
        //getchar();
    }
    
    printf("\n\n======  Start Insert  ======\n");
    for (i = 0; i < 100; i ++) {
        key = rand() % 100;
        printf("\n> try insert key: %ld\n", key);
        b_tree_insert(tree, key);
        //b_tree_show(tree);
    }

    printf("\n\n======  Start Find  ======\n");
    b_tree_find(tree, 64, &pos);
    b_tree_find(tree, 41, &pos);
    b_tree_find(tree, 1, &pos);
   
    b_tree_show(tree);
    b_tree_keys(tree);
    b_tree_save_and_close(tree);
    return 0;
}
