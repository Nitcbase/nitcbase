#include "../define/constants.h"
#include "../define/errors.h"
#include <cstdint>
                                                                                                              
typedef int relId;
char OpenRelTable[MAXOPEN][16]={"RELATIONCAT","ATTRIBUTECAT","NULL","NULL","NULL","NULL","NULL","NULL","NULL","NULL","NULL","NULL"};
struct recBlock {                                                                                                                                                                                                  
	int32_t blockType;
	int32_t pblock;
	int32_t lblock;
	int32_t rblock;
	int32_t numEntries;
	int32_t numAttrs; 
	int32_t numSlots;
	unsigned char reserved[4];
	unsigned char slotMap_Records[BLOCK_SIZE-104];
    unsigned char unused[72];	
};


struct A
{
	long long int a;
	char b[16];
          float c;
};
	

struct internalIndexBlock {
	int32_t blockType;
	int32_t pblock;
	int32_t lblock;
	int32_t rblock;
	int32_t numEntries;
	int32_t numAttrs; 
	int32_t numSlots;
	unsigned char reserved[4];
	struct internalIndex{
    unsigned char childPtr[4];
    unsigned char attrVal[16];
    }intIndex[100];
	unsigned char childPtr100[4];
	unsigned char unused[11];
};

struct leafIndexBlock {
	int32_t blockType;
	int32_t pblock;
	int32_t lblock;
	int32_t rblock;
	int32_t numEntries;
	int32_t numAttrs; 
	int32_t numSlots;
	unsigned char reserved[4];
	struct internalLeaf{
    unsigned char attrVal[16];
    unsigned char blockNum[4];
    unsigned char slotNum[4];
    unsigned char unused[8];
    }intIndex[63];	
};


struct HeadInfo
{
	int32_t blockType;
	int32_t pblock;
	int32_t lblock;
	int32_t rblock;
	int32_t numEntries;
	int32_t numAttrs; 
	int32_t numSlots;
	unsigned char reserved[4];
};

typedef union Attribute{
          long long int ival;
	float fval;
	char sval[ATTR_SIZE];
}Attribute;

struct InternalEntry{
	int32_t lChild;
	union Attribute attrVal;
	int32_t rChild;
};

struct Index{
	union Attribute attrVal;
	int32_t block;
	int32_t slot;
	unsigned char unused[8];
};

typedef struct recId{
	int block;
	int slot;
} recId;

typedef struct RelCatEntry {
    char rel_name[ATTR_SIZE];
    int num_attr;
    int num_rec;
    int first_blk; 
    int last_blk;         //Head of linked list of blocks
    int num_slots_blk;      //Number of slots in a block
} RelCatEntry ;

typedef struct AttrCatEntry {
        char rel_name[ATTR_SIZE];
        char attr_name[ATTR_SIZE];
        int attr_type;     // can be INT/FLOAT/STR
        bool primary_flag; // Currently unused
        int root_block;    // root block# of the B+ tree if exists, else -1
        int offset;        // offset of the attribute in the relation
    } AttrCatEntry ;

class BlockBuffer{
protected:
	//field
	int blockNum;
	
	//methods
	unsigned char *getBufferPtr();
	int getBlock();
	int getFreeBlock(int BlockType);

public:
	//methods
	BlockBuffer(int blockNum);
	BlockBuffer(char blockType);
	int getBlockNum();
	void getHeader(struct HeadInfo* head);
	void setHeader(struct HeadInfo* head);
	int getBlockType(int bufferIndex);
	void setBlockType(int blockType);
};

class RecBuffer : public BlockBuffer{
public:

	//methods
	RecBuffer();
	RecBuffer(int blockNum);
	void getSlotmap(unsigned char *slotMap);
	void setSlotmap(unsigned char *slotMap);
	int getRecord(union Attribute *rec,int slotNum);
	int setRecord(union Attribute *rec,int slotNum);
};

class IndBuffer : public BlockBuffer{
public:
	IndBuffer(int blockNum);
	IndBuffer(char blockType);
	virtual int getEntry(void *ptr, int indexNum) = 0; 
	virtual int setEntry(void *ptr, int indexNum) = 0; 
};

class IndInternal : public IndBuffer{
public:
	IndInternal();//update in documentation
	IndInternal(int blockNum);
	int getEntry(void *ptr, int indexNum) ;
	int setEntry(void *ptr, int indexNum) ;
};

class IndLeaf : public IndBuffer{
public:
	IndLeaf();
	IndLeaf(int blockNum);
	int getEntry(void *ptr, int indexNum) ;
	int setEntry(void *ptr, int indexNum) ;
};

/* TO SEE WHAT THIS IS
class BlockBuffer{

public:
	
	// Making destructor virtual to allow dynamic_cast.
	// Refer to following link for more info:
	// https://stackoverflow.com/questions/15114093/getting-source-type-is-not-polymorphic-when-trying-to-use-dynamic-cast
	//https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors  also useful
	virtual ~BlockBuffer();
};*/


