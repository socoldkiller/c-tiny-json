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
    list *l = listCreate();
    Dict *d = makeDict();
    for (int i = 0; i < 3; i++) {
        listAddNodeTail(l, makeValueInt(rand()));
        listAddNodeTail(l, makeValueStr("hello"));
    }
    addKeyValue(d, "a", makeValueList(l));
    addKeyValue(d, "ae", makeValueStr("what"));
    addKeyValue(d, "abc", makeValueInt(1));
    addKeyValue(d, "dict", makeValueDict(makeDict()));
    Dict *dict = makeDict();
    addKeyValue(dict, "d", makeValueDict(d));
    addKeyValue(dict, "saf", makeValueDict(d));
    vPrintfUsedTime("%s", makeValueDict(dict));
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
    char *filename = argv[1];
    FILE *fp = fopen(filename, "r");
    sdshdr *p = makeSdsHdr("");
    read_file(fp, p);
    // printf("%s", p->buf);
    clock_t s1 = clock();
    Value *data = parse(p);
    sdshdr *s = ValueToString(data);
    clock_t end = clock();
    printf("%f", 1000 * (double) (end - s1) / CLOCKS_PER_SEC);
    fclose(fp);
    // printf("\n\n\n%c", s->buf[s->length - 1]);

}