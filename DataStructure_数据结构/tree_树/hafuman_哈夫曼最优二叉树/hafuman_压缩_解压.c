#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define TRUE    1
#define FALSE   0

typedef struct {
    int counts[256];    //uchar 从0-255 各uchar在文中出现的次数
    int total;          //文中出现 uchar的个数 <= 256
}UcharStatistics;

#define TYPE_LEAF_NODE 0
#define TYPE_TRUNK_NODE 1
#define BIT_STR_SIZE 32
typedef struct _tnode {
    int type;   //节点类型
    int weight; //权值
    union {
        struct {
            struct _tnode *left, *right;
        };
        struct {
            unsigned char orig_byte;
            char codes[BIT_STR_SIZE];
            int codes_len;
        };
    };
}Tnode;

typedef struct {
    Tnode *root;
    Tnode *leaf_list[256];
    int leaf_count;

    int _index;
}HafumanTree;

typedef struct {
    unsigned char orig_byte;
    char codes[BIT_STR_SIZE];
    int codes_len;
}TranCode;

typedef struct {
    TranCode *list[256];
    int count;
}TranCodeTable;

#define MAX_READ_SIZE 1024
int uchar_statistics(const char *file_name, UcharStatistics *p_stat)
{
    FILE *file;
    file = fopen(file_name, "rb");
    assert(file != NULL);
    int i;
    for (i = 0; i < 256; i++) {
        p_stat->counts[i] = 0;
    }
    p_stat->total = 0;
    unsigned char buf[MAX_READ_SIZE];
    int rc;
    while ((rc = fread(buf, sizeof(unsigned char), MAX_READ_SIZE, file)) != 0) {
        for (i = 0; i < rc; i ++) {
            unsigned char uch = buf[i];
            p_stat->counts[uch] ++;
            p_stat->total ++;
        }
    }
    fclose(file);
    //for (i = 0; i < 256; i ++) {
    //    printf("[0x%02x] %d\n", i, p_stat->counts[i]);
    //}
    //printf("Total: %d\n", p_stat->total);
    return 0;
}

static int cmp_tnode(const void *p1, const void *p2)
{
    Tnode *tnode1, *tnode2;
    tnode1 = *(Tnode **)p1;
    tnode2 = *(Tnode **)p2;
    //printf("%d  %d\n", tnode1->weight, tnode2->weight);
    if (tnode1->weight > tnode2->weight)
        return -1;
    else if (tnode1->weight < tnode2->weight)
        return 1;
    else
        return 0;
}

#define RELATION_ROOT   0
#define RELATION_LEFT_CHLID     1
#define RELATION_RIGHT_CHLID    2
void hfm_traversal(HafumanTree *hfm_tree, Tnode *tnode, int relation, char bit_str[], int len)
{
    assert(len < BIT_STR_SIZE);
    if (relation == RELATION_LEFT_CHLID) {
        bit_str[len] = '0';
        len ++;
    } else if (relation == RELATION_RIGHT_CHLID) {
        bit_str[len] = '1';
        len ++;
    }
    if (tnode->type == TYPE_TRUNK_NODE) {
        hfm_traversal(hfm_tree, tnode->left, RELATION_LEFT_CHLID, bit_str, len);
        //printf("%d  ", node->weight);
        hfm_traversal(hfm_tree, tnode->right, RELATION_RIGHT_CHLID, bit_str, len);
    } else {
        bit_str[len] = '\0';
        strncpy(tnode->codes, bit_str, BIT_STR_SIZE);
        tnode->codes_len = len;
        hfm_tree->leaf_list[hfm_tree->_index ++] = tnode;
        //printf("[0x%02x] [0x%s] [%d]\n", tnode->orig_byte, tnode->codes, tnode->weight);
    }
}

TranCodeTable *hfm_gen_tran_code_table(HafumanTree *hfm_tree)
{
    char bit_str[BIT_STR_SIZE] = "";
    hfm_traversal(hfm_tree, hfm_tree->root, RELATION_ROOT, bit_str, 0);
    qsort(hfm_tree->leaf_list, hfm_tree->leaf_count, sizeof(Tnode *), cmp_tnode);
    /*int i;
    for (i = 0; i < hfm_tree->leaf_count; i ++) {
        Tnode *tnode = hfm_tree->leaf_list[i];
        printf("[0x%2x] [0x%s] [%d]\n", tnode->orig_byte, tnode->codes, tnode->weight);
    }*/
    TranCodeTable *table = (TranCodeTable *)malloc(sizeof(TranCodeTable));
    assert(table != NULL);
    bzero(table, sizeof(TranCodeTable));
    int i;
    for (i = 0; i < hfm_tree->leaf_count; i ++) {
        TranCode *item = (TranCode *)malloc(sizeof(TranCode));
        assert(item != NULL);
        Tnode *tnode = hfm_tree->leaf_list[i];
        item->orig_byte = tnode->orig_byte;
        strcpy(item->codes, tnode->codes);
        item->codes_len = tnode->codes_len;
        table->list[table->count ++] = item;
    }
    return table;
}

HafumanTree *hfm_create(UcharStatistics *p_stat)
{
    Tnode *nlist[256] = {NULL};
    int leaf_count = 0;
    int i;
    for (i = 0; i < 256; i ++) {
        if (p_stat->counts[i] == 0)
            continue;
        Tnode *tnode = (Tnode *)malloc(sizeof(Tnode));
        assert(tnode != NULL);
        bzero(tnode, sizeof(Tnode));
        tnode->type = TYPE_LEAF_NODE;
        tnode->orig_byte = (unsigned char)i;
        tnode->weight = p_stat->counts[i];
        nlist[leaf_count ++] = tnode;
    }
    int n = leaf_count;
    while (n > 1) {
        qsort(nlist, n, sizeof(Tnode *), cmp_tnode);    //大的在前小的在后
        Tnode *tnode = (Tnode *)malloc(sizeof(Tnode));
        assert(tnode != NULL);
        bzero(tnode, sizeof(Tnode));
        tnode->type = TYPE_TRUNK_NODE;
        tnode->left = nlist[n-1];
        tnode->right = nlist[n-2];
        tnode->weight = nlist[n-1]->weight + nlist[n-2]->weight;
        nlist[n-2] = tnode;
        n --;
    }
    HafumanTree *hfm_tree = (HafumanTree *)malloc(sizeof(HafumanTree));
    assert(hfm_tree != NULL);
    bzero(hfm_tree, sizeof(HafumanTree));
    hfm_tree->root = nlist[0];
    hfm_tree->leaf_count = leaf_count;
    return hfm_tree;
}

void hfm_traversal_to_destroy(Tnode *tnode)
{
    if (tnode->type == TYPE_TRUNK_NODE) {
        hfm_traversal_to_destroy(tnode->left);
        hfm_traversal_to_destroy(tnode->right);
    }
    free(tnode);
}

void hfm_destroy(HafumanTree *hfm_tree)
{
    hfm_traversal_to_destroy(hfm_tree->root);
    free(hfm_tree);
}

void tran_code_table_destroy(TranCodeTable *tc_table)
{
    int i;
    for (i = 0; i < tc_table->count; i ++) {
        free(tc_table->list[i]);
    }
    free(tc_table);
}

const char *orig2codes(TranCodeTable *tc_table, unsigned char orig_byte)
{
    int i;
    for (i = 0; i < tc_table->count; i ++) {
        if (tc_table->list[i]->orig_byte == orig_byte)
            break;
    }
    return tc_table->list[i]->codes;
}

int codes2orig(TranCodeTable *tc_table, const char codes[], int len, unsigned char *p_orig_byte)
{
    int i;
    for (i = 0; i < tc_table->count; i ++) {
        if (tc_table->list[i]->codes_len != len) {
            if (tc_table->list[i]->codes_len < len)
                continue;
            break;
        }
        if (strncmp(tc_table->list[i]->codes, codes, len) == 0) {
            *p_orig_byte = tc_table->list[i]->orig_byte;
            return 0;
        }
    }
    return -1;
}

unsigned char codes2uchar(char codes[])
{
    unsigned char uchar = 0;
    int i;
    for (i = 0; i < 8; i ++) {
        if (codes[i] == '1') {
            uchar += (unsigned char)(pow(2, 8-1-i));
        }
    }
    return uchar;
}

void uchar2codes(unsigned char unchar, char codes[])
{
    int i;
    for (i = 0; i < 8; i ++) {
        codes[i] = '0';
    }
    codes[8] = '\0';
    int n = 8 - 1;
    while (unchar > 0) {
        codes[ n --] = unchar % 2 + '0';
        unchar /= 2;
    }
}

int compress(const char *infile_name, const char *outfile_name)
{
    // 统计文中 0-255 uchar 出现的频度
    UcharStatistics stat;
    uchar_statistics(infile_name, &stat);
    
    // 根据频度建哈夫曼树
    HafumanTree *hfm_tree = hfm_create(&stat);
   
    //遍历哈夫曼树 计算各uchar的转码  生成转换表
    TranCodeTable *tc_table;
    tc_table = hfm_gen_tran_code_table(hfm_tree);

    //不再需要哈夫曼树
    hfm_destroy(hfm_tree);

    //对文件进行编码,并写入到outfile中
    FILE *infile = fopen(infile_name, "rb");
    fseek(infile, 0, SEEK_END);
    int file_length = ftell(infile);
    fseek(infile, 0, SEEK_SET);
    FILE *outfile = fopen(outfile_name, "wb");
    assert(infile != NULL && outfile != NULL);
    
    //写入头信息
    /*
    |原文件长度(4)|编码表个数(4)|
    |编码表1 
    ....
    |编码表n
    |       编码后的数据        |
     */
    fwrite(&file_length, sizeof(int), 1, outfile);
    fwrite(&tc_table->count, sizeof(int), 1, outfile);
    int i;
    for (i = 0; i < tc_table->count; i ++) {
        fwrite(tc_table->list[i], sizeof(TranCode), 1, outfile);
    }

    //写入编码后的数据
    //将字符编码对照哈夫曼中对应的表进行转换
    char buf[513] = "\0";   //编码缓冲区
    unsigned char uchar;
    while (fread(&uchar, sizeof(unsigned char), 1, infile) > 0) {
        strcat(buf, orig2codes(tc_table, uchar));
        while (strlen(buf) >= 8) {
            uchar = codes2uchar(buf);
            fwrite(&uchar, sizeof(unsigned char), 1, outfile);
            strcpy(buf, buf + 8);
        }
    }
    if (strlen(buf) > 0) {
        strcat(buf, "0000000");
        uchar = codes2uchar(buf);
        fwrite(&uchar, sizeof(unsigned char), 1, outfile);
    }
    tran_code_table_destroy(tc_table);
    fclose(infile);
    fclose(outfile);
    return 0;
}

int uncompress(const char *infile_name, const char *outfile_name)
{
    FILE *infile = fopen(infile_name, "rb");
    FILE *outfile = fopen(outfile_name, "wb");
    assert(infile != NULL && outfile != NULL);

    //读取头信息
    int orig_length;
    TranCodeTable *tc_table = (TranCodeTable *)malloc(sizeof(TranCodeTable));
    assert(tc_table != NULL);
    fread(&orig_length, sizeof(int), 1, infile);
    //printf("orig_length: %d\n", orig_length);
    fread(&tc_table->count, sizeof(int), 1, infile);
    int i;
    for (i = 0; i < tc_table->count; i ++) {
        TranCode *item = (TranCode *)malloc(sizeof(TranCode));
        assert(item != NULL);
        fread(item, sizeof(TranCode), 1, infile);
        tc_table->list[i] = item;
        //printf("[0x%02x] [0x%s]\n", item->orig_byte, item->codes);
    }

    //解码
    char buf[513] = "\0";
    char codes[9] = "\0";
    unsigned char uchar;
    int len, test_len = 1, length = 0;
    while (fread(&uchar, sizeof(unsigned char), 1, infile) > 0) {
        uchar2codes(uchar, codes);
        //printf("%s", codes);
        strcat(buf, codes);
        len = strlen(buf);
        while (test_len < len) {
            if (codes2orig(tc_table, buf, test_len, &uchar) == 0) {
                fwrite(&uchar, sizeof(unsigned char), 1, outfile);
                strcpy(buf, buf + test_len);
                len -= test_len;
                test_len = 1;
                length ++;
                if (length == orig_length)
                    break;
                continue;
            }
            test_len ++;
            if (test_len > BIT_STR_SIZE) {
                printf("Error: 无法解码, 可能数据有损坏.\n");
                exit(-1);
            }
        }
    }

    tran_code_table_destroy(tc_table);
    fclose(infile);
    fclose(outfile);
    return 0;
}

void print_usage(const char *cmd_name)
{
    printf("usage: %s [option] [infile] [outfile]\n", cmd_name);
    printf("     -c: 压缩\n");
    printf("     -x: 解压\n");
}

int main(int argc, const char **argv)
{
    if (argc < 4) {
        print_usage(argv[0]);
        exit(1);
    }
    if (strcmp(argv[1], "-c") == 0) {
        compress(argv[2], argv[3]);
    } else if (strcmp(argv[1], "-x") == 0) {
        uncompress(argv[2], argv[3]);   
    } else {
        print_usage(argv[0]);
        exit(1);
    }
    return 0;
}

