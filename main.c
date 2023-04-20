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
    char *c = malloc(2);
    while (1) {
        memset(c, 0, 2);
        *c = (char) fgetc(fp);
        if (*c == EOF) {
            free(c);
            return;
        }//到文件尾，不存在下一行
        sdsJoinchar(s, c);
    }
    //free(c);
}


int main(int argc, char *argv[]) {
    char *filename = argv[1];
    FILE *fp = NULL;
    if ((fp = fopen(filename, "r")) == NULL) {
        printf("%s not exists", filename);
        exit(1);
    }
    sdshdr *p = makeSdsHdr("");
    read_file(fp, p);
    Value *data = parse(p);
    sdshdr *buf = ValueToString(data);
    printf("%s", buf->buf);
    sdshdrRelease(buf);
    releaseValue(data);
    sdshdrRelease(p);
    fclose(fp);

}