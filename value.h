
#include "sds.h"
#include "adlist.h"

typedef size_t Label;


enum {
    INT = 1,
    FLOAT,
    DOUBLE,
    LIST,
    VALUE,
};


typedef struct Value {

    union {
        sdshdr *str;
        int *number;
        double *doubleNumber;
        list *list;
        void *data;
    };
    Label label;

} Value;


Value *makeValue(void *data, Label label) {

    Value *value;

    if ((value = malloc(sizeof(*value))) == NULL) {
        return NULL;
    }

    value->data = data;
    value->label = label;
}


sdshdr *ValueToString(Value *v) {
    if (!v) {
        return NULL;
    }
    sdshdr *toStr = makeSdsHdr("");
    switch (v->label) {
        case INT:
        case FLOAT:
        case DOUBLE:
        case LIST:
        case VALUE:
        default:
            break;
    }
    return toStr;
}