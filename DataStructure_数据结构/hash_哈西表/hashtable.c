#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

typedef struct _node {
    char *key;
    char *value;
    struct _node *next;
}Node;

typedef struct _hash_table {
    Node **table;
    int table_size;
    int enable_auto_free;
    int total;
}HashTable;

HashTable * create_hash_table(int size, int auto_free)
{
    HashTable *hash_table = (HashTable *)malloc(sizeof(HashTable));
    assert(hash_table != NULL);
    
    hash_table->table = (Node **)malloc(sizeof(Node *) * size);
    assert(hash_table->table != NULL);

    hash_table->table_size = size;
    
    hash_table->enable_auto_free = auto_free;

    hash_table->total = 0;
    int i;
    for (i =  0; i < hash_table->table_size; i ++) {
        hash_table->table[i] = NULL;
    }
    return hash_table;
}

Node * _create_node(char *key, char *value)
{
    Node *node = (Node *)malloc(sizeof(Node));
    assert(node != NULL);
    bzero(node, sizeof(Node));

    node->key = key;
    node->value = value;

    return node;
}

int hash(HashTable *hash_table, const char *key)
{
    int index = 0;
    int key_len = strlen(key);
    int i;
    for (i = 0; i < key_len; i ++) {
        index = (key[i]& 0xff) + (index *31);
    }
    index = abs(index % hash_table->table_size);
    
    //printf("--index = %d ---\n", index);
    return index;
}

int has_key(HashTable *hash_table, const char *key)
{
    int index = hash(hash_table, key);
    Node *node = hash_table->table[index];
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            return 1;
        }
        node = node->next;
        printf("===key:%s node->next:%0x===\n", key, node);
    }
    return 0;
}

void delete(HashTable *hash_table, const char *key)
{
    int index = hash(hash_table, key);
    Node *node = hash_table->table[index], *prev = NULL;
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            break;
        }
        prev = node;
        node = node->next;
    }
    if (node == NULL)
        return;
    if (prev != NULL) {
        prev->next = node->next;
    } else {
        hash_table->table[index] = node->next;
    }
    if (hash_table->enable_auto_free !=  0) {
        free(node->key);
        free(node->value);
    }
    free(node);
    hash_table->total --;
}

int insert(HashTable *hash_table, char *key, char *value)
{
    int index = hash(hash_table, key);
    Node *node = hash_table->table[index];
    if (node == NULL) {
        hash_table->table[index] = _create_node(key, value);
    } else {
        Node *prev = NULL;
        while (node != NULL) {
            if (strcmp(node->key, key) == 0) {
                if (hash_table->enable_auto_free != 0) {
                    free(node->key);
                    free(node->value);
                }
                node->key = key;
                node->value = value;
                return 0;
            }
            prev = node;
            node = node->next;
        }
        prev->next = _create_node(key, value);
    }
    hash_table->total ++;
    return 0;
}

Node *_get_node(HashTable *hash_table, const char *key)
{
    int index = hash(hash_table, key);
    Node *node = hash_table->table[index];
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            break;
        }
        node = node->next;
    }
    return node;
}

char *get(HashTable *hash_table, const char *key)
{
    Node *node = _get_node(hash_table, key);
    if (node != NULL)
        return node->value;
    return NULL;
}


void clear(HashTable *hash_table)
{
    Node *node, *tmp;
    int index = 0;
    int size = hash_table->table_size;
    for (;index < size; index ++) {
        node = hash_table->table[index];
        while (node != NULL) {
            tmp = node;
            node = node->next;
            if (hash_table->enable_auto_free != 0) {
                free(tmp->key);
                free(tmp->value);
            }
            free(tmp);
        }
        hash_table->table[index] = NULL;
    }
}

void print_hash_table(const HashTable *hash_table)
{
    Node *node;
    int index = 0;
    int size = hash_table->table_size;
    for (;index < size; index ++) {
        node =  hash_table->table[index];
        if (node != NULL) {
            printf("\n   [%d] ", index);
        }
        while (node != NULL) {
            printf("\"%s\":\"%s\" ", node->key, node->value);
            node = node->next;
        }
    }
    printf("\nTotal: %d\n", hash_table->total);
}

int main()
{
    char key[10];
    char value[21];
    HashTable * hash_table = create_hash_table(21, 1);
    int i;
    for (i = 0; i < 10; i ++) {
        snprintf(key, sizeof(key), "key_%03d", i);
        snprintf(value, sizeof(value), "value_%03d", i);
        insert(hash_table, strdup(key), strdup(value));
    }
    print_hash_table(hash_table);
    
    for (i = 5; i < 100; i ++) {
        snprintf(key, sizeof(key), "key_%03d", i);
        snprintf(value, sizeof(value), "value_%03d", i);
        insert(hash_table, strdup(key), strdup(value));
    }
    print_hash_table(hash_table);
    
    for (i = 0; i < 10; i ++) {
        snprintf(key, sizeof(key), "key_%03d", i);
        assert(has_key(hash_table, key) != 0);
    } 

    delete(hash_table, "key_045");
    
    char *v = get(hash_table, "key_024");
    assert(strcmp(v, "value_024") == 0);

    print_hash_table(hash_table);
    clear(hash_table);
    return 0;
}
