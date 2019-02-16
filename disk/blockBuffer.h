#include "../define/constants.h"
#include <cstdint>

union Attribute{
	int ival;
	float fval;
	char strval[ATTR_SIZE];
};

struct Index{
	int32_t lchild;
	union Attribute attrval;
	int32_t block;
	int32_t slot;
	unsigned char unused[8];
	int32_t rchild;
};

struct HeadInfo{
	int32_t block_type;
	int32_t pblock;
	int32_t lblock;
	int32_t rblock;
	int32_t num_entries;
	int32_t num_attrs; //not useful for index block
	int32_t num_slots;
	unsigned char reserved[4];
};

class BlockBuffer{
protected:
	int block_num;
	class Buffer *buff_ptr; //May remove this based on decision taken.
	unsigned char * get_dataptr();

public:
	BlockBuffer(int blk_no, class Buffer *buff); //Arg2 will be removed.
	struct HeadInfo getheader();
	void setheader(struct HeadInfo head); //better to change return type to int for indicating errors(if any)
	/* Making destructor virtual to allow dynamic_cast.
	 * Refer to following link for more info:
	 * https://stackoverflow.com/questions/15114093/getting-source-type-is-not-polymorphic-when-trying-to-use-dynamic-cast
	 */
	virtual ~BlockBuffer();
};

class RecBuffer : public BlockBuffer{
public:
	RecBuffer(int blk_no, class Buffer *buff);
	void getSlotmap(unsigned char *slotmap); //change return type to to int = slotmapsize
	void setSlotmap(unsigned char *slotmap);
	void getRecord(union Attribute *rec,int slot_num);
	void setRecord(union Attribute *rec,int slot_num);
};

class IndBuffer : public BlockBuffer{
public:
	IndBuffer(int blk_no, class Buffer *buff);
	struct Index getIndexval(int index_num);
	void setIndexval(struct Index IndexEntry,int index_num);
};

