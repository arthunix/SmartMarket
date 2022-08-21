#pragma once
#ifndef INDEX_H
#define INDEX_H

#include "rbtree.hh"
#include "Product.h"

static void structio_t_destroy(void* a);

static int structio_t_cmp(const void* a, const void* b);

static void structio_t_print(const void* a);

struct structio_t {
	long long int __offset;
	rrn_t __rrn;
};

class Index {
	rbtree *index_tree;
public:
	Index() {
        index_tree = rbtree_create(structio_t_cmp, structio_t_destroy, structio_t_print);
	}
    ~Index() {
        rbtree_destroy(index_tree);
    }
};

void structio_t_destroy(void* a)
{
    free((structio_t*)a);
}

int structio_t_cmp(const void* a, const void* b)
{

    if ((*(structio_t*)a).__rrn < (*(structio_t*)b).__rrn)
    {
        return -1;
    }
    else if ((*(structio_t*)a).__rrn > (*(structio_t*)b).__rrn)
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
