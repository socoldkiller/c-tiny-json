#pragma once

#include "adlist.h"
#include "value.h"

typedef struct Pair {
    sdshdr *key;
    Value *value;
} Pair;

Pair *makePair(const char *key, Value *value) {
    Pair *p;
    if ((p = malloc(sizeof(*p))) == NULL) {
        return NULL;
    }

    p->key = makeSdsHdr(key);
    p->value = value;
    return p;
}


typedef struct Dict {
    list *l;
} Dict;


Dict *makeDict() {
    Dict *d;
    if ((d = malloc(sizeof(*d))) == NULL) {
        return NULL;
    }
    d->l = listCreate();
    return d;
}

Dict *addKeyValue(Dict *d, const char *key, Value *value) {
    listAddNodeTail(d->l, makePair(key, value));
    return d;
}

void *DictKey(Dict *d, const char *key) {
    list *list = d->l;
    listIterDistance *head = listGetIteratorDistance(list, AL_START_HEAD, 0);
    listNode *node;
    void *rValue =NULL;
    while ((node = listDistanceNext(head)) != NULL) {
        Pair *v = node->value;
        if (!strcmp(v->key->buf, key)) {
            rValue =  v->value;
        }
    }
    listReleaseDistance(head);
    return rValue;
}

sdshdr *DictToString(Dict *dict, sdshdr *ctx) {
    if (!ctx) {
        assert("ctx is null");
        exit(1);
    }
    sdsJoinchar(ctx, "{");
    list *list = dict->l;
    listIterDistance *head = listGetIteratorDistance(list, AL_START_HEAD, 0);
    listNode *node;
    while ((node = listDistanceNext(head)) != NULL) {
        Pair *v = node->value;
        sdsJoinchar(ctx, v->key->buf);
        sdsJoinchar(ctx, ":");
        sdshdr *valueToString = ValueToString(v->value);
        sdsJoinchar(ctx, valueToString->buf);
        sdshdrRelease(valueToString);
        if (node->next) {
            sdsJoinchar(ctx, ",");
        }
    }
    sdsJoinchar(ctx, "}");
    listReleaseDistance(head);
    return ctx;
}