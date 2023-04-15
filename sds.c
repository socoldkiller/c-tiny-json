

#include "sds.h"

size_t _sdsmax(size_t a, size_t b) {
    return a > b ? a : b;
}


size_t sdslen(const sdshdr *s) {
    return s->length;
}

// const char * to sdshdr
sdshdr *makeSdsHdr(const char *s) {
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


sdshdr *sdsJoinchar(sdshdr *s, const char *str) {
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
        new_alloc = _sdsmax(new_alloc << 1, s->length + len) | 1;
        char *p = NULL;
        p = realloc(s->buf, new_alloc);
        if (!p) {
            assert("realloc out of memory");
            exit(1);
        }
        s->buf = p;

    }

    //  s->buf = strcat(s->buf, pStr ? pStr : str);
    memcpy(s->buf + s->length, pStr ? pStr : str, strlen(str));
    s->alloc_length = new_alloc;
    // s->length = strlen(s->buf);
    s->length += len;
    free(pStr);
    return s;
}


void sdshdrPrint(sdshdr *s) {
    const char *format = "{\"length\":%zu,\"size\":%zu,\"buf\":\"%s\"}";
    if (!s) {
        printf(format, 0, 0, " ");
        return;
    }
    printf(format, s->length, s->alloc_length, s->buf);
    printf("\n");
}

sdshdr *sdshdrClear(sdshdr *s) {
    //default  empty this function will free memroy
    if (!s) {
        return s;
    }
    free(s->buf);
    s->buf = NULL;
    s->length = 0;
    s->alloc_length = 0;
    return s;
}

sdshdr *sdshdrEmpty(sdshdr *s) {
    if (s == NULL) {
        return NULL;
    }
    memset(s->buf, 0, sizeof(s->buf) * s->alloc_length);
    s->length = 0;
    return s;
}


void sdshdrRelease(void *s) {
    sdshdrClear(s);
    free(s);
}






