#pragma once
#ifndef INDEX_H
#define INDEX_H

#include <cstring>
#include <vector>
#include "rbtree.h"
#include "Product.h"

struct structio_t {
    bool mAmIonInventory;
    char mName[NAME_SIZE];
    unsigned int mLote;
    time_t mManufacturingDate;
    long long int mOffset;
};
static void structio_t_destroy(void* a);
static int structio_t_cmp(const void* a, const void* b);
static void structio_t_print(const void* a);

class Index {
private:
    rbtree* pIndexTree;

    structio_t* safe_structio_malloc()
    {
        void* ptr = new structio_t;
        if (!ptr)
        {
            fprintf(stderr, "At [%s:%ul] unable to allocate (%ul bytes) of memory\n",
                __FILE__, __LINE__, (unsigned long)sizeof(structio_t));
            exit(EXIT_FAILURE);
        }
        return (structio_t*)ptr;
    }

    /* This is so gross I'm about to vomit on the screen but don't you dare say it's bad I just
    took the guts out of the body so I can analyze the evidence instead of doing an MRI this 
    should be an ORACLE closed box, but we're not at the best time */
    void _LookupAtIndexVec(const rbnode* x, const void* data, std::vector<rbnode*> &found) {

        if (x != pIndexTree->nil)
        {
            if (pIndexTree->cmpfun(x->data, data) == 0)
            {
                structio_t structData = *(structio_t*)x->data;
                found.push_back((rbnode*)x);
            }
            _LookupAtIndexVec(x->left, data, found);
            _LookupAtIndexVec(x->right, data, found);
        }
    }

public:
    Index()
    {
        pIndexTree = rbtree_create(structio_t_cmp, structio_t_destroy, structio_t_print);
    }

    ~Index()
    {
        rbtree_destroy(pIndexTree);
    }

    void LookupAtIndexVec( const char* NameToLookup, std::vector<rbnode*> &ret)
    {
        structio_t lStructIO;
        strcpy_s(lStructIO.mName, NAME_SIZE, NameToLookup);
        _LookupAtIndexVec(pIndexTree->root->left, &lStructIO, ret);
    }

    void InsertToIndex(structio_t structio)
    {
        structio_t* pNewStructIO = safe_structio_malloc();
        *pNewStructIO = structio;
        rbtree_insert(pIndexTree, pNewStructIO);
    }

    void RemoveFromIndex(rbnode* NodeToRemove)
    {
        rbtree_delete(pIndexTree, NodeToRemove);
    }

    void PrintIndex()
    {
        rbtree_print_inorder(pIndexTree);
    }
};

bool VectorOfStructIOCharGetter(void* x, int index, const char** output)
{   
    rbnode* *str_io = (rbnode**)x;
    rbnode& crr_str_io = *str_io[index];
    structio_t sio = *(structio_t*)crr_str_io.data;

    *output = sio.mName;

    return true;
}

void structio_t_destroy(void* struct_io)
{
    free((structio_t*)struct_io);
}

int structio_t_cmp(const void* a, const void* b)
{
    if ( (strcmp((*(structio_t*)a).mName, (*(structio_t*)b).mName)) < 0 )
    {
        return -1;
    }
    else if ( (strcmp((*(structio_t*)a).mName, (*(structio_t*)b).mName)) > 0 )
    {
        return 1;
    }
    else // ( (strcmp((*(structio_t*)a).mName, (*(structio_t*)b).mName)) == 0 )
    {
        return 0;
    }
}

void structio_t_print(const void* struct_io) {
    printf("Name = %s / ", (*(structio_t*)struct_io).mName);
    printf("Offset = %lli / ", (*(structio_t*)struct_io).mOffset);
    printf("Lote = %ui", (*(structio_t*)struct_io).mLote);
    fflush(NULL);
}

#endif !INDEX_H
