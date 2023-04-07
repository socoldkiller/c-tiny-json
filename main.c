#include <stdio.h>
#include <stdlib.h>
#include "adlist.h"
#include <string.h>

struct context;
typedef struct context context;

typedef void(callback)(void);

typedef void(ctxCall)(struct context *);

struct context {
    struct context *ctx;
    int index;
    const char *fun_name;
    callback *call;
    char *info;
    void *anycall;
    void *data;
};


void syscall(void) {
    printf("call name");
}

context *make_context() {
    context *ctx = malloc(sizeof(context));
    ctx->ctx = NULL;
    ctx->index = -1;
    ctx->fun_name = "";
    ctx->call = NULL;
    ctx->anycall = NULL;
    ctx->info = "";
    ctx->data = NULL;
    return ctx;
}

void *free_context(context *ctx) {
    //  freeNode(ctx);
    ctx = NULL;
    return ctx;
}


void ctx_call(void *ctx) {
    context *c = ctx;
    int index = c->index;
    printf("index = %d\n", index);

}


int comp1(const void *ctx1, const void *ctx2) {
    const context *ca = *(context **) ctx1;
    const context *cb = *(context **) ctx2;
    return ca->index - cb->index;
}

typedef int(Compare)(const void *, const void *);


struct Reply {
    char *reply;
    int id;
};


void test() {
    context *cc[10];
    srand(1000);


    for (size_t i = 0; i < 10; i++) {
        cc[i] = make_context();
        cc[i]->index = rand() % 100;
        cc[i]->anycall = ctx_call;

    }

    for (size_t i = 0; i < 10; i++) {
        if (i + 1 != 10)
            cc[i]->ctx = cc[i + 1];
    }


    for (context *ctx = cc[0]; ctx != NULL; ctx = ctx->ctx) {
        ctxCall *call = ctx->anycall;
        call(ctx);
    }


    for (size_t i = 0; i < 10; i++) {
        free_context(cc[i]);
    }

}

typedef listNode *(listNodeCallback)(int, listNode *);


listNode *randListNode(int index, listNode *node) {
    int *num = malloc(sizeof(int));
    *num = rand() % 100;
    node->value = num;
    return NULL;
}


typedef struct Data {
    int val;
    int ref_count;
} Data;

Data makeData(int val) {
    Data d;
    d.val = val;
    d.ref_count = 0;
    return d;
}

Data copyData(Data *d) {
    d->ref_count += 1;
    return *d;
}

void freeData(void *d) {
    free(d);
}


void showrandListNode(size_t index, listNode *node) {
    Data *num = node->value;
    printf("index = %zu,num = %d\n", index, num->val);
}


void intFree(void *num) {
    free(num);
}


int main() {

    srand(1000);
    list *l = listCreate();
    l->freeNode = freeData;


    for (int i = 0; i < 10; i++) {
        int *p = malloc(sizeof(int));
        scanf("%d", p);
        listAddNodeTail(l, p);
    }

    listNodeMap(l, showrandListNode);
    listRelease(l);
}


















