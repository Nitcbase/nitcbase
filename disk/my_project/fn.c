/* CSV file format
   1st line should be the name of attributes
   data type of 2nd line is taken as datatype of all attributes
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define BLOCK_SIZE 2048
int count=0;
void attrcat(char *rel_name,char attr_name[][16],char attr_type[][16],int count,char* root_block,char* primary_flag);
int check_type(char *data)
{	
	int count_int=0,count_dot=0,count_string=0,i;
	for(i=0;data[i]!='\0';i++)
	{
		
		if(data[i]>='0'&&data[i]<='9')
			count_int++;
		if(data[i]=='.')
			count_dot++;
		else
			count_string++;
	}
	//printf("%d:%d:%d:%d",count_int,count_dot,count_string,i);
	if(count_dot==1&&count_int==(strlen(data)-1))
		return 0;
	if(count_int==strlen(data))
		return 1;
	else
		return 2;
}
void parse()
{
     FILE *file=fopen("sample.csv","r");
     char *attr=malloc(sizeof(char));
     int len=1;
     char ch;
     int count=1;
     while((ch = fgetc(file)) != '\n') 
     {
 	if(ch==',')
		count++;
        attr[len-1]=ch;
        len++;
        attr=realloc(attr,(len)*sizeof(char));
    }

     attr[len-1]='\0';
     int i=0,j,k;
    // printf("%s\n",attr);

       char attribute[count][16];
       j=0;
	while(j<count)
	{ 	k=0;
       		while(((attr[i]!=',')&&(attr[i]!='\0'))&&(k<16))
		{
	    		attribute[j][k++]=attr[i++];
		}
		attribute[j][k]='\0';
		j++;i++;
	}
	i=0;
	

	 char *attr_type=malloc(sizeof(char));
	len=1;
	while((ch = fgetc(file)) != '\n') 
	 {
        		 attr_type[len-1]=ch;
       		 len++;
       		 attr_type=realloc(attr_type,(len)*sizeof(char));
    	}
    	 attr_type[len-1]='\0';
    	 i=0;
    	// printf("%s\n",attr_type);

       	char attribute_type[count][16];
    	j=0;
	while(j<count)
	{ 	k=0;
       		while(((attr_type[i]!=',')&&(attr_type[i]!='\0'))&&(k<16))
		{
	    		attribute_type[j][k++]=attr_type[i++];
		}
		attribute_type[j][k]='\0';
		int t=check_type(attribute_type[j]);
		//printf("t%d\n",t);
		if(t==1)
			strcpy(attribute_type[j],"FLT\0");
		if(t==1)
			strcpy(attribute_type[j],"INT\0");
		if(t==2)
			strcpy(attribute_type[j],"STR\0");
		j++;i++;
	}
	i=0;
	while(i<count)
	{
		printf("%s: ",attribute[i]);
		printf("%s\n",attribute_type[i++]);
		int offset=i;
	}
	attrcat("RELATION1",attribute,attribute_type,count,"-1","-1");
           printf("%d",count);
	fclose(file);
 }

void createdisk()
{
	//Creates an empty disk
	FILE *disk = fopen("disk.txt","w+");
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
	FILE *disk = fopen("disk.txt","w+");
	int reserved = 6,offset = 8192*2048;
	fseek(disk, 0, SEEK_SET);
	//first five entries are set to one
	for(int i=0;i<reserved;i++)
	{
		fputc('1',disk);
	}
	for(int i=reserved;i<offset;i++)
	{
		fputc('0',disk);
	}	
	fclose(disk);
}

int readblock(void *buffer, int block_no)
{
	//args 1.disk block no, 2.memory buffer.
	FILE *disk=fopen("disk.txt","r");
	int offset = block_no * BLOCK_SIZE;
	fseek(disk, offset, SEEK_SET);
	fread(buffer, BLOCK_SIZE, 1, disk);
	fclose(disk);
} 

int writeblock(int block_no, void *buffer)
{
	FILE *disk=fopen("disk.txt","w+");
	int offset = block_no * BLOCK_SIZE;
	fseek(disk, offset, SEEK_SET);
	fwrite(buffer, BLOCK_SIZE, 1, disk);
	fclose(disk);
}

int getfreeblock()
{
      FILE *disk=fopen("disk.txt","r");
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
	fclose(disk);
      return i;
}  
 
void meta()
{
	FILE *disk = fopen("disk.txt","r+");
	fseek(disk,4*2048, SEEK_SET);
	fputs("REC",disk);
	fputc('\0',disk);
		
	fseek(disk,4*2048+4, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
		
	fseek(disk,4*2048+8, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,4*2048+12, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,4*2048+16, SEEK_SET);	
	fputs("2",disk);
	fputc('\0',disk);

	fseek(disk,4*2048+20, SEEK_SET);	
	fputs("6",disk);
	fputc('\0',disk);
	fseek(disk,4*2048+24, SEEK_SET);	
	fputs("20",disk);
	fputc('\0',disk);
	fseek(disk,4*2048+32, SEEK_SET);	
	fputs("11",disk);
	fputc('\0',disk);


	fseek(disk,4*2048+52, SEEK_SET);	
	fputs("RELATION_CATALOG",disk);
	fputc('\0',disk);
	fseek(disk,4*2048+68, SEEK_SET);	
	fputs("6",disk);
	fputc('\0',disk);
	fseek(disk,4*2048+84, SEEK_SET);	
	fputs("2",disk);
	fputc('\0',disk);
	fseek(disk,4*2048+100, SEEK_SET);	
	fputs("4",disk);
	fputc('\0',disk);
	fseek(disk,4*2048+116, SEEK_SET);	
	fputs("4",disk);
	fputc('\0',disk);
	fseek(disk,4*2048+116, SEEK_SET);	
	fputs("16",disk);
	fputc('\0',disk);

	fseek(disk,4*2048+148, SEEK_SET);	
	fputs("ATTRIBUTECATALOG",disk);
	fputc('\0',disk);
	fseek(disk,4*2048+164, SEEK_SET);	
	fputs("6",disk);
	fputc('\0',disk);
	fseek(disk,4*2048+180, SEEK_SET);	
	fputs("12",disk);
	fputc('\0',disk);
	fseek(disk,4*2048+196, SEEK_SET);	
	fputs("5",disk);
	fputc('\0',disk);
	fseek(disk,4*2048+212, SEEK_SET);	
	fputs("5",disk);
	fputc('\0',disk);
	fseek(disk,4*2048+228, SEEK_SET);	
	fputs("20",disk);
	fputc('\0',disk);

	fseek(disk,5*2048, SEEK_SET);
	fputs("REC",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+4, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+8, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+12, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+16, SEEK_SET);	
	fputs("12",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+20, SEEK_SET);	
	fputs("6",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+24, SEEK_SET);	
	fputs("20",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+32, SEEK_SET);	
	fputs("111111111111",disk);
	fputc('\0',disk);

	fseek(disk,5*2048+52, SEEK_SET);	
	fputs("RELATION_CATALOG",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+52+16, SEEK_SET);	
	fputs("RELATION_NAME",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+52+16*2, SEEK_SET);	
	fputs("STR",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+52+16*3, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+52+16*4, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+52+16*5, SEEK_SET);	
	fputs("0",disk);
	fputc('\0',disk);

	fseek(disk,5*2048+148, SEEK_SET);	
	fputs("RELATION_CATALOG",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+148+16, SEEK_SET);	
	fputs("#ATTRIBUTES",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+148+16*2, SEEK_SET);	
	fputs("INT",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+148+16*3, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+148+16*4, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+148+16*5, SEEK_SET);	
	fputs("1",disk);
	fputc('\0',disk);

	fseek(disk,5*2048+244, SEEK_SET);	
	fputs("RELATION_CATALOG",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+244+16, SEEK_SET);	
	fputs("#RECORDS",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+244+16*2, SEEK_SET);	
	fputs("INT",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+244+16*3, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+244+16*4, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+244+16*5, SEEK_SET);	
	fputs("2",disk);
	fputc('\0',disk);

	fseek(disk,5*2048+340, SEEK_SET);	
	fputs("RELATION_CATALOG",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+340+16, SEEK_SET);	
	fputs("FIRSTBLOCK",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+340+16*2, SEEK_SET);	
	fputs("INT",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+340+16*3, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+340+16*4, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+340+16*5, SEEK_SET);	
	fputs("3",disk);
	fputc('\0',disk);

	fseek(disk,5*2048+436, SEEK_SET);	
	fputs("RELATION_CATALOG",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+436+16, SEEK_SET);	
	fputs("LASTBLOCK",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+436+16*2, SEEK_SET);	
	fputs("INT",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+436+16*3, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+436+16*4, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+436+16*5, SEEK_SET);	
	fputs("4",disk);
	fputc('\0',disk);

	fseek(disk,5*2048+532, SEEK_SET);	
	fputs("RELATION_CATALOG",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+532+16, SEEK_SET);	
	fputs("#SLOTS",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+532+16*2, SEEK_SET);	
	fputs("INT",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+532+16*3, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+532+16*4, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+532+16*5, SEEK_SET);	
	fputs("5",disk);
	fputc('\0',disk);

	fseek(disk,5*2048+628, SEEK_SET);	
	fputs("ATTRIBUTECATALOG",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+628+16, SEEK_SET);	
	fputs("RELATION_NAME",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+628+16*2, SEEK_SET);	
	fputs("STR",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+628+16*3, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+628+16*4, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+628+16*5, SEEK_SET);	
	fputs("0",disk);
	fputc('\0',disk);

	fseek(disk,5*2048+724, SEEK_SET);	
	fputs("ATTRIBUTECATALOG",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+724+16, SEEK_SET);	
	fputs("ATTRIBUTE_NAME",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+724+16*2, SEEK_SET);	
	fputs("STR",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+724+16*3, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+724+16*4, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+724+16*5, SEEK_SET);	
	fputs("1",disk);
	fputc('\0',disk);

	fseek(disk,5*2048+820, SEEK_SET);	
	fputs("ATTRIBUTECATALOG",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+820+16, SEEK_SET);	
	fputs("ATTRIBUTE_TYPE",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+820+16*2, SEEK_SET);	
	fputs("STR",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+820+16*3, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+820+16*4, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+820+16*5, SEEK_SET);	
	fputs("2",disk);
	fputc('\0',disk);

	fseek(disk,5*2048+916, SEEK_SET);	
	fputs("ATTRIBUTECATALOG",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+916+16, SEEK_SET);	
	fputs("PRIMARY_FLAG",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+916+16*2, SEEK_SET);	
	fputs("INT",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+916+16*3, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+916+16*4, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+916+16*5, SEEK_SET);	
	fputs("3",disk);
	fputc('\0',disk);

	fseek(disk,5*2048+1012, SEEK_SET);	
	fputs("ATTRIBUTECATALOG",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+1012+16, SEEK_SET);	
	fputs("ROOT_BLOCK",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+1012+16*2, SEEK_SET);	
	fputs("INT",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+1012+16*3, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+1012+16*4, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+1012+16*5, SEEK_SET);	
	fputs("4",disk);
	fputc('\0',disk);

	fseek(disk,5*2048+1108, SEEK_SET);	
	fputs("ATTRIBUTECATALOG",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+1108+16, SEEK_SET);	
	fputs("OFFSET",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+1108+16*2, SEEK_SET);	
	fputs("INT",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+1108+16*3, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+1108+16*4, SEEK_SET);	
	fputs("-1",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+1108+16*5, SEEK_SET);	
	fputs("5",disk);
	fputc('\0',disk);
	fseek(disk,5*2048+1108+16*5+1, SEEK_SET);
	fputc('\0',disk);
	fclose(disk);
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
	
}*/

void relcat(char *attr_name,int attr,int rec,int first_block,int last_block,int slots)
{
	int free=0;
	FILE *disk = fopen("disk.txt","rw+");
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
	printf("\n");*/
	fclose(disk);
		
} 

void attrcat(char *rel_name,char attr_name[][16],char attr_type[][16],int count,char* root_block,char* primary_flag)
{
	//printf("attribute fn\n");
	int free=0;
	FILE *disk = fopen("disk.txt","r+");
	fseek(disk,4*2048+212, SEEK_SET);
	char temp[5];
	for(int i=0;i<4;i++)
			temp[i]=getc(disk);
	temp[4]='\0';
	int last_block=atoi(temp);
	//printf("%dlast block:\n",last_block);
	fseek(disk,last_block*2048+16, SEEK_SET);
	char ch = getc(disk);

	while(ch!='\0')
	{
		free=free*10+ch-'0';
		ch=getc(disk);
	}
	printf("%d",free);
	int i;
	for(i=0;i<count;i++,free++)
	{
		//printf("hiiii");
		fseek(disk,last_block*2048+(free)*96+52, SEEK_SET);
		fputs(rel_name,disk);
		fputc('\0',disk);
		fseek(disk,last_block*2048+(free)*96+52+16, SEEK_SET);
		fputs(attr_name[i],disk);
		fputc('\0',disk);
		fseek(disk,last_block*2048+(free)*96+52+32, SEEK_SET);
		fputs(attr_type[i],disk);
		fputc('\0',disk);
		fseek(disk,last_block*2048+(free)*96+52+48, SEEK_SET);
		fputs(primary_flag,disk);
		fputc('\0',disk);
		fseek(disk,last_block*2048+(free)*96+52+64, SEEK_SET);
		fputs(root_block,disk);
		fputc('\0',disk);
		fseek(disk,last_block*2048+(free)*96+52+80, SEEK_SET);
		char buf [4];
		sprintf (buf, "%03i", i);
		int digit1, digit2, digit3;
		digit1 = buf [0] - '0';
		digit2 = buf [1] - '0';
		digit3 = buf [2] - '0';
	//******IMPORTANT: OFFSET 0 IS STORED AS 000, USING 3 CHARACTERS******
		fputs(buf,disk);
		fputc('\0',disk);
	}	
	fclose(disk);
}

void read_attr()
{
FILE *disk = fopen("disk.txt","r");
char ch;
printf("\nATTRIBUTE CATALOG\n");
int curr_block=5;
int next_block=5;
int i=0;
do
{
fseek(disk,curr_block*2048+12, SEEK_SET);
char temp[5];
for(int i=0;i<4;i++)
	temp[i]=getc(disk);
temp[4]='\0';
int next_block=atoi(temp);
printf("NNNNN%d\n",next_block);
fseek(disk,curr_block*2048, SEEK_SET);
i=0;

while(i<2048)
{
	/*ch=getc(disk);
	if(ch=='\0')
		//printf("EOS\n");
	else
		//printf("%c\n",ch);*/
	i++;
}
curr_block=next_block;
next_block=fseek(disk,curr_block*2048+12, SEEK_SET);
printf("current block:%d\n",curr_block);
printf("next block:%d\n",next_block);
}while(next_block!=curr_block);
fclose(disk);
}

/*void find_entry(int blkno,int offset)
{
	FILE *disk = fopen("disk.txt","r");
	fseek(disk,blkno*2048+offset, SEEK_SET);
	int free=0;
	char ch = getc(disk);
	while(ch!='|')
	{
		free=free*10+ch-'0';
		ch=getc(disk);
	}
	return free;
}

void export(char *rel_name)
{
	FILE *disk = fopen("disk.txt","r");
	fseek(disk,4*2048+12, SEEK_SET);
	int curr_blk=4;
	char temp[4];
	for(int i=0;i<4;i++)
		temp[i]=getc(disk);
	int next_blk=atoi(temp);

	while(curr_blk!=-1)
	{
		fseek(disk,curr_blk*2048+12, SEEK_SET);
		char temp[5];
		for(int i=0;i<4;i++)
		temp[i]=getc(disk);
		temp[4]='\0';
	   	next_blk=atoi(temp);
		fseek(disk,curr_blk*2048+16, SEEK_SET);
		for(int i=0;i<4;i++)
			temp[i]=getc(disk);
		temp[4]='\0';
	   	no_entries=atoi(temp);
		for(i=0;i<no_entries;i++)
		{
			fseek(disk,curr_blk*2048+i*96+52, SEEK_SET);
			char t[17];
			char ch;
			for(int j=0;j<16&&(ch=getc(disk))!='|';j++)
				t[j]=ch;
			t[j]='\0';
			if(strcmp(t,rel_name)==0)
				break;
		}
		curr_blk=next_blk;
	}
   	if(curr_blk==-1)
	{
		printf("RELATION DOES NOT EXIST!!\n");
		return;
	}

	int no_attributes,no_records,f_blk,l_blk,no_slots;
	for(int m=0;m<5;m++)
	{
		fseek(disk,curr_blk*2048+i*96+52+16*(m+1), SEEK_SET);
		for(int j=0;j<16&&(ch=getc(disk))!='|';j++)
			t[j]=ch;
		t[j]='\0';
		
		if(m==0)
			no_attributes=atoi(t);
		if(m==1)
			no_records=atoi(t);
		if(m==2)
			f_blk=atoi(t);
		if(m==3)
			l_blk=atoi(t);
		if(m==4)
			no_slots=atoi(t);
	}
	

}*/

int main()
{
	createdisk();
	formatdisk();
	meta();
	parse();
 	read_attr();
	//printf("%d",check_type("11111"));
	//printf("%d",check_type("1.111"));
	/*printf("%d",check_type("1.11a"));
	printf("%d",check_type("1..1.1.1"));
	printf("%d",check_type("abcd_"));
	printf("%d",check_type("abcd12"));
	relcat("TABLE\0","Name,Mark,Grade",3);*/
	//fseek(disk,4*2048+16, SEEK_SET);
	//char ch = getc(disk);
	//printf("hiii%c",ch);
	//printf("%d",count);
          //getfreeblock();
	return 0;
}
