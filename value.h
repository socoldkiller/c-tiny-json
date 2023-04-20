#pragma once

#include "sds.h"
#include "adlist.h"
#include <assert.h>
#include <time.h>

typedef size_t Label;


struct Dict;

typedef struct Dict Dict;

void releaseDict(Dict *dict);


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
    size_t ref_count;

} Value;


void releaseValue(void *value) {
    Value *v = value;
    v->ref_count--;
    assert(v->ref_count >= 0);
    if (v->ref_count > 0)
        return;
    switch (v->label) {
        case _NULL:
        case True:
        case False:
            break;

        case INT:
        case DOUBLE:
        case FLOAT:
            free(v->data);
            break;

        case DICT:
            releaseDict(v->dict);
            break;
        case LIST:
            v->list->freeNode = releaseValue;
            listRelease(v->list);
            break;
        case STRING:
            sdshdrRelease(v->str);
            break;

    }
    free(v);
}


Value *makeValue(void *data, Label label) {
    Value *value;
    if ((value = malloc(sizeof(*value))) == NULL) {
        return NULL;
    }
    value->data = data;
    value->label = label;
    value->ref_count = 1;
    return value;
}


Value *copy(Value *v) {
    assert(v != NULL);
    v->ref_count++;
    return v;
}

Value *deepcopy(Value *v) {
    // refernce data ?
    Value *copy = makeValue(v->data, v->label);
    copy->ref_count = 1;
    return v;
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

    static char number[40 + 16] = {0};
    sdshdr *toStr = makeSdsHdr("");

    switch (v->label) {
        case INT: {
            memset(number, 0, 40 + 16);
            sprintf(number, "%d", *(int *) v->number);
            sdsJoinchar(toStr, number);
            break;
        }
        case FLOAT:
            break;

        case DOUBLE: {
            memset(number, 0, 40 + 16);
            sprintf(number, "%f", *(double *) v->number);
            dellastZero(number);
            sdsJoinchar(toStr, number);
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
    if (!v) {
        return;
    }
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
