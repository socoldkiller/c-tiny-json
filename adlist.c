
#include <stdlib.h>
#include "adlist.h"

const int AL_START_HEAD = 0;
const int AL_START_TAIL = 1;


/* Create a new list. The created list can be freed with
 * listRelease(), but private value of every node need to be freed
 * by the user before to call listRelease(), or by setting a freeNode method using
 * listSetFreeMethod.
 *
 * On error, NULL is returned. Otherwise the pointer to the new list. */
list *listCreate(void) {
    list *list;
    if ((list = malloc(sizeof(*list))) == NULL)
        return NULL;
    list->head = list->tail = NULL;
    list->len = 0;
    list->dup = NULL;
    list->freeNode = NULL;
    list->match = NULL;
    return list;
}

/* Remove all the elements from the list without destroying the list itself. */
void listEmpty(list *list) {
    unsigned long len;
    listNode *current, *next;
    current = list->head;
    len = list->len;
    while (len--) {
        next = current->next;
        if (list->freeNode) list->freeNode(current->value);
        free(current);
        current = next;
    }
    list->head = list->tail = NULL;
    list->len = 0;
}

/* freeNode the whole list.
 *
 * This function can't fail. */
void listRelease(list *list) {
    listEmpty(list);
    free(list);
}

/* Add a new node to the list, to head, containing the specified 'value'
 * pointer as value.
 *
 * On error, NULL is returned and no operation is performed (i.e. the
 * list remains unaltered).
 * On success the 'list' pointer you pass to the function is returned. */
list *listAddNodeHead(list *list, void *value) {
    listNode *node;

    if ((node = malloc(sizeof(*node))) == NULL)
        return NULL;
    node->value = value;
    if (list->len == 0) {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    } else {
        node->prev = NULL;
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }
    list->len++;
    return list;
}

/* Add a new node to the list, to tail, containing the specified 'value'
 * pointer as value.
 *
 * On error, NULL is returned and no operation is performed (i.e. the
 * list remains unaltered).
 * On success the 'list' pointer you pass to the function is returned. */
list *listAddNodeTail(list *list, void *value) {
    listNode *node;

    if ((node = malloc(sizeof(*node))) == NULL)
        return NULL;
    node->value = value;
    if (list->len == 0) {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    } else {
        node->prev = list->tail;
        node->next = NULL;
        list->tail->next = node;
        list->tail = node;
    }
    list->len++;
    return list;
}

list *listInsertNode(list *list, listNode *old_node, void *value, int after) {
    listNode *node;

    if ((node = malloc(sizeof(*node))) == NULL)
        return NULL;
    node->value = value;
    if (after) {
        node->prev = old_node;
        node->next = old_node->next;
        if (list->tail == old_node) {
            list->tail = node;
        }
    } else {
        node->next = old_node;
        node->prev = old_node->prev;
        if (list->head == old_node) {
            list->head = node;
        }
    }
    if (node->prev != NULL) {
        node->prev->next = node;
    }
    if (node->next != NULL) {
        node->next->prev = node;
    }
    list->len++;
    return list;
}

/* Remove the specified node from the specified list.
 * It's up to the caller to freeNode the private value of the node.
 *
 * This function can't fail. */
void listDelNode(list *list, listNode *node) {
    if (node->prev)
        node->prev->next = node->next;
    else
        list->head = node->next;
    if (node->next)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;
    if (list->freeNode) list->freeNode(node->value);
    free(node);
    list->len--;
}

/* Returns a list iterator 'iter'. After the initialization every
 * call to listNext() will return the next element of the list.
 *
 * This function can't fail. */
listIter *listGetIterator(list *list, int direction) {
    listIter *iter;

    if ((iter = malloc(sizeof(*iter))) == NULL) return NULL;
    if (direction == AL_START_HEAD)
        iter->next = list->head;
    else
        iter->next = list->tail;
    iter->direction = direction;
    return iter;
}

/* Release the iterator memory */
void listReleaseIterator(listIter *iter) {
    free(iter);
}

/* Create an iterator in the list private iterator structure */
void listRewind(list *list, listIter *li) {
    li->next = list->head;
    li->direction = AL_START_HEAD;
}

void listRewindTail(list *list, listIter *li) {
    li->next = list->tail;
    li->direction = AL_START_TAIL;
}

/* Return the next element of an iterator.
 * It's valid to remove the currently returned element using
 * listDelNode(), but not to remove other elements.
 *
 * The function returns a pointer to the next element of the list,
 * or NULL if there are no more elements, so the classical usage
 * pattern is:
 *
 * iter = listGetIterator(list,<direction>);
 * while ((node = listNext(iter)) != NULL) {
 *     doSomethingWith(listNodeValue(node));
 * }
 *
 * */
listNode *listNext(listIter *iter) {
    listNode *current = iter->next;

    if (current != NULL) {
        if (iter->direction == AL_START_HEAD)
            iter->next = current->next;
        else
            iter->next = current->prev;
    }
    return current;
}


/* Duplicate the whole list. On out of memory NULL is returned.
 * On success a copy of the original list is returned.
 *
 * The 'Dup' method set with listSetDupMethod() function is used
 * to copy the node value. Otherwise the same pointer value of
 * the original node is used as value of the copied node.
 *
 * The original list both on success or error is never modified. */
list *listDup(list *orig) {
    list *copy;
    listIter iter;
    listNode *node;

    if ((copy = listCreate()) == NULL)
        return NULL;
    copy->dup = orig->dup;
    copy->freeNode = orig->freeNode;
    copy->match = orig->match;
    listRewind(orig, &iter);
    while ((node = listNext(&iter)) != NULL) {
        void *value;

        if (copy->dup) {
            value = copy->dup(node->value);
            if (value == NULL) {
                listRelease(copy);
                return NULL;
            }
        } else {
            value = node->value;
        }

        if (listAddNodeTail(copy, value) == NULL) {
            /* freeNode value if duplist succeed but listAddNodeTail failed. */
            if (copy->freeNode) copy->freeNode(value);

            listRelease(copy);
            return NULL;
        }
    }
    return copy;
}

/* Search the list for a node matching a given key.
 * The matchlist is performed using the 'matchlist' method
 * set with listSetMatchMethod(). If no 'matchlist' method
 * is set, the 'value' pointer of every node is directly
 * compared with the 'key' pointer.
 *
 * On success the first matching node pointer is returned
 * (search starts from head). If no matching node exists
 * NULL is returned. */
listNode *listSearchKey(list *list, void *key) {
    listIter iter;
    listNode *node;

    listRewind(list, &iter);
    while ((node = listNext(&iter)) != NULL) {
        if (list->match) {
            if (list->match(node->value, key)) {
                return node;
            }
        } else {
            if (key == node->value) {
                return node;
            }
        }
    }
    return NULL;
}

/* Return the element at the specified zero-based index
 * where 0 is the head, 1 is the element next to head
 * and so on. Negative integers are used in order to count
 * from the tail, -1 is the last element, -2 the penultimate
 * and so on. If the index is out of range NULL is returned. */
listNode *listIndex(list *list, long index) {
    listNode *n;

    if (index < 0) {
        index = (-index) - 1;
        n = list->tail;
        while (index-- && n) n = n->prev;
    } else {
        n = list->head;
        while (index-- && n) n = n->next;
    }
    return n;
}

/* Rotate the list removing the tail node and inserting it to the head. */
void listRotateTailToHead(list *list) {
    if (listLength(list) <= 1) return;

    /* Detach current tail */
    listNode *tail = list->tail;
    list->tail = tail->prev;
    list->tail->next = NULL;
    /* Move it as head */
    list->head->prev = tail;
    tail->prev = NULL;
    tail->next = list->head;
    list->head = tail;
}

/* Rotate the list removing the head node and inserting it to the tail. */
void listRotateHeadToTail(list *list) {
    if (listLength(list) <= 1) return;

    listNode *head = list->head;
    /* Detach current head */
    list->head = head->next;
    list->head->prev = NULL;
    /* Move it as tail */
    list->tail->next = head;
    head->next = NULL;
    head->prev = list->tail;
    list->tail = head;
}

/* Add all the elements of the list 'o' at the end of the
 * list 'l'. The list 'other' remains empty but otherwise valid. */
void listJoin(list *l, list *o) {
    if (o->len == 0) return;

    o->head->prev = l->tail;

    if (l->tail)
        l->tail->next = o->head;
    else
        l->head = o->head;

    l->tail = o->tail;
    l->len += o->len;

    /* Setup other as an empty list. */
    o->head = o->tail = NULL;
    o->len = 0;
}


listIterDistance *listGetIteratorDistance(const list *l, int direction, int distance) {
    listIterDistance *dis;
    if ((dis = malloc(sizeof(*dis))) == NULL) {
        return NULL;
    }
    dis->distance = distance;
    dis->iter = listGetIterator(l, direction);
    return dis;
}


listNode *_listDistanceNext(listIterDistance *iter) {
    int distance = iter->distance;
    while (distance--) {
        listNext(iter->iter);
    }
    return listNext(iter->iter);
}

listNode *listDistanceNext(listIterDistance *iter) {
    static size_t used = 0;
    if (!used) {
        used = 1;
        return listNext(iter->iter);

    }
    listNode *node = _listDistanceNext(iter);
    if (!node)
        used = 0;
    return node;
}


void listReleaseDistance(listIterDistance *iter) {
    listReleaseIterator(iter->iter);
    free(iter);
}


void listNodeMap(list *l, ListNodeCallBack callback) {
    listIterDistance *head = listGetIteratorDistance(l, AL_START_HEAD, 0);
    listNode *node;
    int index = 0;
    while ((node = listDistanceNext(head)) != NULL) {
        callback(index, node);
        ++index;
    }
    listReleaseDistance(head);
}


int findlistNode(const list *l, void *p, int(cmp)(listNode *n, void *p)) {
    listIterDistance *head = listGetIteratorDistance(l, AL_START_HEAD, 0);
    listNode *node;
    int _bool = 0;
    while ((node = listDistanceNext(head)) != NULL) {
        _bool = cmp(node, p);
        if (_bool) {
            break;
        }
    }
    listReleaseDistance(head);
    return _bool;
}

sdshdr *listToString(list *l, sdshdr *callback(list *l, sdshdr *ctx, void *value), sdshdr *ctxStr) {
    listIter iter;
    listNode *node;
    sdsJoinchar(ctxStr, "[");
    listRewind(l, &iter);
    while ((node = listNext(&iter)) != NULL) {
        callback(l, ctxStr, node->value);
        if (!node->next)
            break;
        sdsJoinchar(ctxStr, ",");


    }

    size_t len = ctxStr->length;
    ctxStr->buf[len] = 0;
    sdsJoinchar(ctxStr, "]");
    return ctxStr;
}