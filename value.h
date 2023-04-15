
#include "sds.h"
#include "adlist.h"
#include <assert.h>
typedef size_t Label;


enum {
    INT = 1,
    FLOAT,
    DOUBLE,
    STRING,
    LIST,
    VALUE,
    DICT,
};


typedef struct Value {

    union {
        sdshdr *str;
        int *number;
        double *doubleNumber;
        list *list;
        struct Value *value;
        void *data;
    };
    Label label;

} Value;


Value *makeValue(void *data, Label label) {
    Value *value;
    if ((value = malloc(sizeof(*value))) == NULL) {
        return NULL;
    }
    value->data = data;
    value->label = label;
    return value;
}


Value *makeValueInt(int n) {
    int *data = malloc(sizeof(int));
    *data = n;
    Value *v = makeValue(data, INT);
    return v;
}

Value *makeValueStr(const char *str) {
    Value *v = makeValue(makeSdsHdr(str), STRING);
    return v;
}

Value *makeValueList(list *l) {
    Value *v = makeValue(l, LIST);
    return v;
}


sdshdr *ValueToString(Value *v);

sdshdr *ListToStringCallback(list *l, sdshdr *ctx, void *v) {
    Value *value = v;
    sdshdr *str = ValueToString(value);
    sdsJoinchar(ctx, str->buf);
    sdshdrRelease(str);
    return ctx;
}

sdshdr *ValueToString(Value *v) {
    if (!v) {
        return NULL;
    }
    sdshdr *toStr = makeSdsHdr("");
    switch (v->label) {
        case INT: {
            char *cache = malloc(40 + 16);
            memset(cache, 0, 40 + 16);
            sprintf(cache, "%d", *(int *) v->number);
            sdsJoinchar(toStr, cache);
            free(cache);
            break;
        }

        case FLOAT:
        case DOUBLE:

        case LIST: {
            listToString(v->list, ListToStringCallback,toStr);
            // sdsJoinchar(toStr, p->buf);
          //  sdshdrRelease(p);
            break;
        }
        case VALUE: {
            sdshdr *p = ValueToString(v);
            sdsJoinchar(toStr, p->buf);
            sdshdrRelease(p);
            break;
        }

        case STRING:
            sdsJoinchar(toStr, "\"");
            sdsJoinchar(toStr, (const char *) (sdshdr *) v->str->buf);
            sdsJoinchar(toStr, "\"");
            break;
        default:
            printf("error type! please check type");
            exit(1);
    }
    return toStr;
}


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
    while ((node = listDistanceNext(head)) != NULL) {
        Pair *v = node->value;
        if (!strcmp(v->key->buf, key)){
            return v->value;
        }
    }
    return NULL;
}