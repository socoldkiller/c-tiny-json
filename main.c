#include <stdlib.h>
#include "sds.h"
#include "adlist.h"
#include "value.h"
#include <string.h>
#include <time.h>


int cmp(listNode *n, void *s) {
    sdshdr *sds = n->value;
    const char *s1 = s;
    return !strcmp(sds->buf, s);
}


int findListIndexStr(const list *l, const char *s) {
    return findlistNode(l, s, cmp);
}


void testMakeValue() {
    sdshdr *p = makeSdsHdr("hello world");
    Value *v = makeValue(p, STRING);
    sdshdr *toStr = ValueToString(v);
    printf("%s", toStr->buf);
    sdshdrRelease(toStr);
}

sdshdr *cl(list *l, sdshdr *ctx, void *value) {
    sdshdr *v = value;
    sdsJoinchar(ctx, "\"");
    sdsJoinchar(ctx, v->buf);
    sdsJoinchar(ctx, "\"");
    return ctx;
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


int main() {
    // testsdshdrList();
    //testMakeValue();
    testlistToString();
    return 0;
}