#include "../define/constants.h"
#include <cstdint>

struct BufferMetaInfo {
	bool free;
	bool dirty;
	int blockNum;
	int timeStamp;
};

class StaticBuffer {
				
friend class BlockBuffer;

private:
	//fields
	static unsigned char blocks[32][BLOCK_SIZE];
	static struct BufferMetaInfo metainfo[32];
	static unsigned char blockAllocMap[DISK_BLOCKS];

	//methods
	static int getFreeBuffer(int blockNum);
	static int getBufferNum(int blockNum);
	
public:
	//methods	
	static int getStaticBlockType(int blockNum);
	StaticBuffer();
	~StaticBuffer();
	
};
