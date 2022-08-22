#pragma once
#ifndef INDEX_H
#define INDEX_H

#include "rbtree.hh"
#include "Product.h"

static void structio_t_destroy(void* a);

static int structio_t_cmp(const void* a, const void* b);

static void structio_t_print(const void* a);

struct structio_t {
	long long int Offset;
    bool AmIonInventory; /* If not I am on the shelf */
	rrn_t Rrn;
};

class Index {
	rbtree *pIndexTree;
public:
	Index() {
        pIndexTree = rbtree_create(structio_t_cmp, structio_t_destroy, structio_t_print);
	}
    ~Index() {
        rbtree_destroy(pIndexTree);
    }
};

void structio_t_destroy(void* a)
{
    free((structio_t*)a);
}

int structio_t_cmp(const void* a, const void* b)
{

    if ((*(structio_t*)a).Rrn < (*(structio_t*)b).Rrn)
    {
        return -1;
    }
    else if ((*(structio_t*)a).Rrn > (*(structio_t*)b).Rrn)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void structio_t_print(const void* a) {
    printf("%lli", *(long long int*)a);
}

#endif !INDEX_H
