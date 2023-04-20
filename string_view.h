#include "sds.h"


typedef struct err err;

typedef struct string_view {
    sdshdr *buf;
    size_t now_index;
    err *err_info;
} string_view;


char *str_next(string_view *ctx) {
    return ctx->buf->buf + ctx->now_index;
}

