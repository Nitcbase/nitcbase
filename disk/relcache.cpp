
struct AttributeCache{
	char rel_name[ATTR_SIZE];
	char attr_name[ATTR_SIZE];
	int attr_type;
	bool primary_flag;
	int root_block;
	int offset;
	struct AttributeCache *next;
};

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


