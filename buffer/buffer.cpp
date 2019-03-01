/* This file contains code for all member functions of 'Buffer' class */
#include <cstdint>
#include "buffer.h"
#include "../define/constants.h"
#include "blockBuffer.h"
#include <cstring>


int Buffer::getBlockType(int buffer_index){
	int block_type = *(int32_t *)&blocks[buffer_index][0];
	return block_type;
}

int Buffer::getFreeBuffer(){
	int iter;
	for(iter=0;iter<32;iter++){
		if(metainfo[iter].free){
			return iter;
		}
	}
	return -1; ///no free buffer
}

int Buffer::getbufferblock(int block){
	int iter;
	for(iter=0;iter<32;iter++){
		if(!metainfo[iter].free && metainfo[iter].block_num==block){
			return iter;
		}
	}
	return -1; //disk 'block' is not found in buffer
}

int Buffer::loadBlock(int block_num){
	int free_buffer=getFreeBuffer();
	if(free_buffer==-1){
		return -1; /// Buffer replaceent algorithm goes here ////
	}
	
	//////code to copy block from disk to buffer////////////////

	metainfo[free_buffer].free=false;
	metainfo[free_buffer].dirty=false;
	metainfo[free_buffer].block_num=block_num;

	int block_type = *(int32_t *)(&blocks[free_buffer][0]);

	if(block_type==REC){
		class RecBuffer* newRecBuffer= new RecBuffer(block, this);
		metainfo[free_buffer].blk=newRecBuffer;
	}else{
		if(block_type==INDINT){
		  class IndBuffer* newIndBuffer= new IndInternal(block, this);
		  metainfo[free_buffer].blk=newIndBuffer;
		}else if(block_type==INDLEAF){
		  class IndBuffer* newIndBuffer= new IndLeaf(block, this);
		  metainfo[free_buffer].blk=newIndBuffer;
		}
	} 
	return free_buffer;
}

void Buffer::releaseBufferBlock(int buffer_index){
	metainfo[buffer_index].free=true;
	metainfo[buffer_index].dirty=false;
	/// also free metainfo[i].blk;/// also free metainfo[i].blk;metainfo[i].blk=NULL;
	metainfo[buffer_index].block_num=-1;
	return;
}

unsigned char* Buffer::get_buf_dataptr(int block_num){
	int buffer_block=getbufferblock(block_num);
	if(buffer_block==-1){
		//use replacement algo
	}else{
		return &(blocks[buffer_block][0]);
	}
}

//Public functions

class RecBuffer * Buffer::getFreeRecBlock(){
	int iter;
	for(iter=4;iter<DISK_BLOCKS;iter++){
		if(block_alloc_map[iter]==0){
			break;
		}
	}

	if(iter == DISK_BLOCKS){
		return NULL;
	}else{
		int FreeBuffer=getFreeBuffer();
		if(FreeBuffer==-1){///no free buffer found -- Replacement must be done
			return NULL;
		}
		*(int32_t *)&blocks[FreeBuffer][0]=REC;
		metainfo[FreeBuffer].free=false;
		metainfo[FreeBuffer].dirty=true;
		metainfo[FreeBuffer].block_num=iter;
		class RecBuffer* newRecBuffer= new RecBuffer(iter, this);
		metainfo[FreeBuffer].blk=newRecBuffer;

		block_alloc_map[iter]=1;

		return newRecBuffer;
	}
}

class IndInternal * Buffer::getFreeIndInternal(){
	int iter;
	for(iter=4;iter<DISK_BLOCKS;iter++){
		if(block_alloc_map[iter]==0){
			break;
		}
	}

	if(iter == DISK_BLOCKS){
		return NULL;
	}else{
		int FreeBuffer=getFreeBuffer();
		if(FreeBuffer==-1){///no free buffer found -- Replacement must be done
			return NULL;
		}
		*(int32_t *)&blocks[FreeBuffer][0]=IND;
		metainfo[FreeBuffer].free=false;
		metainfo[FreeBuffer].dirty=true;
		metainfo[FreeBuffer].block_num=iter;
		class IndInternal* newIndBuffer= new IndInternal(iter, this);
		metainfo[FreeBuffer].blk=newIndBuffer;

		block_alloc_map[iter]=1;

		return newIndBuffer;
	}
}

class IndLeaf * Buffer::getFreeIndLeaf(){
	int iter;
	for(iter=4;iter<DISK_BLOCKS;iter++){
		if(block_alloc_map[iter]==0){
			break;
		}
	}

	if(iter == DISK_BLOCKS){
		return NULL;
	}else{
		int FreeBuffer=getFreeBuffer();
		if(FreeBuffer==-1){///no free buffer found -- Replacement must be done
			return NULL;
		}
		*(int32_t *)&blocks[FreeBuffer][0]=IND;
		metainfo[FreeBuffer].free=false;
		metainfo[FreeBuffer].dirty=true;
		metainfo[FreeBuffer].block_num=iter;
		class IndLeaf* newIndBuffer= new IndLeaf(iter, this);
		metainfo[FreeBuffer].blk=newIndBuffer;

		block_alloc_map[iter]=1;

		return newIndBuffer;
	}
}

class RecBuffer * Buffer::getRecBlock(int block_num){
	int buffer_block=getbufferblock(block_num);

	if(buffer_block==-1){
		buffer_block=load_block(block_num);
	}

	if(buffer_block!=-1){
		int blk_type=getBlockType(buffer_block);
		if(blk_type!=REC){
			return NULL;
		}
		return dynamic_cast<RecBuffer*>(metainfo[buffer_block].blk);
	}else{ //will not be required if replacement is done.
		return NULL;
	}
}

class IndBuffer * Buffer::getIndBlock(int block_num){
	int buffer_block=getbufferblock(block_num);

	if(buffer_block==-1){
		buffer_block=load_block(block_num);
	}

	if(buffer_block!=-1){
		int blk_type=getBlockType(buffer_block);
		if(blk_type!=IND){
			return NULL;
		}
		return dynamic_cast<IndBuffer*>(metainfo[buffer_block].blk);
	}else{ //will not be required if replacement is done.
		return NULL;
	}
}


void Buffer::releaseBlock(int block_num){ //To release and commit to disk.
	if(block_num < 0 || block_num >= DISK_BLOCKS){
		return;  
	}

	int buffer_block=getbufferblock(block_num);

	if(buffer_block!=-1){
		if(metainfo[buffer_block].dirty){
		 int write_to_disk=writeblock(block_num, void* (&blocks[buffer_block][0]);
		}
		releaseBufferBlock(buffer_block);
	}

}

void Buffer::deleteBlock(int block_num){ //To completely free any block.
	if(block_num < 0 || block_num >= DISK_BLOCKS){
		return;  
	}

	int buffer_block=getbufferblock(block_num);

	if(buffer_block!=-1){
		releaseBufferBlock(buffer_block);
	}

	block_alloc_map[block_num]=UNUSED;
}
