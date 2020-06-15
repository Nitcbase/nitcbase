
#include "../define/constants.h"
#include "disk_structure.h"


void createdisk()
{
	//Creates an empty disk
	FILE *disk = fopen("disk","wb+");
	int offset = 16*1024*1024; //16MB
	fseek(disk, 0, SEEK_SET);
	
	for(int i=0;i<offset;i++)
          {
		fputc(0,disk);
	}	
	fclose(disk);
	
}

void formatdisk(){
	//formats the disk and set the reserved entries in block allocation map 
	FILE *disk = fopen("disk","wb+");
	int reserved = 6,offset = 16*1024*1024;
	
	fseek(disk, 0, SEEK_SET);
	unsigned char ch[2048*4];
	//first five entries are set to one
	for(int i=0;i<reserved;i++)
	{
		if(i>=0&&i<=3)
			ch[i]=(unsigned char)1;
		else
			ch[i]=(unsigned char)REC;
		
	}
	for(int i=reserved;i<2048*4;i++)
		ch[i]=(unsigned char)UNUSED_BLK;
	fwrite(ch,2048*4,1,disk);
	for(int i=reserved;i<offset;i++)
	{
		fputc(0,disk);
	}		
	fclose(disk);
}


int readblock(void *buffer, int block_no)
{         //args 1.disk block no, 2.memory buffer.
	FILE *disk=fopen("disk","rb");
	int offset = block_no * BLOCK_SIZE;
	fseek(disk, offset, SEEK_SET);
	fread(buffer, BLOCK_SIZE, 1, disk);
	fclose(disk);
} 

int writeblock(int block_no, void *buffer)
{
	FILE *disk=fopen("disk","wb");
	int offset = block_no * BLOCK_SIZE;
	fseek(disk, offset, SEEK_SET);
	fwrite(buffer, BLOCK_SIZE, 1, disk);
	fclose(disk);
}

