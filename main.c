#include <stdlib.h>
#include <time.h>
#include "sds.h"
#include "adlist.h"
#include "dict.h"
#include "parse.h"

void testMakeValue() {
    sdshdr *p = makeSdsHdr("hello world");
    Value *v = makeValue(p, STRING);
    sdshdr *toStr = ValueToString(v);
    printf("%s", toStr->buf);
    sdshdrRelease(toStr);
}

void testlistToString() {
    list *l = listCreate();
    for (int i = 0; i < 10; i++) {
        listAddNodeTail(l, makeValueInt(rand()));
        listAddNodeTail(l, makeValueStr("what fuck"));
    }
    list *ll = listCreate();
    for (int i = 0; i < 100; i++) {
        listAddNodeTail(ll, makeValueList(l));

    }
    Value *v = makeValue(ll, LIST);
    clock_t start = clock();
    sdshdr *s = ValueToString(v);
    printf("%s", s->buf);
    clock_t end = clock();
    printf("used time is %.2fms", 1000 * (double) (end - start) / CLOCKS_PER_SEC);
    sdshdrRelease(s);
}


void testDictToString() {
    Dict *d1 = makeDict();
    list *h = listCreate();
    listAddNodeTail(h, makeValueInt(1));
    addKeyValue(d1, "a", makeValueList(listCopy(h)));
    addKeyValue(d1, "b", makeValueInt(2));
    Dict *d2 = makeDict();
    addKeyValue(d2, "d1", makeValueDict(copyDict(d1)));
    sdshdr *toString = ValueToString(makeValueDict(d2));
    printf("%s", toString->buf);
    sdshdrRelease(toString);
    releaseDict(d2);
    releaseDict(d1);
    printf("%d", h->ref_count);
}


void read_file(FILE *fp, sdshdr *s) {
    char *c = malloc(1);
    while (1) {
        *c = (char) fgetc(fp);
        if (*c == EOF) return;//到文件尾，不存在下一行
        sdsJoinchar(s, c);
    }
    free(c);
}


int main(int argc, char *argv[]) {
    testDictToString();
    char *filename = argv[1];
    FILE *fp = NULL;
    if ((fp = fopen(filename, "r")) == NULL) {
        printf("%s not exists", filename);
        exit(1);
    }
    sdshdr *p = makeSdsHdr("");
    read_file(fp, p);
    // printf("%s", p->buf);
    clock_t s1 = clock();
    Value *data = parse(p);
    sdshdr *s = ValueToString(data);
    clock_t end = clock();
    printf("%f", 1000 * (double) (end - s1) / CLOCKS_PER_SEC);
    fclose(fp);
    vPrintf("%s", data);
    sdshdrRelease(s);
    releaseValue(data);
}