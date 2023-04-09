#pragma  once

#include <string.h>
#include <stdlib.h>

typedef char *sds;


typedef struct sdshdr {
    size_t alloc_length;
    size_t length;
    char *buf;
} sdshdr;


static size_t sdslen(const sdshdr *s) {
    return s->length;
}

// const char * to sdshdr
static sdshdr *strTosdshdr(const char *s) {
    char *p;
    sdshdr *sds;

    if ((sds = malloc(sizeof(*sds))) == NULL) {
        return NULL;
    }

    size_t len = strlen(s);
    if ((p = malloc(sizeof(*p) * (len + 1))) == NULL) {
        return NULL;
    }

    memcpy(p, s, len + 1);
    sds->buf = p;
    sds->length = len;
    sds->alloc_length = len + 1;

    return sds;

}

// free sdshdr
void sdshdrRelease(sdshdr *s) {
    free(s->buf);
    free(s);
}


size_t sdsmax(int a, int b) {
    return a > b ? a : b;
}


sdshdr *sdsJoinsds(sdshdr *s, const char *str) {
    if (!s) {
        return NULL;
    }
    size_t len = strlen(str);

    char *pStr = NULL;
    if (s->buf == str) {
        if ((pStr = malloc(sizeof(*pStr) * (len + 1))) == NULL) {
            return NULL;
        }
        memcpy(pStr, str, len + 1);
    }

    size_t new_alloc = s->alloc_length;
    if (s->length + len >= s->alloc_length) {
        new_alloc = sdsmax(new_alloc << 1, s->length + len) | 1;
        s->buf = realloc(s->buf, new_alloc);
    }

    s->buf = strcat(s->buf, pStr ? pStr : str);
    s->alloc_length = new_alloc;
    s->length = strlen(s->buf);
    free(pStr);
    return s;
}


void sdshdrPrint(sdshdr *s) {
    const char *format = "{\"length\":%zu,\"size\":%zu,\"buf\":\"%s\"}";
    printf(format, s->length, s->alloc_length, s->buf);
    printf("\n");
}






