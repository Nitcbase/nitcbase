#include "../define/constants.h"

struct BufferMetaInfo {
	bool free;
	bool dirty;
	int block_num;
};

class Buffer {
friend class BlockBuffer;
private:
	//fields
	static unsigned char blocks[32][BLOCK_SIZE];
	static struct BufferMetaInfo metainfo[32];
	static unsigned char block_alloc_map[DISK_BLOCKS];

	//methods
	static int getBlockType(int buffer_index);
	static int getFreeBuffer();
	static int getbufferblock(int block_num);
	static int loadBlock(int block_num);
	static void releaseBufferBlock(int buffer_index);
	static unsigned char* get_buf_dataptr(int block_num);

public:
	//methods
	Buffer();
	~Buffer();
	static class RecBuffer *getFreeRecBlock();
	static class IndBuffer *getFreeIndInternal();
	static class IndBuffer *getFreeIndLeaf();
	static class RecBuffer *getRecBlock(int block_num);
	static class IndBuffer *getIndBlock(int block_num);
	static void deleteBlock(int block_num);
	static void releaseBlock(int block_num);
};

/* TODO: 
 * 1. There should be a disk object in buffer
 *		-instance of which is created in constructor.
 * 2. Write constructor for Buffer class. (destructor as well).
 * 3. Should write replacement code for blocks.
 * 4. Change releaseBlock interface.
 */
