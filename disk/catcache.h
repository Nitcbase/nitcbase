#include "../define/constants.h"

struct AttributeCache{
	char rel_name[ATTR_SIZE];
	char attr_name[ATTR_SIZE];
	int attr_type;
	bool primary_flag;
	int root_block;
	int offset;
	struct AttributeCache *next;
	int sblock, sindex; /*IMPORTANT: used for storing state information of any search.
									 which will be used for getting next hit. */
};

struct OpenRelTableEntry{
	char relname[ATTR_SIZE];
	int num_attr;
	int num_rec;
	int first_blk;
	int num_slots_blk;
	struct AttributeCache *attr_list;
	bool free, dirty;
	int block, slot;
};

//Still evolving ;-)
class OpenRelTable{

};