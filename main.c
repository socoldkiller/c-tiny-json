#include <stdlib.h>
#include "sds.h"
#include "adlist.h"
#include "dict.h"
#include <time.h>
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
}

void testIsInt() {
    sdshdr *s = makeSdsHdr("121.32");
    context_string c = {s, 0};
    int n = isInt(&c);
    Value *v;
    if ((v = malloc(sizeof(*v))) == NULL) {
        return;
    }
    parseNumber(v, &c);
    vPrintf("%s", v);
    printf("%d", n);
};


int main() {
//   // testDictToString();
    FILE *fp = fopen("../a.json", "r");
    sdshdr *s = makeSdsHdr("");
    read_file(fp, s);
//    Value *data = parse(s);
//    list *h =listCreate();
//    listAddNodeTail(h, makeValueInt(1));
//    listAddNodeTail(h, makeValueInt(2222));
//    listAddNodeTail(data->list, makeValueList(h));
//    vPrintf("%s",data);
    Value *data = parse(s);
    vPrintf("%s", data);
}