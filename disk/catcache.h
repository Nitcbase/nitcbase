#include "../define/constants.h"
#include "../define/id.h"

typedef struct RelCatEntry{
	char rel_name[ATTR_SIZE];
	int num_attr;
	int num_rec;
	int first_blk;
	int num_slots_blk;
}RelCatEntry ;

typedef struct AttrCatEntry{
	char rel_name[ATTR_SIZE];
	char attr_name[ATTR_SIZE];
	int attr_type;
	bool primary_flag;
	int root_block;
	int offset;
}AttrCatEntry ;

typedef struct AttributeCache{
	AttrCatEntry attrcat_entry;
	bool free, dirty;
	recId rec_id;
	SearchIndexId sid; /*IMPORTANT: used for storing state information of any search.
									 which will be used for getting next hit. */
	struct AttributeCache *next;
} AttributeCache;

typedef struct OpenRelTableEntry{
	RelCatEntry relcat_entry;
	struct AttributeCache *attr_list_head;
	bool free, dirty;
	recId rec_id;
}OpenRelTableEntry;


class OpenRelTable{
private:
	static OpenRelTableEntry RelCache[8];
	static relId getFreeOpenRelTableEntry();
	
public:	
	static relId OpenRel(char rel_name[16]);
	static int CloseRel(relId rel_id);
	static int getRelCatEntry(relId rel_id, RelCatEntry relcat_rec);
	static int setRelCatEntry(relId rel_id, RelCatEntry relcat_rec);
	static int getAttrCatEntry(relId rel_id, char attr_name[16], AttrCatEntry attrcat_rec);
	static int getAttrCatEntry(relId rel_id, int attr_offset, AttrCatEntry attrcat_rec);
	static int setAttrCatEntry(relId rel_id, char attr_name[16], AttrCatEntry attrcat_rec);
	static int setAttrCatEntry(relId rel_id, int attr_offset, AttrCatEntry attrcat_rec);
	static int getSearchIndexId(relId rel_id, char attr_name[16], SearchIndexId sid);
	static int getSearchIndexId(relId rel_id, int attr_offset, SearchIndexId sid);
	static int setSearchIndexId(relId rel_id, char attr_name[16], SearchIndexId sid);
	static int setSearchIndexId(relId rel_id, int attr_offset, SearchIndexId sid);
	static relId getRelId(char rel_name[16]);
};

/*TODO:
Write constructor for OpenRelTable class. (destructor as well).
*/

