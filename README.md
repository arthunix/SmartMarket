# SmartMarket
This is the code I work while doing Organization and Information Recovery course at @UFSCar in 2022.1

## To clone and work with this project
Clone the repository:
```sh
git clone --recurse-submodules git@github.com:arthunix/SmartMarket.git
# or if you are not using ssh for autentication
# git clone --recurse-submodules https://github.com/arthunix/SmartMarket.git
```
## To enjoy  this project
Make sure you have c++ runtimes.
https://docs.microsoft.com/en-gb/cpp/windows/latest-supported-vc-redist?view=msvc-170 \
Just download a release and use for your enjoy

# Program infrastructure
Released under a permissive BSD 2-Clause "Simplified" License

1. Which external libraries were used on the project:
### [Dear ImGui: Bloat-free Graphical User interface for C++ with minimal dependencies](https://github.com/ocornut/imgui)
### [Libcutils : Some util C code library I made for personal and anyone use](https://github.com/arthunix/libcutils)

2. How to project was developed
- First we define how the software would be organized
- After we implement all the data strutured needed how a b-tree and more
- After we implement all the abstract data types like IO, Product, Market and more
- We implement the User Interface



3. Basic Abstract Data Structures were used
Some structures used to carry data

```C
// The node of the tree used as index
struct structio_t {
    long int mOffset;
    time_t mManufacturingDate;
    unsigned int mLote;
    char mName[NAME_SIZE];
    bool mAmIonInventory;
};

// Data structures used to store the data in the file
struct product_controller_t {
    bool is_occuped = false;
    bool is_deleted = false;
    long long int offset = 0;
};

struct product_storage_t {
    bool is_deleted = false;
    unsigned short int whereshelf;
    unsigned short int wheresection;
    Product product_itself;
};

struct FileHeader {
    long long int OffsetToMtf = 0;
    long long int OffsetToShelfBlock = 0;
    long long int OffsetToShelfBlockNextFree = 0;
    long long int OffsetToInventoryBlock = 0;
    long long int OffsetToInventoryBlockNextFree = 0;
    int NumberOfSections = 0;
    int NumberOfShelfsOnSection = 0;
    int NumberOfProductsOnShelf = 0;
};
```

```C
// Responsible to store and manipulate a basic data type product from market
class Product {
private:
	char mName[NAME_SIZE];
	char mDescription[DESCRIPTION_SIZE];
	char mBrand[BRAND_SIZE];
	time_t mExpirationDate;
	time_t mManufacturingDate;
	time_t mAdditionDate;
	time_t mLastModificationDate;
	double mPrice;
	unsigned int mLote;
    ...
}

// Responsible to deal with all IO operations
#define FILENAME "SMARTMARKET"

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
class IO {
    FILE* mIoFileStream;
    FileHeader mFileHeader;
    product_controller_t mMtf[TNumOfSections][TNumOfShelfsOnSection][TNumOfProductsOnShelf];
    ...
}
```

### INDEX CLASS

```C
static void structio_t_destroy(void* a);
static int structio_t_cmp(const void* a, const void* b);
static void structio_t_print(const void* a);

// Responsible to instantiate and to deal with the red black tree used on index
class Index {
private:
    rbtree* pIndexTree;

    structio_t* safe_structio_malloc();

    void _LookupAtIndexVec(const rbnode* x, const void* data, std::vector<rbnode*> &found)
    {
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
    Index() {
        pIndexTree = rbtree_create(structio_t_cmp, structio_t_destroy, structio_t_print);
    }

    ~Index() {
        rbtree_destroy(pIndexTree);
    }

    void LookupAtIndexVec( const char* NameToLookup, std::vector<rbnode*> &ret);

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
```

### MARKET CLASS
```C
// Instantiate the IO system and Index system to a class will deal with all
template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
class Market {
private:
	IO<TNumOfSections,TNumOfShelfsOnSection,TNumOfProductsOnShelf> mIOfs;
	Index mIndex;
	std::vector<rbnode*> mSearchCointainer;
    std::vector<rbnode*> &LookupForAProducts(const char* name)
    ...
}


template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
void Market<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::
insertProduct(Product toInsert) {
	structio_t lIOStructure;
	lIOStructure.mAmIonInventory = false;
	strcpy_s(lIOStructure.mName, NAME_SIZE, toInsert.getName());
	lIOStructure.mOffset = mIOfs.InsertToShelf(toInsert);
	lIOStructure.mLote = toInsert.getLote();
	lIOStructure.mManufacturingDate = toInsert.getManufacturingDate();
	mIndex.InsertToIndex(lIOStructure);
}

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
void Market<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::
removeProduct(void* whereIsToRemove, int index) {
	rbnode* *str_io = (rbnode**)whereIsToRemove;
	rbnode& crr_str_io = *str_io[index];
	structio_t sio = *(structio_t*)crr_str_io.data;
	mIndex.RemoveFromIndex(str_io[index]);
	mIOfs.RemoveFromShelf(sio.mOffset);
}

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
void Market<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::
changeProduct(void* whereIsToChange, int index, Product productToChange) {
	removeProduct(whereIsToChange, index);
	insertProduct(productToChange);
}

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
Product Market<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::getProduct(structio_t whereIs) {
	mIOfs.SeekOnShelf(whereIs.mOffset);
}
```

# Link with discipline
1. Advanced file manipulation
2. Index using binary tree
3. Access and file maintenance