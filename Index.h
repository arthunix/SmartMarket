#pragma once
#ifndef INDEX_H
#define INDEX_H

#include <cstring>

#include "rbtree.h"
#include "Product.h"

// IMPORTANT DEFINITIONS FOR THE INDEX
// Defining basic structures
typedef unsigned long long int rrn_t;
struct structio_t {
    bool mAmIonInventory; /* If not I am on the shelf */
    char mName[NAME_SIZE]; /* I think we don't need to store the entire name */
    long long int mOffset; /* Where we should look for in an IO operation */
};
// Defining necessary functions
static void structio_t_destroy(void* a);
static int structio_t_cmp(const void* a, const void* b);
static void structio_t_print(const void* a);


class Index {
private:
    rbtree* pIndexTree;

    structio_t* safe_structio_malloc()
    {
        void* ptr = new structio_t;
        if (!ptr) {
            fprintf(stderr, "At [%s:%ul] unable to allocate (%ul bytes) of memory\n",
                __FILE__, __LINE__, (unsigned long)sizeof(structio_t));
            exit(EXIT_FAILURE);
        }
        return (structio_t*)ptr;
    }

    rbnode* FindIndexNode(const char* NameToLookup)
    {
        structio_t lStructIO;
        strcpy_s(lStructIO.mName, NAME_SIZE, NameToLookup);
        return rbtree_search(pIndexTree, &lStructIO);
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

    structio_t LookupAtIndex(const char* NameToLookup)
    {
        rbnode* NodeFound = FindIndexNode(NameToLookup);

        return *((structio_t*)((*NodeFound).data));
    }

    bool InsertToIndex(structio_t structio)
    {
        structio_t* pNewStructIO = safe_structio_malloc();
        *pNewStructIO = structio;
        return rbtree_insert(pIndexTree, pNewStructIO);
    }

    bool RemoveFromIndex(const char* NameToLookup)
    {
        rbnode* NodeFound = FindIndexNode(NameToLookup);

        if (NodeFound)
        {
            rbtree_delete(pIndexTree, NodeFound);
            return true;
        }
        else
        {
            return false;
        }
    }
};

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
    else if ( (strcmp((*(structio_t*)a).mName, (*(structio_t*)b).mName)) > 0)
    {
        return 1;
    }
    else // ( (strcmp((*(structio_t*)a).mName, (*(structio_t*)b).mName)) == 0)
    {
        return 0;
    }
}

void structio_t_print(const void* struct_io) {
    printf("Name    = %s / ", (*(structio_t*)struct_io).mName);
    printf("Offset  = %lli", (*(structio_t*)struct_io).mOffset);
    fflush(NULL);
}

#endif !INDEX_H
