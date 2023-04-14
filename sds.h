#pragma  once

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

typedef char *sds;

typedef struct sdshdr {
    size_t alloc_length;
    size_t length;
    char *buf;
} sdshdr;

size_t _sdsmax(size_t a, size_t b);

size_t sdslen(const sdshdr *s);

sdshdr *makeSdsHdr(const char *s);

sdshdr *sdsJoinchar(sdshdr *s, const char *str);

void sdshdrPrint(sdshdr *s);

sdshdr *sdshdrClear(sdshdr *s);

sdshdr *sdshdrEmpty(sdshdr *s);


void sdshdrRelease(void *s);

