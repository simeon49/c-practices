#include <stdio.h>
#include <string.h>

void get_next(const char *t, int *next)
{
    int i = 0, j = 0;
    int len = strlen(t);
    next[0] = 0;
    while (i < len) {
        if (t[i] == t[j]) {
            i ++;
            j ++;
            next[i] = j;
        } else {
            j = next[j];
        }
    }
}

int index_kmp(const char *s, const char * t, int pos)
{
    int next[255];
    int i;
    int len = strlen(t);
    get_next(t, next);
    for (i = 0; i < len; i ++) {
        printf(" %d", next[i]);
    }
    printf("\n");
}

int main()
{
    char s[] = "goodgooggoodgoogleasgon";
    //char t[] = "goodgoogle";
    char t[] = "abcabcd";
    int pos = index_kmp(s, t, 0);
    printf("s=%s\nt=%s\npos=%d\n", s, t, pos);
    return 0;
}

