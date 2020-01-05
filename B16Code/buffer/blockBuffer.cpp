#include "blockBuffer.h"
#include "staticBuffer.h"
#include "../define/constants.h"
#include "../define/error.h"
using namespace std;

BlockBuffer::BlockBuffer(int blockNum){
// set the blockNum field of the object to input argument.
this->blockNum = blockNum;

// copy the block into buffer using getBlock function.
/***Note: disregard the return type***/
/***To be updated: make it camel case***/
getBlock();

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
	int bufferIndex = getBlock();
	
	// return the pointer to this buffer (blocks[bufferIndex]).
	return staticBuffer.blocks[bufferIndex];
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

}

int BlockBuffer::getBlock(){
     //check whether the block is already present in the buffer using StaticBuffer.getBufferNum() .
		int bufferIndex = staticBuffer.getBufferNum(this->blockNum);
		
     //if present, set the timestamp of the corresponding buffer to 0 and increment the timpestamps of all other occupied buffers in the BufferMetaInfo.
     if(bufferIndex != -1){
     	for (int i = 0;i<32;++i) {
     		if(staticBuffer.metaInfo[i].timeStamp != -1)
     			staticBuffer.metaInfo[i].timeStamp++;
     	}
     	staticBuffer.metaInfo[bufferIndex].timeStamp=0;
     }
     
     //if not present, get a free buffer using StaticBuffer.getFreeBuffer() and read the block into the free buffer using readBlock().
     else {
     	bufferIndex = staticBuffer.getFreeBuffer(this->blockNum);
     	readblock((void *)staticBuffer.blocks[bufferIndex],this->blockNum);
     }
   
}

int BlockBuffer::getFreeBlock(int blockType){
//iterate through the StaticBuffer.blockAllocMap and find the index of a free block in the disk.
	int blockNum=-1;
	for(int i=0;i<32;++i)
		if(((int32_t)(staticBuffer.blockAllocMap[i]))==USED)
			blockNum = i;
	
   
//if no block is free, return FAILURE.
	if(blockNum == -1)
		return FAILURE;
   
//find a free buffer using StaticBuffer.getFreeBuffer() 
	int bufferIndex = staticBuffer.getFreeBuffer(this->blockNum);

//update StaticBuffer.blockAllocMap.
	staticBuffer.blockAllocMap[blockNum] = (unsigned char) blockType;
   
//update the block type of the block to the input block type using Blockbuffer.setBlockType().
	setBlockType(blockType);
	
   
//return block number of the free block.
	return blockNum;

}

int BlockBuffer::getBlockNum(){

//return corresponding block number
	return this->blockNum;
}

int BlockBuffer::getBlockType(int bufferIndex){

//blocks[bufferIndex][0] gives the staring address of the buffer
unsigned char * startOfBuffer = staticBuffer.blocks[bufferIndex];

//retrieve the first 4 bytes of the buffer that stores the block type.
	int blockType = *((int32_t*)(startOfBuffer));
	return blockType;
	
}

void BlockBuffer::setBlockType(int blockType){						
//find the starting address of the buffer using BlockBuffer.getBufferPtr()
unsigned char * startOfBuffer = getBufferPtr();

//store the given block type in the first 4 bytes of the buffer
*((int32_t*)(startOfBuffer)) = blockType;

//update the BlockAllocMap 
staticBuffer.blockAllocMap[this->blockNum] = (unsigned char) blockType;
	
}


/****************************************Record Buffer**************************************************/
RecBuffer::RecBuffer(int blockNum) : BlockBuffer(blockNum){}
RecBuffer::RecBuffer() : BlockBuffer('R'){}

void RecBuffer::getSlotMap(unsigned char *slotMap){
	unsigned char *bufferPtr = getBufferPtr();
	int numOfSlots=*((int32_t*) (bufferPtr + 6*4));
	memcpy(slotMap, bufferPtr + 32, numOfSlots);
} 
void RecBuffer::setSlotMap(unsigned char *slotMap){
	unsigned char *bufferPtr = getBufferPtr();
	int numOfSlots=*((int32_t*) (bufferPtr + 6*4));
	//TODO : update dirty bit
/*	for(int i=0; i<32; i++)
	    if(this->blockNum == staticBuffer.metainfo[i].blockNum)
	        staticBuffer.metainfo[i].dirty = true;*/ 
	memcpy(bufferPtr + 32, slotMap, numOfSlots);
	return;
}

int RecBuffer::getRecord(union Attribute *rec,int slotNum){ 
	unsigned char* bufferPtr = getBufferPtr();
	int numOfAttrib=*((int32_t*) (bufferPtr + 5*4));
	int numOfSlots=*((int32_t*) (bufferPtr + 6*4));
	if(slotNum < 0 || slotNum > numOfSlots - 1)
		return E_OUTOFBOUND;  
	unsigned char *slotMap = new unsigned char;
	getSlotMap(slotMap);
	if((int32_t*)(slotMap + slotNum) == 0)
	    return E_FREESLOT;
	memcpy((void*)(bufferPtr + 32 + numOfSlots + (slotNum*numOfAttrib)*ATTR_SIZE), (void*)rec, numOfAttrib*ATTR_SIZE);
	return SUCCESS;
	
}

int RecBuffer::setRecord(union Attribute *rec,int slotNum){  //return type to be updated in the code 
	unsigned char* bufferPtr = getBufferPtr();
	int numOfAttrib=*((int32_t*) (bufferPtr + 5*4));
	int numOfSlots=*((int32_t*) (bufferPtr + 6*4));
	if(slotNum < 0 || slotNum > numOfSlots - 1)
		return E_OUTOFBOUND;
	memcpy((void*)rec, (void*)(bufferPtr + 32 + numOfSlots +(slotNum*numOfAttrib)*ATTR_SIZE), numOfAttrib*ATTR_SIZE);
	//TODO : update dirty bit
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
    int numOfEntries = *((int32_t*) (bufferPtr + 4*4));
    if(indexNum<0 || indexNum > numOfEntries - 1) //to be updated 
        return E_OUTOFBOUND;
    struct InternalEntry entry;
	entry=*((struct InternalEntry*) (bufferPtr + 32 + indexNum*20)); //need to double check as snrs code was different
	(*(struct InternalEntry*)ptr) = entry;
    return SUCCESS;
}

int IndInternal::setEntry(void *ptr, int indexNum){
    unsigned char* bufferPtr = getBufferPtr();
    int numOfEntries = *((int32_t*) (bufferPtr + 4*4));
    if(indexNum<0 || indexNum > numOfEntries - 1) //to be updated 
        return E_OUTOFBOUND;
    struct InternalEntry Entry;
    Entry = (*(struct InternalEntry*)ptr);
	*((struct InternalEntry*) (bufferPtr + 32 + indexNum*24)) = Entry;
	//TODO : update dirty bit
    return SUCCESS;

}
/******************************************************IND_LEAF**********************************************************************/
IndLeaf::IndLeaf() : IndBuffer('L'){}

IndLeaf::IndLeaf(int blockNum) : IndBuffer(blockNum){}

int IndLeaf::getEntry(void *ptr, int indexNum){

// get the starting address of the buffer containing the block using BlockBuffer.getBufferPtr().
/***To be updated: no need for BlockBuffer.***/
unsigned char * startOfBuffer = getBufferPtr();

// if the indexNum is not in range of 0-(#Entries(in block)+1), return E_OUTOFBOUND

int numEntries = *((int32_t *)(startOfBuffer + 4 * 4));

if((indexNum<0) || indexNum > numEntries)
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
    int numOfEntries = *((int32_t*) (bufferPtr + 4*4));
    if(indexNum<0 || indexNum > numOfEntries - 1) //to be updated 
        return E_OUTOFBOUND;
    struct Index Entry;
    Entry = (*(struct Index*)ptr);
	*((struct Index*) (bufferPtr + 32 + indexNum*32)) = Entry;
	//TODO : update dirty bit
    return SUCCESS;
}

