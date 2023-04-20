#pragma once

#include <ctype.h>
#include "value.h"
#include <assert.h>
#include <ctype.h>
#include "err.h"


typedef struct string_view string_view;

Value *_parse(string_view *ctx_string);

struct string_view *skip_space(string_view *ctx);


char *forword_str_next(string_view *ctx) {
    skip_space(ctx);
    return str_next(ctx);
}

Value *parseNumber(Value *ctx, string_view *ctx_string) {
    char *str_view = str_next(ctx_string);
    char *endpoint = NULL;
    double num = strtod(str_view, &endpoint);
    if (num - (int) num) {
        double *heap = malloc(sizeof(double));
        *heap = num;
        ctx->doubleNumber = heap;
        ctx->label = DOUBLE;

    } else {
        int *heap = malloc(sizeof(int));
        *heap = (int) num;
        ctx->number = heap;
        ctx->label = INT;
    }

    ctx_string->now_index += (endpoint - str_view);
    return ctx;
}

Value *parseNull(Value *ctx, string_view *ctx_string) {
    char *nullStr = str_next(ctx_string);
    if (nullStr[1] == 'u' && nullStr[2] == 'l' && nullStr[3] == 'l') {
        ctx->label = _NULL;
    } else {
        print_parse_error_info(ctx_string);
        return NULL;
    }
    ctx_string->now_index += 4;
    return ctx;
}

Value *parseTrue(Value *ctx, string_view *ctx_string) {
    char *TrueStr = str_next(ctx_string);
    if (TrueStr[1] == 'r' && TrueStr[2] == 'u' && TrueStr[3] == 'e') {
        ctx->label = True;
    } else {
        print_parse_error_info(ctx_string);
        return NULL;
    }
    ctx_string->now_index += 4;
    return ctx;
}

Value *parseFalse(Value *ctx, string_view *ctx_string) {
    char *FalseStr = str_next(ctx_string);
    if (FalseStr[1] == 'a' && FalseStr[2] == 'l' && FalseStr[3] == 's' && FalseStr[4] == 'e') {
        ctx->label = False;
    } else {
        print_parse_error_info(ctx_string);
        return NULL;
    }
    ctx_string->now_index += 5;
    return ctx;
}

Value *parseString(Value *ctx, string_view *ctx_string) {
    // skip "
    if (str_next(ctx_string)[0] != '\"') {
        print_parse_error_info(ctx_string);
        return NULL;
    }

    // skip '\"'

    ctx_string->now_index += 1;
    sdshdr *s = makeSdsHdr("\"");
    char *start = str_next(ctx_string);
    char *tmp = malloc(1);
    memset(tmp, 0, 1);
    for (size_t i = 0;; i++) {
        ctx_string->now_index += 1;
        char ch = start[i];
        tmp[0] = ch;
        sdsJoinchar(s, tmp);
        if (ch == '\"') {
            if (start[i - 1] != '\\')
                break;
        }
    }
    free(tmp);
    ctx->str = s;
    ctx->label = STRING;
    return ctx;
}

string_view *skip_space(string_view *ctx) {
    char *iter = str_next(ctx);

    while (ctx->now_index < ctx->buf->length) {
        if (isspace(iter[0]) || iter[0] == '\n') {
            ctx->now_index += 1;
            ++iter;
        } else
            break;
    }
    return ctx;
}

Value *parseArray(Value *ctx, string_view *ctx_string) {
    // skip [
    ctx_string->now_index += 1;
    list *l = listCreate();
    while (1) {
        skip_space(ctx_string);
        if (str_next(ctx_string)[0] == ']') {
            ctx_string->now_index += 1;
            break;
        }

        Value *v = _parse(ctx_string);
        if (!v) {
            return NULL;
        }
        listAddNodeTail(l, v);
        skip_space(ctx_string);
        if (str_next(ctx_string)[0] == ']') {
            ctx_string->now_index += 1;
            break;
        }
        // json is not ','
        if (str_next(ctx_string)[0] == ',' || str_next(ctx_string)[0] == ']')
            ++ctx_string->now_index;
        else {
            print_parse_error_info(ctx_string);
            return NULL;
        }

    }
    ctx->list = l;
    ctx->label = LIST;
    return ctx;
}

Value *parseDict(Value *ctx, string_view *ctx_string) {
    // skip {
    ctx_string->now_index += 1;

    Dict *d = makeDict();
    while (1) {
        //key
        if (forword_str_next(ctx_string)[0] == '}') {
            ctx_string->now_index += 1;
            break;
        }
        // skip " " or "\n"
        skip_space(ctx_string);
        Value *key = parseString(ctx, ctx_string);

        if (!key)
            return NULL;

        sdshdr *this_key = key->str;
        skip_space(ctx_string);
        char *p = str_next(ctx_string);
        if (p[0] == ':')
            ++ctx_string->now_index;
        else {
            print_parse_error_info(ctx_string);
            return NULL;
        }
        skip_space(ctx_string);
        Value *value = _parse(ctx_string);
        skip_space(ctx_string);
        addKeyValue(d, this_key->buf, value);
        sdshdrRelease(this_key);
        char *str_view = str_next(ctx_string);

        if (str_view[0] == '}') {
            ++ctx_string->now_index;
            break;
        }


        if (str_view[0] != ',') {
            print_parse_error_info(ctx_string);
            exit(1);
        }
        ++ctx_string->now_index;
    }

    ctx->dict = d;
    ctx->label = DICT;
    return ctx;

}

Value *_parse(string_view *ctx_string) {
    skip_space(ctx_string);
    Value *value = makeValue(NULL, _NULL);
    if (!value)
        return NULL;
    char ch = str_next(ctx_string)[0];
    switch (ch) {
        case '\"':
            value = parseString(value, ctx_string);
            break;
        case '-':
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
        case 'n':
            value = parseNull(value, ctx_string);
            break;
        case 't':
            value = parseTrue(value, ctx_string);
            break;
        case 'f':
            value = parseFalse(value, ctx_string);
            break;
        case 0:
            break;
        default:
            print_parse_error_info(ctx_string);
            return NULL;
    }
    return value;
}

Value *parse(sdshdr *str) {
    //持有语义，非占有语义
    string_view ctx_string = {
            .buf =str,
    };

    err error = {
            .line =0,
            .column = 0,
            .stringView = &ctx_string
    };

    ctx_string.now_index = 0;
    ctx_string.err_info = &error;
    Value *data = _parse(&ctx_string);
    return data;
}


