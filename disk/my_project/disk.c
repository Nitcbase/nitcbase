#include<stdio.h>
#define BLOCK_SIZE 2048
int count=0;

void createdisk()
{
	//Creates an empty disk
	FILE *disk = fopen("disk.txt","wb+");
	int offset = 8192*2048; //16MB
	fseek(disk,0, SEEK_SET);
	for(int i=0;i<offset;i++)
	{
		fputc('0',disk);
		count++;
	}
	fclose(disk);
}

void formatdisk()
{
	//formats the disk and set the reserved entries in block allocation map 
	FILE *disk = fopen("disk","wb+");
	int reserved = 6,offset = 8192*2048;
	fseek(disk, 0, SEEK_SET);
	
	//first five entries are set to one
	for(int i=0;i<reserved;i++)
	{
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
	FILE *disk=fopen("disk","wb+");
	int offset = block_no * BLOCK_SIZE;
	fseek(disk, offset, SEEK_SET);
	fwrite(buffer, BLOCK_SIZE, 1, disk);
	fclose(disk);
}

int getfreeblock()
{
      FILE *disk=fopen("disk","rb");
      fseek(disk,0, SEEK_SET);
      char ch;
      int i=0;
      while ((ch = fgetc(disk)) != EOF)
        {
            if(ch=='0')
             break;
 	  i++;
        }
      printf("\n%d",i);
      return i;
}  
 
void meta()
{

	FILE *disk = fopen("disk","wb+");
	fseek(disk,4*2048, SEEK_SET);
	fputs("REC\0",disk);
		
	fseek(disk,4*2048+4, SEEK_SET);	
	fputs("-1\0",disk);
		
	fseek(disk,4*2048+8, SEEK_SET);	
	fputs("-1\0",disk);
	fseek(disk,4*2048+12, SEEK_SET);	
	fputs("-1\0",disk);
	fseek(disk,4*2048+16, SEEK_SET);	
	fputs("2\0",disk);

	fseek(disk,4*2048+20, SEEK_SET);	
	fputs("6\0",disk);
	fseek(disk,4*2048+24, SEEK_SET);	
	fputs("20\0",disk);
	fseek(disk,4*2048+32, SEEK_SET);	
	fputs("11\0",disk);


	fseek(disk,4*2048+52, SEEK_SET);	
	fputs("RELATION_CATALOG",disk);
	fseek(disk,4*2048+68, SEEK_SET);	
	fputs("6\0",disk);
	fseek(disk,4*2048+84, SEEK_SET);	
	fputs("2\0",disk);
	fseek(disk,4*2048+100, SEEK_SET);	
	fputs("4\0",disk);
	fseek(disk,4*2048+116, SEEK_SET);	
	fputs("4\0",disk);
	fseek(disk,4*2048+116, SEEK_SET);	
	fputs("16\0",disk);

	fseek(disk,4*2048+148, SEEK_SET);	
	fputs("ATTRIBUTECATALOG",disk);
	fseek(disk,4*2048+164, SEEK_SET);	
	fputs("6\0",disk);
	fseek(disk,4*2048+180, SEEK_SET);	
	fputs("12\0",disk);
	fseek(disk,4*2048+196, SEEK_SET);	
	fputs("5\0",disk);
	fseek(disk,4*2048+212, SEEK_SET);	
	fputs("5\0",disk);
	fseek(disk,4*2048+228, SEEK_SET);	
	fputs("20\0",disk);

	fseek(disk,5*2048, SEEK_SET);
	fputs("REC\0",disk);
	fseek(disk,5*2048+4, SEEK_SET);	
	fputs("-1\0",disk);
	fseek(disk,5*2048+8, SEEK_SET);	
	fputs("-1\0",disk);
	fseek(disk,5*2048+12, SEEK_SET);	
	fputs("-1\0",disk);
	fseek(disk,5*2048+16, SEEK_SET);	
	fputs("12\0",disk);
	fseek(disk,5*2048+20, SEEK_SET);	
	fputs("6\0",disk);
	fseek(disk,5*2048+24, SEEK_SET);	
	fputs("20\0",disk);
	fseek(disk,5*2048+32, SEEK_SET);	
	fputs("111111111111\0",disk);

	fseek(disk,5*2048+52, SEEK_SET);	
	fputs("RELATION_CATALOG",disk);
	fseek(disk,5*2048+52+16, SEEK_SET);	
	fputs("RELATION_NAME\0",disk);
	fseek(disk,4*2048+52+16*2, SEEK_SET);	
	fputs("STR\0",disk);
	fseek(disk,4*2048+52+16*3, SEEK_SET);	
	fputs("-1",disk);
	fseek(disk,4*2048+52+16*4, SEEK_SET);	
	fputs("-1\0",disk);
	fseek(disk,4*2048+52+16*5, SEEK_SET);	
	fputs("0\0",disk);

	fseek(disk,5*2048+148, SEEK_SET);	
	fputs("RELATION_CATALOG",disk);
	fseek(disk,5*2048+148+16, SEEK_SET);	
	fputs("#ATTRIBUTES\0",disk);
	fseek(disk,4*2048+148+16*2, SEEK_SET);	
	fputs("INT\0",disk);
	fseek(disk,4*2048+148+16*3, SEEK_SET);	
	fputs("-1",disk);
	fseek(disk,4*2048+148+16*4, SEEK_SET);	
	fputs("-1\0",disk);
	fseek(disk,4*2048+148+16*5, SEEK_SET);	
	fputs("1\0",disk);

	fseek(disk,5*2048+244, SEEK_SET);	
	fputs("RELATION_CATALOG",disk);
	fseek(disk,5*2048+244+16, SEEK_SET);	
	fputs("#RECORDS\0",disk);
	fseek(disk,4*2048+244+16*2, SEEK_SET);	
	fputs("INT\0",disk);
	fseek(disk,4*2048+244+16*3, SEEK_SET);	
	fputs("-1",disk);
	fseek(disk,4*2048+244+16*4, SEEK_SET);	
	fputs("-1\0",disk);
	fseek(disk,4*2048+244+16*5, SEEK_SET);	
	fputs("2\0",disk);

	fseek(disk,5*2048+340, SEEK_SET);	
	fputs("RELATION_CATALOG",disk);
	fseek(disk,5*2048+340+16, SEEK_SET);	
	fputs("FIRSTBLOCK\0",disk);
	fseek(disk,4*2048+340+16*2, SEEK_SET);	
	fputs("INT\0",disk);
	fseek(disk,4*2048+340+16*3, SEEK_SET);	
	fputs("-1",disk);
	fseek(disk,4*2048+340+16*4, SEEK_SET);	
	fputs("-1\0",disk);
	fseek(disk,4*2048+340+16*5, SEEK_SET);	
	fputs("3\0",disk);

	fseek(disk,5*2048+436, SEEK_SET);	
	fputs("RELATION_CATALOG",disk);
	fseek(disk,5*2048+436+16, SEEK_SET);	
	fputs("LASTBLOCK\0",disk);
	fseek(disk,4*2048+436+16*2, SEEK_SET);	
	fputs("INT\0",disk);
	fseek(disk,4*2048+436+16*3, SEEK_SET);	
	fputs("-1",disk);
	fseek(disk,4*2048+436+16*4, SEEK_SET);	
	fputs("-1\0",disk);
	fseek(disk,4*2048+436+16*5, SEEK_SET);	
	fputs("4\0",disk);

	fseek(disk,5*2048+532, SEEK_SET);	
	fputs("RELATION_CATALOG",disk);
	fseek(disk,5*2048+532+16, SEEK_SET);	
	fputs("#SLOTS\0",disk);
	fseek(disk,4*2048+532+16*2, SEEK_SET);	
	fputs("INT\0",disk);
	fseek(disk,4*2048+532+16*3, SEEK_SET);	
	fputs("-1",disk);
	fseek(disk,4*2048+532+16*4, SEEK_SET);	
	fputs("-1\0",disk);
	fseek(disk,4*2048+532+16*5, SEEK_SET);	
	fputs("5\0",disk);

	fseek(disk,5*2048+52, SEEK_SET);	
	fputs("ATTRIBUTECATALOG",disk);
	fseek(disk,5*2048+52+16, SEEK_SET);	
	fputs("RELATION_NAME\0",disk);
	fseek(disk,4*2048+52+16*2, SEEK_SET);	
	fputs("STR\0",disk);
	fseek(disk,4*2048+52+16*3, SEEK_SET);	
	fputs("-1",disk);
	fseek(disk,4*2048+52+16*4, SEEK_SET);	
	fputs("-1\0",disk);
	fseek(disk,4*2048+52+16*5, SEEK_SET);	
	fputs("0\0",disk);

	fseek(disk,5*2048+148, SEEK_SET);	
	fputs("ATTRIBUTECATALOG",disk);
	fseek(disk,5*2048+148+16, SEEK_SET);	
	fputs("ATTRIBUTE_NAME\0",disk);
	fseek(disk,4*2048+148+16*2, SEEK_SET);	
	fputs("STR\0",disk);
	fseek(disk,4*2048+148+16*3, SEEK_SET);	
	fputs("-1",disk);
	fseek(disk,4*2048+148+16*4, SEEK_SET);	
	fputs("-1\0",disk);
	fseek(disk,4*2048+148+16*5, SEEK_SET);	
	fputs("1\0",disk);

	fseek(disk,5*2048+244, SEEK_SET);	
	fputs("ATTRIBUTECATALOG",disk);
	fseek(disk,5*2048+244+16, SEEK_SET);	
	fputs("ATTRIBUTE_TYPE\0",disk);
	fseek(disk,4*2048+244+16*2, SEEK_SET);	
	fputs("STR\0",disk);
	fseek(disk,4*2048+244+16*3, SEEK_SET);	
	fputs("-1",disk);
	fseek(disk,4*2048+244+16*4, SEEK_SET);	
	fputs("-1\0",disk);
	fseek(disk,4*2048+244+16*5, SEEK_SET);	
	fputs("2\0",disk);

	fseek(disk,5*2048+340, SEEK_SET);	
	fputs("ATTRIBUTECATALOG",disk);
	fseek(disk,5*2048+340+16, SEEK_SET);	
	fputs("PRIMARY_FLAG\0",disk);
	fseek(disk,4*2048+340+16*2, SEEK_SET);	
	fputs("INT\0",disk);
	fseek(disk,4*2048+340+16*3, SEEK_SET);	
	fputs("-1",disk);
	fseek(disk,4*2048+340+16*4, SEEK_SET);	
	fputs("-1\0",disk);
	fseek(disk,4*2048+340+16*5, SEEK_SET);	
	fputs("3\0",disk);

	fseek(disk,5*2048+436, SEEK_SET);	
	fputs("ATTRIBUTECATALOG",disk);
	fseek(disk,5*2048+436+16, SEEK_SET);	
	fputs("ROOT_BLOCK\0",disk);
	fseek(disk,4*2048+436+16*2, SEEK_SET);	
	fputs("INT\0",disk);
	fseek(disk,4*2048+436+16*3, SEEK_SET);	
	fputs("-1",disk);
	fseek(disk,4*2048+436+16*4, SEEK_SET);	
	fputs("-1\0",disk);
	fseek(disk,4*2048+436+16*5, SEEK_SET);	
	fputs("4\0",disk);

	fseek(disk,5*2048+532, SEEK_SET);	
	fputs("ATTRIBUTECATALOG",disk);
	fseek(disk,5*2048+532+16, SEEK_SET);	
	fputs("OFFSET\0",disk);
	fseek(disk,4*2048+532+16*2, SEEK_SET);	
	fputs("INT\0",disk);
	fseek(disk,4*2048+532+16*3, SEEK_SET);	
	fputs("-1",disk);
	fseek(disk,4*2048+532+16*4, SEEK_SET);	
	fputs("-1\0",disk);
	fseek(disk,4*2048+532+16*5, SEEK_SET);	
	fputs("5\0",disk);
}
/*void fill_block()
{
	char each_attr[16];
       	int i=0,j=0,k;
	while(j<count)
	{ 	k=0;
		fseek(disk,4*2048+(free-1)*96+52+16*(j+1), SEEK_SET);
       		while(((attr[i]!=',')&&(attr[i]!='\0'))&&(k<16))
		{
	    		each_attr[k++]=attr[i++];
		}
		each_attr[k++]='\0';
		j++;i++;
	 	printf("%s\n",each_attr);
		fputs(each_attr,disk);
	}
	
}

void relcat(char *attr_name,int attr,int rec,int first_block,int last_block,int slots)
{
	int free=0;
	FILE *disk = fopen("disk","rw+");
	fseek(disk,4*2048+16, SEEK_SET);
	char ch = getc(disk);
	while(ch!='\0')
	{
		free=free*10+ch-'0';
		ch=getc(disk);
	}
 	//free = no of entries
	fseek(disk,4*2048+(free)*96+52, SEEK_SET);
	fputs(attr_name,disk);
	/*fseek(disk,4*2048+(free)*96+52, SEEK_SET);
	ch=getc(disk);
	while(ch!='\0')
	{
		printf("%c",ch);
		ch=getc(disk);
	}
	printf("\n");
		
} 
*/
int main()
{
	createdisk();
	meta();

	//relcat("TABLE\0","Name,Mark,Grade",3);
	//fseek(disk,4*2048+16, SEEK_SET);
	//char ch = getc(disk);
	//printf("hiii%c",ch);
	printf("%d",count);
          getfreeblock();
	return 0;
}
