/* This file contains code for all member functions of 'BlockBuffer', 'RecBuffer' and 'IndBuffer' classes */

#include <cstring>
#include <cstdint>
#include "../define/constants.h"
#include "blockBuffer.h"
#include "buffer.h"

//BlockBuffer member functions

BlockBuffer::BlockBuffer(int blk_no){
	this->block_num=blk_no;
	//this->buff_ptr=buff;
}

struct HeadInfo BlockBuffer::getheader(){
	unsigned char* data_ptr=get_dataptr();
	struct HeadInfo head;
	head.block_type=*(int32_t *)(data_ptr);
	head.pblock= *(int32_t *)(data_ptr + 4);
	head.lblock= *(int32_t *)(data_ptr + 2*4);
	head.rblock= *(int32_t *)(data_ptr + 3*4);
	head.num_entries= *(int32_t *)(data_ptr + 4*4);
	head.num_attrs=*(int32_t *)(data_ptr + 5*4);
	head.num_slots=*(int32_t *)(data_ptr + 6*4);

	return head;
}

void BlockBuffer::setheader(struct HeadInfo head){
	unsigned char* data_ptr=get_dataptr();
	*((struct HeadInfo*)(data_ptr))=head;
	return;
}

unsigned char * BlockBuffer::get_dataptr(){
	return Buffer::get_buf_dataptr(block_num);
}

BlockBuffer :: ~BlockBuffer() {
	//call buff_ptr->releaseblock(this->block_num);
}


//RecBuffer member functions

RecBuffer::RecBuffer(int blk_no) : BlockBuffer(blk_no){}

void RecBuffer::getSlotmap(unsigned char *slotmap){ //Assumes sufficient space is allocated
	unsigned char* data_ptr=get_dataptr();
	int num_of_slots=*((int32_t*) (data_ptr + 6*4));
	memcpy(slotmap,data_ptr+32,num_of_slots);
	return;
}

void RecBuffer::setSlotmap(unsigned char *slotmap){
	unsigned char* data_ptr=get_dataptr();
	int num_of_slots=*((int32_t*)(data_ptr + 6*4));
	memcpy(data_ptr+32,slotmap,num_of_slots);
	return;
}

void RecBuffer::getRecord(union Attribute *rec,int slot_num){ //Assumes enough memory is allocated.
	unsigned char* data_ptr=get_dataptr();
	int num_of_attrib=*((int32_t*) (data_ptr + 5*4));
	int num_of_slots=*((int32_t*) (data_ptr + 6*4));
	memcpy((void*)(data_ptr + 32 + num_of_slots +(slot_num*num_of_attrib)*ATTR_SIZE),(void*)rec,num_of_attrib*ATTR_SIZE);
	return;
}

void RecBuffer::setRecord(union Attribute *rec,int slot_num){
	unsigned char* data_ptr=get_dataptr();
	int num_of_attrib=*((int32_t*) (data_ptr + 5*4));
	int num_of_slots=*((int32_t*) (data_ptr + 6*4));
	memcpy((void*)rec,(void*)(data_ptr + 32 + num_of_slots +(slot_num*num_of_attrib)*ATTR_SIZE),num_of_attrib*ATTR_SIZE);
	return;
}

//IndBuffer member functions

IndBuffer::IndBuffer(int blk_no) : BlockBuffer(blk_no, buff){}

IndInternal::IndInternal(int blk_no) : IndBuffer(blk_no, buff){}
IndLeaf::IndLeaf(int blk_no) : IndBuffer(blk_no, buff){}


struct InternalEntry IndInternal::getInternalEntry(int index_num){
	unsigned char* data_ptr=get_dataptr();
	struct InternalEntry Entry;
	Entry=*((struct InternalEntry*) (data_ptr + 32 + index_num*24)); 
	return Entry;
}

void IndInternal::setInternalEntry(struct InternalEntry Entry,int index_num){
	unsigned char* data_ptr=get_dataptr();
	*((struct InternalEntry*) (data_ptr + 32 + index_num*24))=Entry; 
	return ;
}

struct Index IndLeaf::getIndexval(int index_num){
	unsigned char* data_ptr=get_dataptr();
	struct Index IndexEntry;
	IndexEntry=*((struct Index*) (data_ptr + 32 + index_num*32)); 
	return IndexEntry;
}

void IndLeaf::setIndexval(struct Index IndexEntry,int index_num){
	unsigned char* data_ptr=get_dataptr();
	*((struct IndexLeaf*) (data_ptr + 32 + index_num*32))=IndexEntry; 
	return ;
}
