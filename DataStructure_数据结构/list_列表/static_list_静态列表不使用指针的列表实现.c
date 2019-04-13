/**
 * 静态链表
 */
#include<stdio.h>

typedef struct {
    char    data;
    int     cur;
}Node;

#define MAXSIZE 100


void __show_memorry(Node list[])
{
    //int i;
    //for (i = 0; i <=10; i ++) {
    //    printf(" [%d]%d", i, list[i].cur);
    //}
    //printf(" [%d]%d", MAXSIZE+1, list[MAXSIZE+1].cur);
    //printf("\n");

}

void list_init(Node list[])
{
    int i;
    for (i = 1; i < MAXSIZE; i ++) {
        list[i].cur = i+1;
    }
    list[MAXSIZE].cur = 0;
    list[0].cur = 1;
    list[MAXSIZE + 1].cur = 0;
}

int _list_malloc(Node list[])
{
    int i;
    int n = list[0].cur;
    if (n != 0) {
        list[0].cur = list[n].cur;
    }
    return n;
}

void _list_free(Node list[], int n)
{
    list[n].cur = list[0].cur;
    list[0].cur = n;
}

int list_total(Node list[])
{
    int k = list[MAXSIZE + 1].cur;
    int total = 0;
    while (k != 0) {
        total ++;
        k = list[k].cur;
    }
    return total;
}

int list_append(Node list[], char data)
{
    int n = _list_malloc(list);
    if (n == 0) {
        printf("[Err]:fun=list_append|info=No space to append any data.\n");
        return -1;
    }
    list[n].data = data;
    list[n].cur = 0;
    printf("[Debug]:elemet %c at %d\n", data, n);
    int k = list[MAXSIZE + 1].cur;
    if (k == 0) {
        list[MAXSIZE + 1].cur = n;
    } else {
        while (list[k].cur != 0) {
            k = list[k].cur;
        }
        list[k].cur = n;
    }
    __show_memorry(list);
    return 0;
}

int list_find(Node list[], int pos, char *pdata)
{
    int total = list_total(list);
    if (pos < 0 || pos >= total) {
        printf("[Debud]:no such pos %d\n", pos);
        return -1;
    }
    int k = list[MAXSIZE + 1].cur;
    int i;
    for (i = 0; i < pos; i ++) {
        k = list[k].cur;
    }
    *pdata = list[k].data;
    return k;
}

int list_insert(Node list[], int pos, char data)
{
    int total = list_total(list);
    if (pos < 0)
        pos = 0;
    else if (pos >= total)
        return list_append(list, data);
    int n = _list_malloc(list);
    if (n == 0) {
        printf("[Err]:fun=list_insert|info=No space to insert any data.\n");
        return -1;
    }
    list[n].data = data;
    printf("[Debug]:elemet %c at %d\n", data, n);
    int k = list[MAXSIZE + 1].cur;
    if (pos == 0) {
        list[n].cur = k;
        list[MAXSIZE + 1].cur = n;
        __show_memorry(list);
        return 0;
    }
    int i;
    for (i = 1; i < pos; i ++) {
        k = list[k].cur;
    }
    list[n].cur = list[k].cur;
    list[k].cur = n;
    __show_memorry(list);
    return 0;
}

int list_remove(Node list[], int pos, char *pdata)
{
    int total = list_total(list);
    if (pos < 0 || pos >= total) {
        printf("[Debug]:no such pos %d", pos);
        return -1;
    }
    int k;
    if (pos == 0) {
        k = list[MAXSIZE + 1].cur;
        list[MAXSIZE + 1].cur = list[k].cur;
    } else {
        int prev = list_find(list, pos-1, pdata);
        k = list[prev].cur;
        list[prev].cur = list[k].cur;
    }
    *pdata = list[k].data;
    _list_free(list, k);
    __show_memorry(list);
    return 0;
}

void list_show(Node list[])
{
    int k = list[MAXSIZE + 1].cur;
    printf("==============list=============\n");
    while (k != 0) {
        printf(" [%d] %c\n", k, list[k].data);
        k = list[k].cur;
    }
    printf("\n");
}

int main()
{   
    Node list[MAXSIZE + 2];
    list_init(list);
    
    printf("Insert 10 elements into list.\n");
    int i;
    for (i = 0; i < 10; i ++) {
        list_append(list, 'a'+i);
    }
    list_show(list);

    char ch = ' ';
    printf("Remove pos=3 element from list.\n");
    list_remove(list, 3, &ch);
    printf("Remove pos=1 element from list.\n");
    list_remove(list, 1, &ch);

    list_show(list);

    printf("Insert pos=0 'B' element into list.\n");
    list_insert(list, 0, 'B');
    printf("Insert pos=0 'A' element into list.\n");
    list_insert(list, 0, 'A');
    printf("Insert pos=1000 '.' element into list\n");
    list_insert(list, 1000, '.');
    list_show(list);

    list_find(list, 7, &ch);
    printf("The 7th ch = %c\n", ch);
    return 0;
}

