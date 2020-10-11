#include "../define/constants.h"
#include <bits/stdc++.h>
//#include "blockBuffer.h"
#include "physical.h"

struct BufferMetaInfo {
	bool free;
	bool dirty;
	int blockNum;
	int timeStamp;
};

class StaticBuffer {
				
friend class BlockBuffer;

public:
	//methods	
	static int getStaticBlockType(int blockNum);
	static void setDirtyBit(int blockNum);
	
	static void dispMetaInfo();
	static void dispblockAllocMap();
	StaticBuffer();
	~StaticBuffer();
	
private:
	//fields
	 static unsigned char blocks[32][BLOCK_SIZE];
	 static struct BufferMetaInfo metainfo[32];
 	 static unsigned char blockAllocMap[DISK_BLOCKS];

	//methods
	 static int getFreeBuffer(int blockNum);
	 static int getBufferNum(int blockNum);
	
	
};

unsigned char StaticBuffer:: blocks[32][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer:: metainfo[32];
unsigned char StaticBuffer::blockAllocMap[DISK_BLOCKS]={myUNUSED};

StaticBuffer::StaticBuffer(){
    // copy blockAllocMap blocks from disk to buffer (using readblock() of disk)
    
    //cout <<"Inside StaticBuffer() constructor\n";
    for(int it = 0;it<4;it++)
    {
      Disk::readBlock(blockAllocMap+it*BLOCK_SIZE, it);
    }

    //initialise metainfo of all the buffer blocks with dirty:false, free:true, timestamp:-1 and blockNum:-1
    for(int it = 0;it<32;it++)
    {
      metainfo[it].dirty = false;
      metainfo[it].free = true;
      metainfo[it].timeStamp = -1;
      metainfo[it].blockNum = -1;
    }
}

StaticBuffer::~StaticBuffer(){
    // copy blockAllocMap blocks from buffer to disk(using writeblock() of disk)
    //NOTE : 2nd param should be explicitly type casted to void*
    
    //cout <<"inside static buffer destructor\n";
    for(int it = 0;it<4;it++)
    {
      Disk::writeBlock(it, (void*)(blockAllocMap+it*BLOCK_SIZE));
    }

    /*iterate through all the buffer blocks, 
    	write back blocks with meta info as free:false,dirty:true (using writeblock() of disk)*/
    for(int it = 0;it<32;it++)
    {
      if(metainfo[it].free == false && metainfo[it].dirty == true)
          Disk::writeBlock(metainfo[it].blockNum, (void*)blocks[it]);
    }
}

int StaticBuffer::getFreeBuffer(int blockNum){

	//cout <<"Entering getFreeBuffer\n";
	
    // increase the time stamps in metainfo of all the occupied buffers.
    for(int it = 0;it<32;it++)
    {
      if(metainfo[it].free == false)
        metainfo[it].timeStamp += 1;
    }
    
    // if free buffer is available, bufferIndex is the index of the free buffer.
    int bufferIndex = -1;
    for(int it = 0;it<32;it++)
    {
      if(metainfo[it].free == true){
          bufferIndex = it;
          break;
      }
    }
    
    // if free buffer is not available, replace the buffer with the largest timestamp and set it as bufferIndex.
    if(bufferIndex == -1){
        int largest_timestamp = -1;
        int largest_index = -1;
        for(int it = 0;it < 32;it++)
        {
            if(metainfo[it].timeStamp > largest_timestamp)
            {
              largest_timestamp =  metainfo[it].timeStamp;
              largest_index = it;
            }
        }
        Disk::writeBlock(metainfo[largest_index].blockNum, (void*)blocks[largest_index]);
        bufferIndex = largest_index;
        //cout<<"\nwritten back\n\n";
    }
  
    
    // update the metainfo array corresponding to the buffer index. 
    metainfo[bufferIndex].dirty = false;
    metainfo[bufferIndex].free = false;
    metainfo[bufferIndex].timeStamp = 0;
    metainfo[bufferIndex].blockNum = blockNum;

	//cout <<"Exiting getFreeBuffer with buffer index "<<bufferIndex<<"\n";
    
    // return the buffer index
    return bufferIndex;
}


  int StaticBuffer::getBufferNum(int blockNum){
  	//cout <<"Entering getBufferNum with blockNum "<<blockNum<<"\n";
    //traverse through the metainfo array, find the buffer index of the buffer to which the block is loaded.
     int bufferIndex = -1;
     for(int it = 0;it < 32;it++)
     {
        if(metainfo[it].blockNum == blockNum)
        {
            bufferIndex = it;
            break;
        }
     }
    //cout <<"Exiting getBufferNum with bufferNum "<<bufferIndex<<"\n";
    //if found return buffer index, else indicate failure.
    if(bufferIndex == -1)
         return FAILURE;
     
    return bufferIndex;
}


int StaticBuffer::getStaticBlockType(int blockNum){
    //traverse the blockAllocMap to find the type corresponding to blockNum.
    //doubt
    
    //return the blockType obtained(REC/IND_INTERNAL/IND_LEAF/UNUSED)
    return (int)blockAllocMap[blockNum];
}

void StaticBuffer::setDirtyBit(int blockNum){
	//cout <<"Entering setDirtyBit\n";
    int bufferNum = getBufferNum(blockNum);
    metainfo[bufferNum].dirty = true;
    //cout <<"Exiting setDirtyBit\n";
}

void StaticBuffer::dispMetaInfo(){
	cout <<"|"<<setw(16)<<"Buffer|"<<setw(16)<<"Free|"<<setw(16)<<"Dirty|"<<setw(16)<<"blockNum|"<<setw(16)<<"timeStamp|\n";
	cout <<"+---------------+---------------+---------------+---------------+\n";
	for(int i =0;i<32;++i) {
		cout<<"|"<<setw(15)<<i<<"|"<<setw(15)<< metainfo[i].free<<"|"<<setw(15)<<metainfo[i].dirty<<"|"<<setw(15)<<metainfo[i].blockNum<<"|"<<setw(15)<<metainfo[i].timeStamp<<"|\n";
	}
}

void StaticBuffer::dispblockAllocMap(){
	for(int i =0;i< DISK_BLOCKS;++i){
		cout << (int)StaticBuffer::blockAllocMap[i]<<" ";
	}
	cout <<endl;
}
