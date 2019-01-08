union Attribute{
	int ival;
	float fval;
	char strval[ATTR_SIZE];
};

struct Index{
	int lchild;
	union Attribute attrval;
	int block;
	int slot;
	unsigned char unused[8];
	int rchild;
};

struct HeadInfo{
	int block_type;
	int pblock;
	int lblock;
	int rblock;
	int num_entries;
};

struct BufferMetaInfo {
	bool free;
	bool dirty;
	int block_num;
	class BlockBuffer *blk;	
};

class BlockBuffer{
protected:
	unsigned char *data_ptr;
	struct BufferMetaInfo *meta_ptr;

public:
	BlockBuffer(unsigned char *data,struct BufferMetaInfo *meta){
		this->data_ptr=data;
		this->meta_ptr=meta;
	}
	struct HeadInfo getheader(){
		struct HeadInfo head;
		head->block_type=*(int *)(this->data_ptr);
		head->pblock= *(int *)(this->data_ptr + 4);
		head->lblock= *(int *)(this->data_ptr + 2*4);
		head->rblock= *(int *)(this->data_ptr + 3*4);
		head->num_entries= *(int *)(this->data_ptr + 4*4);
		return head;
	}
	
	void setheader(struct HeadInfo head){
		*(struct HeadInfo*(this->data_ptr))=head;
		return;
	}

};

class RecBuffer : public BlockBuffer{
public:
	void getSlotmap(unsigned char *slotmap, int num_of_slots){
		memcpy(slotmap,this->data_ptr+32,num_of_slots);
		return;
	}
	
	void setSlotmap(unsigned char *slotmap, int num_of_slots){
		memcpy(this->data_ptr+32,slotmap,num_of_slots);
		return;
	}
	
	void getRecord(union Attribute *rec,int slot_num, int num_of_attrib, int num_of_slots){
		
		memcpy(void*(this->data_ptr + 32 + num_of_slots +(slot_num*num_of_attrib)*ATTR_SIZE),void*(rec),num_of_attrib*ATTR_SIZE);
		return;
	}
	void setRecord(union Attribute *rec,int slot_num, int num_of_attrib, int num_of_slots){
		
		memcpy(void*(rec),void*(this->data_ptr + 32 + num_of_slots +(slot_num*num_of_attrib)*ATTR_SIZE),num_of_attrib*ATTR_SIZE);
		return;
	}

	/*union Attribute getAttribute(int slot_num, int num_of_attrib, int num_of_slots, int attr_offset){
		union Attribute attr;
		attr=*(union Attribute* (this->data_ptr + 32 + num_of_slots + (slot_num*num_of_attrib + attr_offset)*ATTR_SIZE ));
		return attr;
		//return union Attribute* (this->data_ptr + 32 + num_of_slots + (slot_num*num_of_attrib + attr_offset)*ATTR_SIZE );
	}
	void setAttribute(union Attribute attr,int slot_num, int num_of_attrib, int num_of_slots, int attr_offset){
		*(union Attribute* (this->data_ptr + 32 + num_of_slots + (slot_num*num_of_attrib + attr_offset)*ATTR_SIZE ))=attr;
		return;
	}*/

};

class IndBuffer : public BlockBuffer{
public:
	struct IndexVal getIndexval(int index_num){
		struct Index IndexEntry;
		IndexEntry=*(struct IndexVal* (this->data_ptr + 32 + index_num*36)); 
		return IndexEntry;
	}
	void setIndexval(struct Index IndexEntry,int index_num){
		*(struct IndexVal* (this->data_ptr + 32 + index_num*36))=IndexEntry; 
		return ;
	}
	/*
	struct Index *getIndex(int index_num){
		return struct Index* (this->data_ptr + 32 + index_num*32);
	}

	int *getChild(int child_num){
		return  (this->data_ptr + 1792 + child_num*4);
	}*/
}

class Buffer{
private:
	unsigned char blocks[32][BLOCK_SIZE];
	struct BufferMetaInfo metainfo[32];
	struct OpenRelTable rel_table[8];
	unsigned char block_alloc_map[DISK_BLOCKS];

	int getFreeBuffer(){
		int iter;
		for(iter=0;iter<32;iter++){
			if(metainfo[iter].free){
				return iter;
			}
		}
		return -1; ///no free buffer
	}

	int getbufferblock(int block){
		int iter;
		for(iter=0;iter<32;iter++){
			if(!metainfo[iter].free && metainfo[iter].block_num==block){
				return iter;
			}
		}
		return -1; //disk 'block' is not found in buffer
	}

	int load_block(int block){
		int free_buffer=getFreeBuffer();
		if(free_buffer==-1){
			return -1;
		}
		
		//////code to copy block from disk to buffer////////////////

		metainfo[free_buffer].free=false;
		metainfo[free_buffer].dirty=false;
		metainfo[free_buffer].block_num=block;

		int block_type = *(int *)(&blocks[free_buffer][0]);

		if(block_type==REC){
			class RecBuffer* newRecBuffer= new RecBuffer(&blocks[FreeBuffer],&metainfo[FreeBuffer]);
			metainfo[FreeBuffer].blk=newRecBuffer;
		}else if(block_type==IND){
			class IndBuffer* newIndBuffer= new IndBuffer(&blocks[FreeBuffer],&metainfo[FreeBuffer]);
			metainfo[FreeBuffer].blk=newIndBuffer;
		}
		return free_buffer;
	}

	void releaseBufferBlock(buffer_block){
		metainfo[i].free=true;
		metainfo[i].dirty=false;
		//// also free metainfo[i].blk;
		metainfo[i].block_num=-1;
		return;
	}

	
public:
	/*Block related functions*/
	class RecBuffer *getFreeRecBlock(){
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
			if(FreeBuffer==-1){///no free buffer found
				return NULL;
			}
			*(int *)&blocks[FreeBuffer][0]=REC;
			metainfo[FreeBuffer].free=false;
			metainfo[FreeBuffer].dirty=true;
			metainfo[FreeBuffer].block=iter;
			class RecBuffer* newRecBuffer= new RecBuffer(&blocks[FreeBuffer],&metainfo[FreeBuffer]);
			metainfo[FreeBuffer].blk=newRecBuffer;

			block_alloc_map[iter]=1;

			return newRecBuffer;
		}
	}

	class IndBuffer *getFreeIndBlock(){
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
			if(FreeBuffer==-1){///no free buffer found
				return NULL;
			}
			*(int *)&blocks[FreeBuffer][0]=IND;
			metainfo[FreeBuffer].free=false;
			metainfo[FreeBuffer].dirty=true;
			metainfo[FreeBuffer].block=iter;
			class IndBuffer* newIndBuffer= new IndBuffer(&blocks[FreeBuffer],&metainfo[FreeBuffer]);
			metainfo[FreeBuffer].blk=newIndBuffer;

			block_alloc_map[iter]=1;

			return newIndBuffer;
		}
	}

	class RecBuffer *getRecBlock(int block_num){
		int buffer_block=getbufferblock(block_num);

		if(buffer_block==-1){
			buffer_block=load_block(block_num);
		}

		if(buffer_block!=-1){
			int blk_type=getBlockType(metainfo[buffer_block].blk);
			if(blk_type!=REC){
				return NULL;
			}
			return dynamic_cast<RecBuffer*>(metainfo[buffer_block].blk);
		}else{
			return NULL;
		}
	}

	class IndBuffer *getIndBlock(int block_num){
		int buffer_block=getbufferblock(block_num);

		if(buffer_block==-1){
			buffer_block=load_block(block_num);
		}

		if(buffer_block!=-1){
			int blk_type=getBlockType(metainfo[buffer_block].blk);
			if(blk_type!=IND){
				return NULL;
			}
			return dynamic_cast<IndBuffer*>(metainfo[buffer_block].blk);
		}else{
			return NULL;
		}
	}

	void releaseBlock(int block_num){
		if(block_num < 0 || block_num >= DISK_BLOCKS){
			return;  
		}

		int buffer_block=getbufferblock(block_num);

		if(buffer_block!=-1){
			releaseBufferBlock(buffer_block);
		}

		block_alloc_map[block_num]=UNUSED;
	}

};
