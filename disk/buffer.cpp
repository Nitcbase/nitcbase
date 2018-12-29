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
};

class BlockBuffer{
private:
	char *data_ptr;
	struct BufferMetaInfo *meta_ptr;

public:
	int getBlockType(){

	}

	int getPblock(){

	}

	int getLblock(){

	}

	int getRblock(){

	}

	int getNumEntries(){

	}

	void setPblock(int block_num){

	}

	void setLblock(int block_num){

	}

	void setRblock(int block_num){

	}

	void setNumEntries(int num){

	}
};

class RecBuffer : public BlockBuffer{
public:
	char *getSlotmap(){

	}

	union Attribute *getRecord(int slot_num, int num_of_attrib, int num_of_slots){

	}

	union Attribute *getAttribute(int slot_num, int num_of_attrib, int num_of_slots, int attr_offset){

	}

};

class IndBuffer : public BlockBuffer{
public:
	struct Index *getIndex(int index_num){

	}

	int *getChild(int child_num){

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

public:
	/*Block related functions*/
	class RecBuffer *getFreeRecBlock(){

	}

	class IndBuffer *getFreeIndBlock(){

	}

	class RecBuffer *getRecBlock(int block_num){

	}

	class IndBuffer *getIndBlock(int block_num){

	}

	void releaseBlock(int block_num){

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

	struct AttributeCache *getAttrCache(int rel_id){

	}

	void setAttrCache(struct AttributeCache* attr_list){

	}

	void setNumRec(int rel_id, int num_rec){

	}

	void setFirstBlk(int rel_id, int first_blk){

	}


};