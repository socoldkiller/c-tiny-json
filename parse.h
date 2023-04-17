#pragma once

#include <ctype.h>
#include "value.h"
#include <assert.h>
#include <ctype.h>

typedef struct context_string {
    sdshdr *buf;
    size_t now_index;
} context_string;


char *string_view(context_string *ctx) {
    return ctx->buf->buf + ctx->now_index;
}


int isInt(context_string *context_string) {
    char *p = string_view(context_string);
    while (p[0]) {
        if (p[0] == '.')
            return DOUBLE;

        if (!isdigit(p[0]))
            break;
        p++;
    }
    return INT;
}


Value *deepCopyInt(Value *v, int n) {
    int *heap = malloc(sizeof(int));
    *heap = n;
    v->number = heap;
    v->label = INT;
    return v;
}

Value *deepCopyDouble(Value *v, double n) {
    double *heap = malloc(sizeof(double));
    *heap = n;
    v->doubleNumber = heap;
    v->label = DOUBLE;
    return v;
}


Value *_parse(context_string *ctx_string);

Value *parseNumber(Value *ctx, context_string *ctx_string) {
    char *p = string_view(ctx_string);
    char *endpoint = NULL;
    int n = isInt(ctx_string);
    if (n == INT) {
        int data = (int) strtod(p, &endpoint);
        deepCopyInt(ctx, data);

    } else if (n == DOUBLE) {
        double data = strtod(p, &endpoint);
        deepCopyDouble(ctx, data);
    }
    ctx_string->now_index += (endpoint - p);
    return ctx;
}


Value *parseString(Value *ctx, context_string *ctx_string) {
    // skip "
    ctx_string->now_index += 1;
    sdshdr *s = makeSdsHdr("\"");
    char *start = string_view(ctx_string);
    char *tmp = malloc(1);
    memset(tmp, 0, 1);
    for (size_t i = 0;; i++) {
        ctx_string->now_index += 1;
        char ch = start[i];
        tmp[0] = ch;
        sdsJoinchar(s, tmp);
        if (ch == '\"')
            break;
    }
    free(tmp);
    ctx->str = s;
    ctx->label = STRING;
    return ctx;
}

context_string *skip_space(context_string *ctx) {
    char *iter = string_view(ctx);
    while (ctx->now_index != ctx->buf->length) {
        if (isspace(iter[0]) || iter[0] == '\n') {
            ctx->now_index += 1;
            ++iter;
        } else
            break;
    }
    return ctx;
}


Value *parseArray(Value *ctx, context_string *ctx_string) {
    // skip [
    ctx_string->now_index += 1;
    list *l = listCreate();
    while (1) {
        skip_space(ctx_string);
        Value *v = _parse(ctx_string);
        listAddNodeTail(l, v);
        skip_space(ctx_string);
        if (string_view(ctx_string)[0] == ']') {
            ctx_string->now_index += 1;
            break;
        }
        if (string_view(ctx_string)[0] != ',') {
            // json is not ','
            printf("%s\n", "json _parse error");
            printf("error line index = %zu\n", ctx_string->now_index);
            printf("%s\n", string_view(ctx_string));
            exit(1);
        }
        ctx_string->now_index += 1;
    }
    ctx->list = l;
    ctx->label = LIST;
    return ctx;
}


Value *parseDict(Value *ctx, context_string *ctx_string) {

    // skip {
    ctx_string->now_index += 1;
    // skip " " or "\n"
    Dict *d = makeDict();
    while (1) {
        //key
        skip_space(ctx_string);
        Value *key = parseString(ctx, ctx_string);
        sdshdr *this_key = key->str;
        skip_space(ctx_string);
        char *p = string_view(ctx_string);
        if (p[0] == ':')
            ctx_string->now_index += 1;
        else {
            printf("error type");
            exit(1);
        }
        skip_space(ctx_string);
        Value *value = _parse(ctx_string);
        skip_space(ctx_string);
        addKeyValue(d, this_key->buf, value);
        sdshdrRelease(this_key);
        char *str_view = string_view(ctx_string);

        if (str_view[0] == '}') {
            ctx_string->now_index += 1;
            break;
        }

        if (str_view[0] != ',') {
            printf("parse error");
            exit(1);
        }
        ctx_string->now_index += 1;
    }

    ctx->dict = d;
    ctx->label = DICT;
    return ctx;

}


Value *_parse(context_string *ctx_string) {
    Value *value = makeValue(NULL, VALUE);
    char ch = string_view(ctx_string)[0];
    switch (ch) {
        case '\"':
            value = parseString(value, ctx_string);
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            value = parseNumber(value, ctx_string);
            break;
        case '[':
            value = parseArray(value, ctx_string);
            break;
        case '{':
            value = parseDict(value, ctx_string);
            break;
        default:
            printf("known error type,please check!");
            exit(1);
    }

    return value;
}

Value *parse(sdshdr *str) {

    //持有语义，非占有语义

    context_string ctx_string = {
            .now_index =0,
            .buf =str
    };

    ctx_string.buf = str;
    ctx_string.now_index = 0;
    Value *data = _parse(&ctx_string);
    return data;

}


