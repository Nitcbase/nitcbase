//#include <cstdint>
#include <bits/stdc++.h>
#include "staticBuffer.h"
#include "../define/constants.h"
#include "../define/errors.h"



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
	int getBlockType();
	void setBlockType(int blockType);
	
	//AppuAdded: for bplus tree destroy
	void releaseBlock();
};

class RecBuffer : public BlockBuffer{
public:

	//methods
	RecBuffer();
	RecBuffer(int blockNum);
	void getSlotMap(unsigned char *slotMap);
	void setSlotMap(unsigned char *slotMap);
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

/****************************************BLock Buffer**************************************************/

BlockBuffer::BlockBuffer(int blockNum){

//cout <<"Inside BlockBuffer(int blockNum) constructor\n";

// set the blockNum field of the object to input argument.
this->blockNum = blockNum;

// copy the block into buffer using getBlock function.
/***Note: disregard the return type***/
/***To be updated: make it camel case***/
getBlock();

//cout <<"Exiting BlockBuffer(int blockNum) constructor\n";
}

BlockBuffer::BlockBuffer(char blockType){
/***Note: disregard the return type***/

// allocate a block on the disc and a buffer in memory to hold the new block of given type using getFreeBlock function.

// set the blockNum field of the object to that of the allocated block number.
int type;
if(blockType == 'R')
	type = REC;
else if(blockType == 'I')
	type = IND_INTERNAL;
else
	type = IND_LEAF;
	
this->blockNum = getFreeBlock(type);
}

unsigned char * BlockBuffer::getBufferPtr(){
	//find the buffer index of the block using getBlock()
	
	//cout <<"Entering getBufferPtr\n"; 
	int bufferIndex = getBlock();
	
	//cout <<"Exiting getBufferPtr "<<bufferIndex<<"\n"; 
	// return the pointer to this buffer (blocks[bufferIndex]).
	return StaticBuffer::blocks[bufferIndex];
}

void BlockBuffer::getHeader(struct HeadInfo *head){	
// get the starting address of the buffer containing the block using getBufferPtr. 
	 unsigned char * startOfBuffer = getBufferPtr();
	        
//copy the header of block to the memory location pointed to by the argument head using appropriate type casting
	*head = *((struct HeadInfo*) (startOfBuffer));

}

void BlockBuffer::setHeader(struct HeadInfo *head){
// get the starting address of the buffer containing the block using getBufferPtr.
	unsigned char * startOfBuffer = getBufferPtr();
	
//copy the contents of the memory location pointed to by the argument head to the header of block using appropriate type casting
	*((struct HeadInfo*) (startOfBuffer)) = *head;
	StaticBuffer::setDirtyBit(this->blockNum);

}

int BlockBuffer::getBlock(){
     //check whether the block is already present in the buffer using StaticBuffer.getBufferNum() .
		int bufferIndex = StaticBuffer::getBufferNum(this->blockNum);
		
		////cout <<"Inside getBlock() bufferIndex "<< bufferIndex<<endl;
		
     //if present, set the timestamp of the corresponding buffer to 0 and increment the timpestamps of all other occupied buffers in the BufferMetaInfo.
     if(bufferIndex != FAILURE){

     	for (int i = 0;i<32;++i) {
     		if(StaticBuffer::metainfo[i].timeStamp != -1)
     			StaticBuffer::metainfo[i].timeStamp++;
     	}
     	StaticBuffer::metainfo[bufferIndex].timeStamp=0;
     }
     
     //if not present, get a free buffer using StaticBuffer.getFreeBuffer() and read the block into the free buffer using readBlock().
     else {

     	bufferIndex = StaticBuffer::getFreeBuffer(this->blockNum);
     	//cout <<"Loaded to buffer " <<bufferIndex<<endl;
     	Disk::readBlock((void *)StaticBuffer::blocks[bufferIndex],this->blockNum);
     }
   ////cout <<"Exiting getBlock()\n";
   
   return bufferIndex;
}

int BlockBuffer::getFreeBlock(int blockType){
	//cout <<"Entering getFreeBlock\n";
//iterate through the StaticBuffer.blockAllocMap and find the index of a free block in the Disk::
	this->blockNum=-1;
	for(int i=0;i<DISK_BLOCKS;++i)
		if(((int32_t)(StaticBuffer::blockAllocMap[i]))==myUNUSED){
		    //cout<<(int32_t)(StaticBuffer::blockAllocMap[i])<<endl;
			this->blockNum = i;
			break;
		}
		//cout<<this->blockNum<<endl;
	
   
//if no block is free, return FAILURE.
	if(this->blockNum == -1)
		return FAILURE;
   
 //  cout<<"Current blockNum "<<this->blockNum<<endl;
//find a free buffer using StaticBuffer.getFreeBuffer() 
	int bufferIndex = StaticBuffer::getFreeBuffer(this->blockNum);

/*//update StaticBuffer.blockAllocMap.
	StaticBuffer::blockAllocMap[blockNum] = blockType;*/
	
	HeadInfo head;
	head.pblock = -1;
	head.lblock = -1;
	head.rblock = -1;
	head.numEntries = 0;
	head.numAttrs = 0;
	head.numSlots = 0;
	setHeader(&head);
   
	//update the block type of the block to the input block type using Blockbuffer.setBlockType().
	setBlockType(blockType);
	
   ////cout <<"Exiting getFreeBlock "<<this->blockNum<<" "<<blockNum<<" "<<bufferIndex<<"\n";
//return block number of the free block.
	return this->blockNum;

}

int BlockBuffer::getBlockNum(){

//return corresponding block number
	return this->blockNum;
}

int BlockBuffer::getBlockType(){
// get the starting address of the buffer containing the block using getBufferPtr. 
	 unsigned char * startOfBuffer = getBufferPtr();

//retrieve the first 4 bytes of the buffer that stores the block type.
	int blockType = *((int32_t*)(startOfBuffer));
	return blockType;
	
}

void BlockBuffer::setBlockType(int blockType){
//cout <<"Entering setBlockType\n";		
//find the starting address of the buffer using BlockBuffer.getBufferPtr()
unsigned char * startOfBuffer = getBufferPtr();

//store the given block type in the first 4 bytes of the buffer
*((int32_t*)(startOfBuffer)) = blockType;

//update the BlockAllocMap 
StaticBuffer::blockAllocMap[this->blockNum] = blockType;
StaticBuffer::setDirtyBit(this->blockNum);
	
//cout <<"Exiting setBlockType\n";
}

void BlockBuffer::releaseBlock(){
    
    int buffNum=StaticBuffer::getBufferNum(this->blockNum);
    if(buffNum!=-1)
	    StaticBuffer::metainfo[buffNum].free = true;
    
    StaticBuffer::blockAllocMap[this->blockNum] = myUNUSED;
    
    this->blockNum = -1;
    
    //return SUCCESS;
}
/****************************************Record Buffer**************************************************/
RecBuffer::RecBuffer(int blockNum) : BlockBuffer(blockNum){}
RecBuffer::RecBuffer() : BlockBuffer('R'){}

void RecBuffer::getSlotMap(unsigned char *slotMap){
	//cout <<"Entering getSlotMap\n";
	unsigned char *bufferPtr = getBufferPtr();
	int numOfSlots=*((int32_t*) (bufferPtr + 6*4));
	memcpy(slotMap, bufferPtr + 32, numOfSlots);
	
	//cout <<"Exiting getSlotMap\n";
}

//IMPORTANT: to be done only after setting the header
void RecBuffer::setSlotMap(unsigned char *slotMap){
	unsigned char *bufferPtr = getBufferPtr();
	int numOfSlots=*((int32_t*) (bufferPtr + 6*4));
	StaticBuffer::setDirtyBit(this->blockNum);
/*	for(int i=0; i<32; i++)
	    if(this->blockNum == staticBuffer.metainfo[i].blockNum)
	        staticBuffer.metainfo[i].dirty = true;*/ 
	memcpy(bufferPtr + 32, slotMap, numOfSlots);
	return;
}

int RecBuffer::getRecord(union Attribute *rec,int slotNum){ 

	//cout <<"Entering getRecord\n";
	unsigned char* bufferPtr = getBufferPtr();
	int numOfAttrib=*((int32_t*) (bufferPtr + 5*4));
	int numOfSlots=*((int32_t*) (bufferPtr + 6*4));
	if(slotNum < 0 || slotNum > numOfSlots - 1)
		return E_OUTOFBOUND;  
	//appu changed:Assumption max slot map possible is 120//
	unsigned char slotMap[120];
	getSlotMap(slotMap);
	if((int32_t*)(slotMap + slotNum) == 0)
	    return E_FREESLOT;
	//memcpy((void*)(bufferPtr + 32 + numOfSlots + (slotNum*numOfAttrib)*ATTR_SIZE), (void*)rec, numOfAttrib*ATTR_SIZE);
	
	memcpy((void*)rec, (void*)(bufferPtr + 32 + numOfSlots + (slotNum*numOfAttrib)*ATTR_SIZE), numOfAttrib*ATTR_SIZE);
	
	//cout <<"Exiting getRecord with blockNum "<<this->blockNum<<endl;
	return SUCCESS;
	
}

//Important: setRecord to be called only after setting the header
int RecBuffer::setRecord(union Attribute *rec,int slotNum){  //return type to be updated in the code 

	
	unsigned char* bufferPtr = getBufferPtr();
	int numOfAttrib=*((int32_t*) (bufferPtr + 5*4));
	int numOfSlots=*((int32_t*) (bufferPtr + 6*4));
	/*cout <<"Entering setRecord with num of attrs: "<<numOfAttrib<<"\n";
	if(numOfAttrib == 2)
		cout<<"Inside stu table\n";*/
	//cout <<"Attr && Slots "<<numOfAttrib<<" "<<numOfSlots<<"\n";
	if(slotNum < 0 || slotNum > numOfSlots - 1)
		return E_OUTOFBOUND;
	
	//memcpy((void*)rec, (void*)(bufferPtr + 32 + numOfSlots +(slotNum*numOfAttrib)*ATTR_SIZE), numOfAttrib*ATTR_SIZE);
	
	memcpy((void*)(bufferPtr + 32 + numOfSlots +(slotNum*numOfAttrib)*ATTR_SIZE), (void*)rec, numOfAttrib*ATTR_SIZE);
	
	StaticBuffer::setDirtyBit(this->blockNum);//update dirty bit
	//cout <<"Exiting setRecord "<<numOfAttrib<<" "<<numOfSlots<<"\n";
	return SUCCESS;
}


/**********************************************IND BUFFER***********************************************************************/
IndBuffer::IndBuffer(int blockNum) : BlockBuffer(blockNum){}
IndBuffer::IndBuffer(char blockType) : BlockBuffer(blockType){} //to update camelCase in "char blocktype" 

/**********************************************Class IndInternal***************************************************************/
IndInternal::IndInternal() : IndBuffer('I'){}
IndInternal::IndInternal(int blockNum) : IndBuffer(blockNum){}

int IndInternal::getEntry(void *ptr, int indexNum){
    unsigned char* bufferPtr = getBufferPtr();
    /*int numOfEntries = *((int32_t*) (bufferPtr + 4*4));
    if(indexNum<0 || indexNum > numOfEntries - 1) //to be updated 
        return E_OUTOFBOUND;*/
    int numOfSlots=100;
    if(indexNum<0 || indexNum > numOfSlots - 1) //to be updated 
        return E_OUTOFBOUND;
    struct InternalEntry entry;
	entry=*((struct InternalEntry*) (bufferPtr + 32 + indexNum*20)); //need to double check as snrs code was different
	(*(struct InternalEntry*)ptr) = entry;
    return SUCCESS;
}

int IndInternal::setEntry(void *ptr, int indexNum){
    unsigned char* bufferPtr = getBufferPtr();
    /*int numOfEntries = *((int32_t*) (bufferPtr + 4*4));
    if(indexNum<0 || indexNum > numOfEntries - 1) //to be updated 
        return E_OUTOFBOUND;*/
    int numOfSlots=100;
    if(indexNum<0 || indexNum > numOfSlots - 1) //to be updated 
        return E_OUTOFBOUND;
    struct InternalEntry Entry;
    Entry = (*(struct InternalEntry*)ptr);
	*((struct InternalEntry*) (bufferPtr + 32 + indexNum*20)) = Entry;
	StaticBuffer::setDirtyBit(this->blockNum);//update dirty bit
    return SUCCESS;

}
/*
int IndInternal::releaseIndBLock() {
	if(StaticBuffer::getStaticBlockType(this->blockNum)!= IND_INTERNAL)
		return FAILURE;
	releaseBlock();
	return SUCCESS;
}*/
/******************************************************IND_LEAF**********************************************************************/
IndLeaf::IndLeaf() : IndBuffer('L'){}

IndLeaf::IndLeaf(int blockNum) : IndBuffer(blockNum){}

int IndLeaf::getEntry(void *ptr, int indexNum){

	// get the starting address of the buffer containing the block using BlockBuffer.getBufferPtr().
	/***To be updated: no need for BlockBuffer.***/
	unsigned char * startOfBuffer = getBufferPtr();

	// if the indexNum is not in range of 0-(#Entries(in block)+1), return E_OUTOFBOUND

	/*int numEntries = *((int32_t *)(startOfBuffer + 4 * 4));

	if((indexNum<0) || indexNum > numEntries)
		return E_OUTOFBOUND;*/
		
	int numOfSlots=63;
    if(indexNum<0 || indexNum > numOfSlots - 1) //to be updated 
        return E_OUTOFBOUND;

	// copy the indexNum'th Index entry in block to memory ptr(ptr can be type casted appropriately if needed). 
		struct Index Entry;
		Entry=*((struct Index*) (startOfBuffer + 32 + indexNum*32));
		*((struct Index*)ptr) = Entry;
	
		
	// return SUCCESS.
		return SUCCESS;

}

int IndLeaf::setEntry(void *ptr, int indexNum){						
    unsigned char* bufferPtr = getBufferPtr();
    /*int numOfEntries = *((int32_t*) (bufferPtr + 4*4));
    if(indexNum<0 || indexNum > numOfEntries - 1) //to be updated 
        return E_OUTOFBOUND;*/
    int numOfSlots=63;
    if(indexNum<0 || indexNum > numOfSlots - 1) //to be updated 
        return E_OUTOFBOUND;

    struct Index Entry;
    Entry = (*(struct Index*)ptr);
	*((struct Index*) (bufferPtr + 32 + indexNum*32)) = Entry;
	StaticBuffer::setDirtyBit(this->blockNum);//update dirty bit
    return SUCCESS;
}

/*
int IndLeaf::releaseIndBLock() {
	if(StaticBuffer::getStaticBlockType(this->blockNum)!= IND_LEAF)
		return FAILURE;
	releaseBlock();
	return SUCCESS;
}*/

/* TO SEE WHAT THIS IS
class BlockBuffer{
public:
	
	// Making destructor virtual to allow dynamic_cast.
	// Refer to following link for more info:
	// https://stackoverflow.com/questions/15114093/getting-source-type-is-not-polymorphic-when-trying-to-use-dynamic-cast
	//https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors  also useful
	virtual ~BlockBuffer();
};*/
