#include "../define/constants.h"
#include <cstdint>


struct HeadInfo{
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
	int ival;
	float fval;
	char strval[ATTR_SIZE];
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
	virtual int getEntry(void *ptr, int indexNum) = 0; //To be updated in documentation
	virtual int setEntry(void *ptr, int indexNum) = 0; //To be updated in documentation
};

class IndInternal : public IndBuffer{
public:
	IndInternal();//update in documentation
	IndInternal(int blk_no);
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

