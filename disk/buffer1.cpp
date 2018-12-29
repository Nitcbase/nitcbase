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

class BlockBuffer{
	public:
		unsigned char data[BLOCK_SIZE];
	private:
		int block;
		bool dirty, free;


	public:
		int getBlockType(){
			int block_type = *(int *)(this->data);
			return block_type;
		}
		int getPblock(){
			int p_block = *(int *)(this->data + 4);
			return p_block;
		}
		int getLblock(){
			int l_block = *(int *)(this->data + 2*4);
			return l_block;
		}
		int getRblock(){
			int r_block = *(int *)(this->data + 3*4);
			return r_block;
		}
		int getNumentries(){
			int num_entries= *(int *)(this->data + 4*4);
			return num_entries;
		}

		virtual unsigned char* getSlotmap(){
			return NULL;
		}
		



}

struct RecBuffer{
	int blktype;
	int pblock,lblock,rblock;
	int num_of_entries;
	unsigned char reserved[12];
	unsigned char data[2016];
};


struct IndBuffer{
	int blktype;
	int pblock,lblock,rblock;
	int num_of_entries;
	unsigned char reserved[12];
	struct Index indices[55];
	int children[56];
	unsigned char unused[32];
};

struct RelcatEntry{
	union Attribute relname;
	union Attribute num_of_attr;
	union Attribute num_of_rec;
	union Attribute firstblk;
	union Attribute no_of_slots_blk;
	union Attribute reserved1, reserved2, reserved3;
};

struct RelationCache{
	int blktype;
	int pblock,lblock,rblock;
	int num_of_entries;
	unsigned char reserved[12];
	unsigned char slotmap[15];
	struct RelcatEntry relcat_entry[15];
	unsigned char unused[81]
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

union BlockBuffer{
	struct RecBuffer recordbuffer;
	struct IndBuffer indexbufferr;
	struct RelationCache relcache;
};

struct OpenRelTable{
	char rel_name[ATTR_SIZE];
	struct AttributeCache *attr_list;
	// Block num and slot num of corresponding entry in RELCAT.
	int block_num;
	int slot_num;
};



class Buffer{
	public:
		union BlockBuffer buffer[32];
		struct OpenRelTable rel_table[8];
		//struct RelationCache rel_cache[2];
	private:
		unsigned char block_alloc_map[DISK_BLOCKS];
		bool alloc_map_dirty[4];

		bool free[32];
		bool dirty[32];
		int block[32];


		int getBlockType(int buffer_index){
			int block_type = *(int *)&buffer[buffer_index];
			return block_type;
		}

		int getBufferIndex(int block_num){
			int buffer_index = -1;
			for(int iter=0; iter<32; iter++){
				if(block[iter]==block_num){
					buffer_index = iter;
					break;
				}
			}
			return buffer_index;
		}

	public:
		// returns the starting address of slotmap
		unsigned char *getSlotmap(int block_num){
			int buffer_index = getBufferIndex(block_num);
			unsigned char *slotmap = (unsigned char *)((&buffer[buffer_index]) + 32);
			return slotmap;
		}

		// returns the starting address of ith(slot) record
		union Attribute *getRecord(int block_num, int slot, int num_of_attrib, int num_of_slots){
			int buffer_index = getBufferIndex(block_num);
			int block_type = getBlockType(buffer_index);
			if (block_type != REC){
				return NULL;
			}
			struct RecBuffer *rec_blk = (struct RecBuffer *)&buffer[buffer_index];
			union Attribute *record = (union Attribute *)(rec_blk->data + num_of_slots + slot * num_of_attrib * ATTR_SIZE);
			return record;
		} 

		union Attribute *getAttribute(int block_num, int slot, int num_of_attrib, int num_of_slots, int attr_offset){
			union Attribute *record = getRecord(block_num, slot, num_of_attrib, num_of_slots);
			union Attribute *attribute = (record + attr_offset * ATTR_SIZE);
			return attribute;
		}

		int getFreeBlock(){
			int block_num = -1;
			for(int iter = 0; iter < DISK_BLOCKS; iter++){
				if(block_alloc_map[iter] == UNUSED){
					block_alloc_map[iter] = ALLOTED;
					alloc_map_dirty[iter/BLOCK_SIZE] = TRUE;
					block_num = iter;
					break;
				}
			}
			return block_num;
		}

		void releaseBlock(int block_num){
			if(block_num < 0 || block_num >= DISK_BLOCKS){
				return;  
			}

			block_alloc_map[block_num] = UNUSED;
			alloc_map_dirty[block_num/BLOCK_SIZE] = TRUE;
			return;
		}

		struct RecBuffer *getRecordBlock(int block_num){
			int buffer_index = getBufferIndex(block_num);
			if(buffer_index == -1){
				buffer_index = getFreeBuffer();
				if(buffer_index != -1){
					block[buffer_index] = block_num;
					readblock(&buffer[buffer_index],block_num);
				}
				else{
					return NULL;
				}
			}

			int blktype = getBlockType(buffer_index);
			if(blktype != REC){
				return NULL;
			}

			return (RecBuffer *) &buffer[buffer_index];
		}

		struct IndBuffer *getIndexBlock(int block_num){
			int buffer_index = getBufferIndex(block_num);
			if(buffer_index == -1){
				buffer_index = getFreeBuffer();
				if(buffer_index != -1){
					block[buffer_index] = block_num;
					readblock(&buffer[buffer_index],block_num);
				}
				else{
					return NULL;
				}
			}

			int blktype = getBlockType(buffer_index);
			if(blktype != IND){
				return NULL;
			}

			return (IndBuffer *) &buffer[buffer_index];
		}
};