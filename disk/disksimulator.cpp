#include<iostream>
#include "../define/constants.h"

using namespace std;

void createdisk(){
	//Creates an empty disk
	FILE *disk = fopen("disk","wb+");
	int offset = 16*1024*1024; //16MB
	fseek(disk, 0, SEEK_SET);
	
	for(int i=0;i<offset;i++){
		fputc(0,disk);
	}
	
	fclose(disk);
	
}

void formatdisk(){
	//formats the disk and set the reserved entries in block allocation map 
	FILE *disk = fopen("disk","wb+");
	int reserved = 6,offset = 16*1024*1024;
	
	fseek(disk, 0, SEEK_SET);
	
	//first five entries are set to one
	for(int i=0;i<reserved;i++){
		fputc(1,disk);
	}
	
	for(int i=reserved;i<offset;i++){
		fputc(0,disk);
	}	
	
	fclose(disk);

}

int readblock(void *buffer, int block_no){//args 1.disk block no, 2.memory buffer.
	FILE *disk=fopen("disk","rb");
	int offset = block_no * BLOCK_SIZE;
	fseek(disk, offset, SEEK_SET);
	fread(buffer, BLOCK_SIZE, 1, disk);
	fclose(disk);
} 

int writeblock(int block_no, void *buffer){
	FILE *disk=fopen("disk","wb");
	int offset = block_no * BLOCK_SIZE;
	fseek(disk, offset, SEEK_SET);
	fwrite(buffer, BLOCK_SIZE, 1, disk);
	fclose(disk);
}

int main(){
	createdisk();
	return 0;
}