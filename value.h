#pragma once

#include "sds.h"
#include "adlist.h"
#include <assert.h>
#include <time.h>

typedef size_t Label;


struct Dict;

typedef struct Dict Dict;

sdshdr *DictToString(Dict *dict, sdshdr *ctx);

enum {
    INT = 1,
    FLOAT,
    DOUBLE,
    STRING,
    LIST,
    VALUE,
    DICT,
    _NULL,
    True,
    False
};

const char *getTypeName(int type) {
    switch (type) {
        case INT:
            return "INT";
        case FLOAT:
            return "FLOAT";
        case DOUBLE:
            return "DOUBLE";
        case STRING:
            return "STRING";
        case LIST:
            return "LIST";
        case VALUE:
            return "VALUE";
        case DICT:
            return "DICT";
        default:
            return "known type";
    }
}


typedef struct Value {

    union {
        sdshdr *str;
        int *number;
        double *doubleNumber;
        list *list;
        struct Value *value;
        Dict *dict;
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

Value *copyValue(Value *v) {
    return makeValue(v->data, v->label);
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

Value *makeValueDict(Dict *dict) {
    Value *v = makeValue(dict, DICT);
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

char *dellastZero(char *s) {
    size_t len = strlen(s);
    while (--len) {
        if (s[len] == '0')
            s[len] = 0;
        else
            break;
    }
    return s;
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
            break;

        case DOUBLE: {
            char cache[40 + 16] = {0};
            memset(cache, 0, 40 + 16);
            sprintf(cache, "%f", *(double *) v->number);
            dellastZero(cache);
            sdsJoinchar(toStr, cache);
            break;
        }

        case LIST: {
            listToString(v->list, ListToStringCallback, toStr);
            break;
        }
        case VALUE: {
            sdshdr *p = ValueToString(v);
            sdsJoinchar(toStr, p->buf);
            sdshdrRelease(p);
            break;
        }
        case STRING:
            sdsJoinchar(toStr, v->str->buf);
            break;

        case DICT:
            DictToString(v->dict, toStr);
            break;

        case _NULL:
            sdsJoinchar(toStr, "null");
            break;


        case True:
            sdsJoinchar(toStr, "true");
            break;

        case False:
            sdsJoinchar(toStr, "false");
            break;

        default:
            printf("error type! please check type");
            exit(1);
    }
    return toStr;
}

void vPrintf(const char *format, Value *v) {
    sdshdr *toString = ValueToString(v);
    if (!strcmp("%s", format)) {
        printf("%s", toString->buf);
    }
    sdshdrRelease(toString);
}


void vPrintfUsedTime(const char *format, Value *v) {
    clock_t start = clock();
    vPrintf(format, v);
    clock_t end = clock();
    printf("\n\"used time is %.2fms\"", 1000 * (double) (end - start) / CLOCKS_PER_SEC);
}
