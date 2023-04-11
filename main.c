#include <stdlib.h>
#include "sds.h"
#include "adlist.h"
#include <string.h>
#include <time.h>

sdshdr *p;

void callback(size_t idx, listNode *n) {
    sdshdr *r = n->value;
    sdsJoinchar(p, r->buf);
    sdshdrPrint(r);
}

sdshdr *listToString(const list *l);


int cmp(listNode *n, void *s) {
    sdshdr *sds = n->value;
    const char *s1 = s;
    return !strcmp(sds->buf, s);
}


int findListIndexStr(const list *l, const char *s) {
    return findlistNode(l, s, cmp);
}

void testsdshdrList() {
    char s[50] = {0};
    int n = 1;
    list *l = listCreate();
    sdshdr *tmp = makeSdsHdr("hello world");
    listAddNodeTail(l, tmp);
    while (n--) {
        memset(s, 0, sizeof(char) * 50);
        int limit = rand() % 50;
        for (int i = 0; i < limit; i++) {
            s[i] = rand() % 26 + 'a';
        }

        sdshdr *sp = makeSdsHdr(s);
        listAddNodeTail(l, sp);
    }

    sdshdr *s1 = listToString(l);
    printf("%s", s1->buf);
    int _bool = findListIndexStr(l, "hello world");
    printf("%d", _bool);
    sdshdrRelease(s1);
}


sdshdr *listToString(const list *l) {
    sdshdr *s = makeSdsHdr("[");
    listIterDistance *head = listGetIteratorDistance(l, AL_START_HEAD, 0);
    listNode *node;
    while ((node = listDistanceNext(head)) != NULL) {
        sdshdr *data = node->value;
        sdsJoinchar(s, "\"");
        sdsJoinchar(s, data->buf);
        sdsJoinchar(s, "\"");
        sdsJoinchar(s, ",");
    }
    //delete last ,
    size_t len = strlen(s->buf);
    s->buf[len - 1] = 0;
    sdsJoinchar(s, "]");
    return s;
}


int main() {
    clock_t start = clock();
    srand(time(NULL));
    testsdshdrList();
    printf("\n");
    clock_t end = clock();
    printf("%.4fms", ((double) (end - start) / CLOCKS_PER_SEC) * 1000);
    return 0;
}


















