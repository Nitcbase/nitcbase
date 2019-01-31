#include "../define/constants.h"

struct BufferMetaInfo {
	bool free;
	bool dirty;
	int block_num;
	class BlockBuffer *blk;	//May remove this.
};

class Buffer {
friend class BlockBuffer;
private:
	//fields
	unsigned char blocks[32][BLOCK_SIZE];
	struct BufferMetaInfo metainfo[32];
	unsigned char block_alloc_map[DISK_BLOCKS];

	//methods
	int getBlockType(int buffer_index);
	int getFreeBuffer();
	int getbufferblock(int block);
	int load_block(int block);
	void releaseBufferBlock(int i);
	unsigned char* get_buf_dataptr(int block_num);

public:
	//methods
	class RecBuffer *getFreeRecBlock();
	class IndBuffer *getFreeIndBlock();
	class RecBuffer *getRecBlock(int block_num);
	class IndBuffer *getIndBlock(int block_num);
	void releaseBlock(int block_num);
};

/* TODO: 
 * 1. There should be a disk object in buffer
 *		-instance of which is created in constructor.
 * 2. Write constructor for Buffer class. (destructor as well).
 * 3. Should write replacement code for blocks.
 * 4. Change releaseBlock interface.
 */