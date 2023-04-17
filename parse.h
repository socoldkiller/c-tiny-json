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


Value *_parse(context_string *ctx_string);

Value *parseNumber(Value *ctx, context_string *ctx_string) {
    char *p = string_view(ctx_string);
    char *endpoint = NULL;
    double data = strtod(p, &endpoint);
    double *heap_data = NULL;
    if (!ctx->doubleNumber) {
        if ((ctx->doubleNumber = malloc(sizeof(double))) == NULL) {
            return NULL;
        }
    }
    if ((heap_data = malloc(sizeof(double))) == NULL) {
        return NULL;
    }
    *heap_data = data;
    ctx->doubleNumber = heap_data;
    ctx->label = DOUBLE;
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
            assert("json _parse error");
            exit(1);
        }
        ctx_string->now_index += 1;
    }
    ctx->list = l;
    ctx->label = LIST;
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
        default:
            exit(1);
    }

    return value;
}

Value *parse(sdshdr *str) {
    context_string *ctx_string = NULL;
    if ((ctx_string = malloc(sizeof(*ctx_string))) == NULL) {
        return 0;
    }
    ctx_string->buf = str;
    ctx_string->now_index = 0;
    Value *data = _parse(ctx_string);
    return data;

}


