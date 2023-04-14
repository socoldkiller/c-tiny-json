#pragma  once


#include "sds.h"

extern const int AL_START_HEAD;
extern const int AL_START_TAIL;


typedef struct list list;
typedef int bool;

typedef struct listNode {
    struct listNode *prev;
    struct listNode *next;
    void *value;
} listNode;


typedef struct listIter {
    listNode *next;
    int direction;
} listIter;


typedef struct listIterDistance {
    listIter *iter;
    int distance;
} listIterDistance;

listIterDistance *listGetIteratorDistance(const list *l, int direction, int distance);

listNode *listDistanceNext(listIterDistance *iter);

void listReleaseDistance(listIterDistance *iter);

typedef list *(duplist)(list *);

typedef void (freeNode)(void *);

typedef int (matchlist)(void *ptr, void *key);

typedef unsigned long ulong;
typedef struct list {
    listNode *head;
    listNode *tail;
    duplist *dup;
    freeNode *freeNode;
    matchlist *match;
    ulong len;
} list;

inline static ulong listLength(list *l) {
    return l->len;
}

inline static listNode *listFirst(list *l) {
    return l->head;
}

inline static listNode *listFast(list *l) {
    return l->tail;
}

inline static listNode *listPrevNode(listNode *n) {
    return n->prev;
}

inline static listNode *listNextNode(listNode *n) {
    return n->next;
}

inline static duplist *listGetDupMethod(list *l) {
    return l->dup;
}

inline static freeNode *listGetFreeMethod(list *l) {
    return l->freeNode;
}

inline static matchlist *listGetMatchMethod(list *l) {
    return l->match;
}


list *listCreate(void);

void listRelease(list *list);

void listEmpty(list *list);

list *listAddNodeHead(list *list, void *value);

list *listAddNodeTail(list *list, void *value);

list *listInsertNode(list *list, listNode *old_node, void *value, int after);

void listDelNode(list *list, listNode *node);

listIter *listGetIterator(list *list, int direction);

listNode *listNext(listIter *iter);

void listReleaseIterator(listIter *iter);

list *listDup(list *orig);

listNode *listSearchKey(list *list, void *key);

listNode *listIndex(list *list, long index);

void listRewind(list *list, listIter *li);

void listRewindTail(list *list, listIter *li);

void listRotateTailToHead(list *list);

void listRotateHeadToTail(list *list);

void listJoin(list *l, list *o);


typedef void (ListNodeCallBack)(size_t idx, listNode *node);


void listNodeMap(list *l, ListNodeCallBack callback);

int findlistNode(const list *l, void *p, int(cmp)(listNode *n, void *p));

sdshdr *listToString(list *l, sdshdr *callback(list *l, sdshdr *ctx, void *node));