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
    listAddNodeTail(l, makeValueStr("hello,world"));
    int *n = malloc(sizeof(int));
    *n = 100;
    listAddNodeTail(l, makeValue(n, INT));
    list *l2 = listCreate();
    listAddNodeTail(l2, makeValue(l, LIST));
    listAddNodeTail(l2, makeValue(l, LIST));
    listAddNodeTail(l2, makeValue(l, LIST));
    listAddNodeTail(l2, makeValue(n, INT));
    Value *p = makeValue(l2, LIST);
    sdshdr *s = ValueToString(p);
    printf("%s", s->buf);
    sdshdrRelease(s);
}


int main() {
    // testsdshdrList();
    //testMakeValue();
    testlistToString();
    return 0;
}