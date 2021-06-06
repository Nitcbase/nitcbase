class Disk {			
public:
	Disk()
	{
		FILE *disk-copy = fopen("disk_copy","wb+");
		fseek(disk-copy, 0, SEEK_SET);
		FILE *disk=fopen("disk","rb");
		fseek(disk, 0, SEEK_SET);
		unsigned char ch[BLOCK_SIZE];

		for(int i=0;i<8192;i++)
		{
			fseek(disk,i*BLOCK_SIZE,SEEK_SET);
			fseek(disk-copy,i*BLOCK_SIZE,SEEK_SET);
			fread(ch,BLOCK_SIZE,1,disk);
			fwrite(ch,BLOCK_SIZE,1,disk-copy);
		}
		fclose(disk);
		fclose(disk-copy);
	}

	~Disk()
	{
		FILE *disk-copy = fopen("disk_copy","rb");
		fseek(disk-copy, 0, SEEK_SET);
		FILE *disk=fopen("disk","wb+");
		fseek(disk, 0, SEEK_SET);
		unsigned char ch[BLOCK_SIZE];

		for(int i=0;i<8192;i++)
		{
			fseek(disk,i*BLOCK_SIZE,SEEK_SET);
			fseek(disk-copy,i*BLOCK_SIZE,SEEK_SET);
			fread(ch,BLOCK_SIZE,1,disk-copy);
			fwrite(ch,BLOCK_SIZE,1,disk);
		}
		fclose(disk);
		fclose(disk-copy);
	}

	static int readBlock(unsigned char *block, int blockNum)
  	{
		//args 1.disk block no, 2.memory buffer.
		FILE *disk=fopen("disk_copy","rb");
		int offset = blockNum * BLOCK_SIZE;
		fseek(disk, offset, SEEK_SET);
		fread(block, BLOCK_SIZE, 1, disk);
		fclose(disk);
	}

	static int writeBlock(unsigned char *block, int blockNum)
	{
		FILE *disk=fopen("disk_copy","wb");
		int offset = blockNum * BLOCK_SIZE;
		fseek(disk, offset, SEEK_SET);
		fwrite(block, BLOCK_SIZE, 1, disk);
		fclose(disk);
	}
};
 
