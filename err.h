
#include <stddef.h>
#include "string_view.h"

typedef struct string_view string_view;


typedef struct err {
    size_t line;
    size_t column;
    size_t *now_index;
    string_view *stringView;
} err;


size_t prev_column(err *error) {
    string_view *str = error->stringView;
    size_t len = str->now_index;
    while (len--) {
        if (str->buf->buf[len] == '\n')
            break;
    }
    error->column = str->now_index - len;
    return len;
}


size_t prev_line(err *error) {
    string_view *str = error->stringView;
    size_t len = str->now_index;
    size_t line = 1;
    while (len--) {
        if (str->buf->buf[len] == '\n')
            line++;
    }
    error->line = line;
    return len;
}


void print_parse_error_info(string_view *ctx_string) {
    prev_line(ctx_string->err_info);
    prev_column(ctx_string->err_info);
    printf("parse error: Invalid numeric literal at line %zu, column %zu", ctx_string->err_info->line,
           ctx_string->err_info->column);
}

