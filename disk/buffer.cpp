union Attribute{
	int ival;
	float fval;
	char strval[ATTR_SIZE];
};

struct Index{
	union Attribute attrval;
	int block;
	int slot;
};

struct AttributeCache{
	char rel_name[ATTR_SIZE];
	char attr_name[ATTR_SIZE];
	int attr_type;
	bool primary_flag;
	int root_block;
	int offset;
	struct AttributeCache *next;
};

struct BufferMetaInfo{
	bool free;
	bool dirty;
	int block_num;
	class BlockBuffer *blk;	
};

class BlockBuffer{
protected:
	char *data_ptr;
	struct BufferMetaInfo *meta_ptr;

public:
	BlockBuffer(char *data,struct BufferMetaInfo *meta){
		this->data_ptr=data;
		this->meta_ptr=meta;
	}
	int getBlockType(){
		int block_type = *(int *)(this->data_ptr);
		return block_type;
	}

	int getPblock(){
		int p_block = *(int *)(this->data_ptr + 4);
		return p_block;
	}

	int getLblock(){
		int l_block = *(int *)(this->data_ptr + 2*4);
		return l_block;
	}

	int getRblock(){
		int r_block = *(int *)(this->data_ptr + 3*4);
		return r_block;
	}

	int getNumEntries(){
		int num_entries = *(int *)(this->data_ptr + 4*4);
			return num_entries;
	}

	void setPblock(int block_num){
		*(int *)(this->data_ptr + 4) = block_num;
	}

	void setLblock(int block_num){
		*(int *)(this->data_ptr + 2*4) = block_num;
	}

	void setRblock(int block_num){
		*(int *)(this->data_ptr + 3*4) = block_num;
	}

	void setNumEntries(int num){
		*(int *)(this->data_ptr + 4*4) = num;
	}
};

class RecBuffer : public BlockBuffer{
public:
	char *getSlotmap(){
		return this->data_ptr + 32;
	}

	union Attribute *getRecord(int slot_num, int num_of_attrib, int num_of_slots){
		return union Attribute* (this->data_ptr + 32 + num_of_slots + slot_num*num_of_attrib*ATTR_SIZE);
	}

	union Attribute *getAttribute(int slot_num, int num_of_attrib, int num_of_slots, int attr_offset){
		return union Attribute* (this->data_ptr + 32 + num_of_slots + (slot_num*num_of_attrib + attr_offset)*ATTR_SIZE );
	}

};

class IndBuffer : public BlockBuffer{
public:
	struct Index *getIndex(int index_num){
		return struct Index* (this->data_ptr + 32 + index_num*32);
	}

	int *getChild(int child_num){
		return  (this->data_ptr + 1792 + child_num*4);
	}
}

struct OpenRelTable{
	char relname[ATTR_SIZE];
	int num_attr;
	int num_rec;
	int first_blk;
	int num_slots_blk;
	struct AttributeCache *attr_list;
	bool free, dirty;
	int block, slot;
};

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

	/*Open Relation table (Relation cache) and attribute cache related functions*/
	int getFreeRelId(){

	}

	int getRelId(char relname[ATTR_SIZE]){

	}

	int getNumAttr(int rel_id){

	}

	int getNumRec(int rel_id){

	}

	int getFirstBlk(int rel_id){

	}

	int getNumSlotsBlk(int rel_id){

	}

	struct AttributeCache* getAttrCache(int rel_id){

	}

	void setAttrCache(struct AttributeCache* attr_list){

	}

	void setNumRec(int rel_id, int num_rec){

	}

	void setFirstBlk(int rel_id, int first_blk){

	}


};