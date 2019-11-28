#include "blockBuffer.h"
#include "staticBuffer.h"
#include "../define/constants.h"
#include "../define/error.h"
using namespace std;



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
/****************************************************************************************************************************/

