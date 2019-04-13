#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct NODE{
    struct NODE * prev;
    struct NODE * next;
    int num;
} NODE;

NODE *new_node(int num)
{
    NODE *node;
    node = malloc(sizeof(NODE));
    bzero(node, sizeof(NODE));
    node->num = num;
    return node;
}

int push_back(NODE *root, NODE *node)
{
    NODE *end_node = root;
    while (end_node->next != NULL) {
        end_node = end_node->next;
    }
    end_node->next = node;
    node->prev = end_node;
}

NODE *find(NODE *root, int num)
{
    NODE *node = root;
    while (node != NULL) {
        if (num == node->num) {
            break;
        }
        node = node->next;
    }
    return node;
}

void unlink_node(NODE **p_root, NODE *node)
{
    if (node == NULL)
        return;
    NODE *iterm = *p_root;
    while (iterm != NULL) {
        if (iterm == node) {
            break;
        }
        iterm = iterm->next;
    }
    if (iterm == NULL)
        return;
    if (iterm->prev == NULL)
        *p_root = iterm->next;
    else {
        iterm->prev->next = iterm->next;
        iterm->next->prev = iterm->prev;
    }
    node->next = NULL;
    node->prev = NULL;
}

void del_node(NODE *node)
{
    free(node);
}

void clear_list(NODE **p_root)
{
    NODE *iterm = *p_root;
    NODE *tmp;
    while (iterm != NULL) {
        tmp = iterm;
        iterm = iterm->next;
        free(tmp);
    }
    *p_root = NULL;
}

void reverse(NODE **p_root)
{
    NODE *root = *p_root;
    NODE *node = root, *next = node->next, *tmp = NULL;
    while (next != NULL) {
        tmp = next->next;
        node->prev = next;
        next->next = node;
        
        node = next;
        next = tmp;
    }
    root->next = NULL;
    node->prev = NULL;
    *p_root = node;
}


void print_list(NODE *root)
{
    NODE *node = root;
    int index = 0;
    while (node != NULL) {
        printf("[%d] %d\n", index, node->num);
        index ++;
        node = node->next;
    }
}

void do_test()
{
    NODE *prev = NULL, *next = NULL;
    NODE *root = NULL, *node = NULL;
    int i;
    for (i = 0; i < 21; i ++) {
        node = new_node(i+1);
        if (root == NULL) {
            root = node;
        } else {
            push_back(root, node);
        }
    }
    print_list(root);   

    node = find(root, 7);
    assert(node->num == 7);

    unlink_node(&root, node);
    del_node(node);

    printf("--- REVERSE ---");
    reverse(&root);
    print_list(root);

    printf("--- Clear ---");
    clear_list(&root);
}

int main()
{
    do_test();
    return 0;
}

