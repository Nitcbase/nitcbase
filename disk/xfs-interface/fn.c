/*TO DO:
type should be stored in block allocation map
REC in header should be replaced by an integer value
relation catalog when new blk is alloted header is not created,no of entries
consider slotmap when allocating space in blocks for entries of relation
sample.csv relation name to sample
export name to sample.txt
rm  slotmap update
    check slotmap instead of no of entries
*/

/*CSV file format
  1st line should be the name of attributes
  data type of 2nd line is taken as datatype of all attributes
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#define BLOCK_SIZE 2048
int count=0;
void attrcat(char *rel_name,char attr_name[][16],char attr_type[][16],int count,char* root_block,char* primary_flag);
void relcat(char *rel_name,int attr,int rec,int first,int last,int slots);
void formatdisk();
void meta();
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


int getfreeblock()
{
      FILE *disk=fopen("disk.txt","r+");
      fseek(disk,0, SEEK_SET);
      char ch;
      int i=0;
      while (i<8192)
       {
	ch=getc(disk);
            if(ch=='0')
             break;
 	  i++;
        }
      printf("\n%d",i);
      fseek(disk,i,SEEK_SET);
      fputc('1',disk);
	fclose(disk);
      return i;
}  
 

int number_of_records(FILE *fp)
{	
	fseek(fp,0,SEEK_SET);
	char ch;int count=0;
	while((ch=getc(fp))!=EOF)
	{
		if(ch=='\n')
			count++;
	}
	return (count-1);
	//where to write fclose??
}

int check_rel_exists(char *filename)
{
	char s[5];
	FILE *disk=fopen("disk.txt","r");
	fseek(disk,4*2048+12,SEEK_SET);
	int curr_blk=4;int next_blk;
	while(curr_blk!=-1)
	{
		fseek(disk,curr_blk*2048+12,SEEK_SET);
		fgets(s,5,disk);
		next_blk=atoi(s);
		int no_of_entries;
	
		fseek(disk,curr_blk*2048+16,SEEK_SET);
		fgets(s,5,disk);
		no_of_entries=atoi(s);
		//printf("%d\n",no_of_entries);
		int i;char rel_name[17];
		for(i=0;i<no_of_entries;i++)
			{
			fseek(disk,curr_blk*2048+52+i*96,SEEK_SET);
			fgets(rel_name,17,disk);
			if(strcmp(rel_name,filename)==0)
				return 1;
			}

		curr_blk=next_blk;
	}
	fclose(disk);
	return 0;
}

void parse(char *filename)
{
     FILE *file=fopen(filename,"r");
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
		if(t==0)
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
	char newfilename[(strlen(filename))-4];
	int loopv=0;
	while(filename[loopv]!='.')
		{
		newfilename[loopv]=filename[loopv];
		loopv++;
		}
	newfilename[loopv]='\0';
	printf("%s",newfilename);
	if(check_rel_exists(newfilename))
		{
			printf("\n RELATION OF SAME NAME ALREADY EXISTS!!\n");
			return;
		}
	attrcat(newfilename,attribute,attribute_type,count,"-1","-1");
	//relcat("RELATION1",count,3,7,8,9);
	//relcat("RELATION2",count,3,7,8,9);
          printf("%d",count);
	printf("%c\n",ch);
	int first_blk=getfreeblock();
	int curr_blk=first_blk;
	int prev_blk=-1;
	int no_of_slots=floor(2016/(count*16+1));
	int no_of_rec=number_of_records(file);
	//printf("No of rec%d",no_of_rec);
	fclose(file);
	int no_records=no_of_rec;
	int l=no_of_rec/no_of_slots;
	if(no_of_rec%no_of_slots>0)
		l=l+1;	
	int no_of_blocks=l;
	file=fopen(filename,"r");
	while((ch = fgetc(file)) != '\n')
	 	continue;
	//ch=fgetc(file);

	printf("Count:%d no-rec:%d slots:%d",count,no_of_rec,no_of_slots);
	printf("no blk%d\n",no_of_blocks);
	FILE *disk = fopen("disk.txt","r+");
	for(i=0;i<no_of_blocks;i++)
	{
		//printf("Entered loop\n");
		printf("CURR%d\n",curr_blk);
		fseek(disk,curr_blk*2048,SEEK_SET);
		fputs("REC",disk);
		//printf("\n%c\n",cc);
		fputc('\0',disk);
		fseek(disk,curr_blk*2048+4,SEEK_SET);
		fputs("-1",disk);
		fputc('\0',disk);	
		fseek(disk,curr_blk*2048+4*2,SEEK_SET);		
		char t[5];
		sprintf(t, "%d", prev_blk);
		fputs(t,disk);
		fputc('\0',disk);
		
		fseek(disk,curr_blk*2048+4*4,SEEK_SET);
		int no_of_entries;
		int next_blk;
		if(no_of_rec>=no_of_slots)
		{

			 no_of_entries=no_of_slots;
			sprintf(t, "%d", no_of_slots);
			fputs(t,disk);
			fputc('\0',disk);
			fseek(disk,curr_blk*2048+4*3,SEEK_SET);
			next_blk=getfreeblock();
			sprintf(t, "%d", next_blk);
			fputs(t,disk);
			fputc('\0',disk);
		}
		else
		{
			 no_of_entries=no_of_rec;
			char t[5];
			sprintf(t, "%d", no_of_rec);
			fputs(t,disk);
			fputc('\0',disk);
			next_blk=-1;
			fseek(disk,curr_blk*2048+4*3,SEEK_SET);
			fputs("-1",disk);
			fputc('\0',disk);
		}
		
		fseek(disk,curr_blk*2048+4*5,SEEK_SET);
		sprintf(t, "%d", count);
		fputs(t,disk);
		fputc('\0',disk);
		fseek(disk,curr_blk*2048+4*6,SEEK_SET);
		sprintf(t, "%d", no_of_slots);
		fputs(t,disk);
		fputc('\0',disk);
		fseek(disk,curr_blk*2048+4*8,SEEK_SET);
		for(int n=0;n<no_of_entries;n++)
			fputc('1',disk);
		fseek(disk,curr_blk*2048+32+no_of_slots,SEEK_SET);
		char temp[16];int k=0;
	         // printf("LOOP1\n");
		for(;k<no_of_slots&&no_of_rec!=0;k++,no_of_rec--)
		{         int j=0;
			//printf("%c\n",ch);
			//printf("LOOP2\n");
			while(j<count)
			{ 
			int p=0;
			//printf("LOOP3\n");
			char cc=fgetc(file);
			//printf("%c\n",cc);
       			while(((cc!=',')&&(cc!='\n'))&&(p<16))
			{
	    		temp[p++]=cc;
			cc=fgetc(file);
			}
			temp[p]='\0';
			fputs(temp,disk);
			fputc('\0',disk);
			
			//printf("File %c\n",ch);
			int dif=16-strlen(temp)-1;
			fseek(disk,dif,SEEK_CUR);
			j++;		
			}
			
		}
		prev_blk=curr_blk;
		curr_blk=next_blk;
		
	}
	//printf("+++++++++++%d\n+++++++%d\n",first_blk,curr_blk);
	relcat(newfilename,count,no_records,first_blk,curr_blk,no_of_slots);
	fclose(file);
	fclose(disk);
}

void fdisk()
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
	formatdisk();
	meta();
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
void bmap_dump()
{
	FILE *disk=fopen("disk.txt","r");
	char ch;
    int i=0;
    FILE *bmap=fopen("bmap.txt","w+");
    fseek(disk,0,SEEK_SET);
    while (i<8192)
    {	
       	ch=getc(disk);
        fprintf(bmap,"%d : %c\n",i,ch);
 	  	i++;
    }
	fclose(disk);
	fclose(bmap);
	printf("Dumped Block Allocation Map Successfully\n");

}
void ls()
{	printf("\n\nLS\n");
	char s[5];
	FILE *disk=fopen("disk.txt","r");
	fseek(disk,4*2048+12,SEEK_SET);
	int curr_blk=4;int next_blk;
	while(curr_blk!=-1)
	{
		fseek(disk,curr_blk*2048+12,SEEK_SET);
		fgets(s,5,disk);
		next_blk=atoi(s);
		int no_of_entries;
	
		fseek(disk,curr_blk*2048+16,SEEK_SET);
		fgets(s,5,disk);
		no_of_entries=atoi(s);
		//printf("%d\n",no_of_entries);
		int i;char rel_name[17];
		for(i=0;i<no_of_entries;i++)
			{
			fseek(disk,curr_blk*2048+52+i*96,SEEK_SET);
			fgets(rel_name,17,disk);
			printf("%s\n",rel_name);
			}

		curr_blk=next_blk;
	}
	printf("\n");
	fclose(disk);
}
void dump_record_block(int block_no,char *filename)
{	
	FILE *disk=fopen("disk.txt","r");
	FILE *fp=fopen(filename,"w+");
	fseek(disk,(block_no*2048)+24,SEEK_SET);
	int no_of_slots;
	char s[5];
	fgets(s,5,disk);
	no_of_slots=atoi(s);
	int no_of_entries,no_of_attributes;
	fseek(disk,block_no*2048+16,SEEK_SET);
	fgets(s,5,disk);
	no_of_entries=atoi(s);
	fseek(disk,block_no*2048+20,SEEK_SET);
	fgets(s,5,disk);
	no_of_attributes=atoi(s);
	int i=0;
	for(i=0;i<=28;i+=4)
	{	fseek(disk,block_no*2048+i,SEEK_SET);
		char s[5];
		fgets(s,5,disk);
		fprintf(fp,"%s, ",s);
	}
	char t[no_of_slots+1];
	fseek(disk,block_no*2048+32,SEEK_SET);
	fgets(t,(no_of_slots+1),disk);
	printf("no-slots%d\n%s",no_of_slots,t);
	fprintf(fp,"%s\n",t);
	fseek(disk,block_no*2048+32+no_of_slots,SEEK_SET);
	int j;
	char t1[17];
	for(i=0;i<no_of_entries;i++)
	{	fseek(disk,block_no*2048+32+no_of_slots+(no_of_attributes*16*i),SEEK_SET);
		for(j=0;j<no_of_attributes;j++)
		{	
			fgets(t1,17,disk);
			fprintf(fp,"%s ,",t1);
			//fseek(disk,16,SEEK_CUR);
		}
		fprintf(fp,"\n");
	}
         fclose(disk);
	fclose(fp);
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
	//fputc('\0',disk);


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
	fseek(disk,4*2048+132, SEEK_SET);	
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
	//fputc('\0',disk);

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

void relcat(char *rel_name,int attr,int rec,int first,int last,int slots)
{
	int free=0;
	printf("No of rec in relcat%d\n",rec);
	FILE *disk = fopen("disk.txt","r+");
	//finding the last block of relation catalog from relation catalog
	fseek(disk,4*2048+116, SEEK_SET);
	char temp[5];
	for(int i=0;i<4;i++)
			temp[i]=getc(disk);
	temp[4]='\0';
	int last_block=atoi(temp);
	printf("BYE from relcat  %d\n",last_block);
	//finding the no of entries in last block
	fseek(disk,last_block*2048+16, SEEK_SET);
	char ch = getc(disk);
	while(ch!='\0')
	{
		free=free*10+ch-'0';
		ch=getc(disk);
	}
	printf("%d",free);
	//if the present last blk is filled,get the new free blk
	if(free==19)
		{
		int z;
		z=last_block;
		last_block=getfreeblock();
		printf("GET FREE BLK:%d\n",last_block);
		free=0;
		fseek(disk,z*2048+12, SEEK_SET);
		char t[5];
		sprintf(t, "%d", z);
		fputs(t,disk);
		fseek(disk,last_block*2048+12,SEEK_SET);
		fputs("-1",disk);
		fputc('\0',disk);
		}

		fseek(disk,last_block*2048+32, SEEK_SET);char cha;
		while((cha=getc(disk))!='0')
		{}
		fseek(disk,-1,SEEK_CUR);
		fputc('1',disk);
		char t[5];
		fseek(disk,last_block*2048+16, SEEK_SET);
		fgets(t,5,disk);
		int r=atoi(t);
		r=r+1;
		sprintf(t, "%d", r);
		printf("last blk:%d\nr%d\n",last_block,r);
		fseek(disk,last_block*2048+16, SEEK_SET);
		fputs(t,disk);
		fputc('\0',disk);
	//inserting metadata of each relation to relation catalog 
		fseek(disk,last_block*2048+(free)*96+52, SEEK_SET);
		fputs(rel_name,disk);
		fputc('\0',disk);
		fseek(disk,last_block*2048+(free)*96+52+16, SEEK_SET);
		
		sprintf(t, "%d", attr);
		fputs(t,disk);
		fputc('\0',disk);
		fseek(disk,last_block*2048+(free)*96+52+32, SEEK_SET);
		sprintf(t, "%d", rec);
		fputs(t,disk);
		fputc('\0',disk);
		fseek(disk,last_block*2048+(free)*96+52+48, SEEK_SET);
		sprintf(t, "%d", first);
		fputs(t,disk);
		fputc('\0',disk);
		fseek(disk,last_block*2048+(free)*96+52+64, SEEK_SET);
		sprintf(t, "%d", last);
		fputs(t,disk);
		fputc('\0',disk);
		fseek(disk,last_block*2048+(free)*96+52+80, SEEK_SET);
		sprintf(t, "%d", slots);
		fputs(t,disk);
		fputc('\0',disk);
			
	fclose(disk);
} 

void attrcat(char *rel_name,char attr_name[][16],char attr_type[][16],int count,char* root_block,char* primary_flag)
{
	int free=0;
	FILE *disk = fopen("disk.txt","r+");
	FILE *slotmap = fopen("disk.txt","r+");
	FILE *no_entries= fopen("disk.txt","r+");
	//finding the last block of attribute catalog from relation catalog
	fseek(disk,4*2048+212, SEEK_SET);
	char temp[5];
	for(int i=0;i<4;i++)
			temp[i]=getc(disk);
	temp[4]='\0';
	int last_block=atoi(temp);
	//finding the no of entries in last block
	//fseek(disk,4*2048+212, SEEK_SET);
	fseek(disk,last_block*2048+16, SEEK_SET);
	char ch = getc(disk);
	while(ch!='\0')
	{
		free=free*10+ch-'0';
		ch=getc(disk);
	}
	printf("%d",free);
	
	//if the present last blk is filled,get the new free blk
	if(free==20)
		{
		int z;
		z=last_block;
		last_block=getfreeblock();
		printf("GET FREE BLK:%d\n",last_block);
		free=0;
		fseek(disk,z*2048+12, SEEK_SET);
		char t[5];
		sprintf(t, "%d", z);
		fputs(t,disk);
		fseek(disk,last_block*2048+12,SEEK_SET);
		fputs("-1",disk);
		fputc('\0',disk);
		fseek(disk,last_block*2048,SEEK_SET);
		fputs("REC",disk);
		fputc('\0',disk);
		fseek(disk,last_block*2048+4,SEEK_SET);
		fputs("-1",disk);
		fputc('\0',disk);
		fseek(disk,last_block*2048+8,SEEK_SET);
		char tt[5];
		sprintf(tt, "%d", z);
		fputs(tt,disk);
		fputc('\0',disk);
		
		fseek(disk,last_block*2048+20,SEEK_SET);
		sprintf(tt, "%d", count);
		fputs(tt,disk);
		fputc('\0',disk);
		fseek(disk,last_block*2048+24,SEEK_SET);
		fputs("20",disk);
		fputc('\0',disk);
		}
	fseek(slotmap,last_block*2048+32,SEEK_SET);	
	char v=getc(slotmap);
	while(v!='0')
	{
		v=getc(slotmap);
	}
	fseek(slotmap,-1,SEEK_CUR);
	
	int i;
	//inserting metadata of each attribute to attribute catalog
	for(i=0;i<count;i++,free++)
	{
		//if the present last blk is filled,get the new free blk 
		if(free==20)
		{
		int z=last_block;
		last_block=getfreeblock();
		printf("GET FREE BLK:%d\n",last_block);
		free=0;
		fseek(disk,z*2048+12, SEEK_SET);
		char t[5];
		sprintf(t, "%d", last_block);
		fputs(t,disk);
		fputc('\0',disk);
		//printf("jjjjj%s\n",t);
		fseek(disk,last_block*2048+12,SEEK_SET);
		fputs("-1",disk);
		fputc('\0',disk);
		fseek(slotmap,last_block*2048+32,SEEK_SET);
		fseek(disk,last_block*2048,SEEK_SET);
		fputs("REC",disk);
		fputc('\0',disk);
		fseek(disk,last_block*2048+4,SEEK_SET);
		fputs("-1",disk);
		fputc('\0',disk);
		fseek(disk,last_block*2048+8,SEEK_SET);
		char tt[5];
		sprintf(tt, "%d", z);
		fputs(tt,disk);
		fputc('\0',disk);
		
		fseek(disk,last_block*2048+20,SEEK_SET);
		sprintf(tt, "%d", count);
		fputs(tt,disk);
		fputc('\0',disk);
		fseek(disk,last_block*2048+24,SEEK_SET);
		fputs("20",disk);
		fputc('\0',disk);
		
		}

		fseek(no_entries,last_block*2048+16,SEEK_SET);
		char t[5];
		fgets(t,5,no_entries);
		int r=atoi(t);
		r=r+1;
		printf("entry attr%d\n",r);
		sprintf(t, "%d", r);
		fseek(no_entries,last_block*2048+16,SEEK_SET);
		fputs(t,no_entries);
		fputc('\0',no_entries);	
		fseek(no_entries,last_block*2048+16,SEEK_SET);
		/*fgets(t,5,no_entries);
		printf("Entries in attrcat%s\n",t);*/

		fputc('1',slotmap);
		fseek(slotmap,-1,SEEK_CUR);
		char bb=getc(slotmap);
		printf("SLOTMAP %c",bb);

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
	printf("bye\n");
	fclose(disk);
	fclose(slotmap);
	fclose(no_entries);
}

void read_rel_catalog()
{
	FILE *disk = fopen("disk.txt","r");
	char ch;
	printf("\nRELATION CATALOG\n");
	//set the first block as current blk
	int curr_block=4;
	int next_block=-1;
	int i=0;
	FILE *fptr;
	fptr = fopen("relation_catalog.txt", "w");
	do
	{
	//find the next blk using current blk
	fseek(disk,curr_block*2048+12, SEEK_SET);
	char temp[5];
	for(int i=0;i<4;i++)
		temp[i]=getc(disk);
	temp[4]='\0';
	int next_block=atoi(temp);
	//print current blk contents
	fseek(disk,curr_block*2048, SEEK_SET);
	i=0;
	/*while(i<32)
	{
		int j=0;
		while(j<4)
		{
		ch=getc(disk);
		if(ch=='\0')
			{
			j++;
			i++;
			}
		else
			{			
			fprintf(fptr,"%c", ch);
			j++;i++;
			}
		
		}		
		fprintf(fptr,"\t");		
	}
	i=33;*/
	while(i<2048)
	{
		ch=getc(disk);
		if(ch=='\0')
			fprintf(fptr,"EOS\n");
		else
			fprintf(fptr,"%c\n", ch);
		i++;
	}
	curr_block=next_block;
 	//when next block becomes -1, exit from the loop
	if(curr_block==-1)
   		break;
	//change the next blk
	next_block=fseek(disk,curr_block*2048+12, SEEK_SET);
	printf("current block:%d\n",curr_block);
	printf("next block:%d\n",next_block);
	}while(1);
fclose(disk);
fclose(fptr);
}

void read_attr()
{
	FILE *disk = fopen("disk.txt","r");
	char ch;
	printf("\nATTRIBUTE CATALOG\n");
	//set the first block as current blk
	int curr_block=5;
	int next_block=-1;
	int i=0;
	FILE *fptr;
	fptr = fopen("attribute_catalog.txt", "w");
	do
	{
	//find the next blk using current blk
	fseek(disk,curr_block*2048+12, SEEK_SET);
	char temp[5];
	for(int i=0;i<4;i++)
		temp[i]=getc(disk);
	temp[4]='\0';
	int next_block=atoi(temp);
	//print current blk contents
	fseek(disk,curr_block*2048, SEEK_SET);
	i=0;
	
	while(i<32)
	{
		//ch=getc(disk);
		char tem[5];
		fgets(tem,5,disk);
		fprintf(fptr,"%s, ",tem);
		/*if(ch=='\0')
			fprintf(fptr,"\n");
		else
			fprintf(fptr,"%c", ch);*/
		i=i+4;
	}
	i=32;
	while(i<52)
	{
		char ch;
		ch=getc(disk);
		
		fprintf(fptr,"%c", ch);
		i++;
	}
	fprintf(fptr,"\n");
	i=51;
	while(i<2048)
	{
		int j=0;
		while(j<96)
		{
		char tem[17];
		fgets(tem,17,disk);
		fprintf(fptr,"%s, ",tem);	
		j+=16;
		}
		fprintf(fptr,"\n");
		i=i+96;
	}
	curr_block=next_block;
 	//when next block becomes -1, exit from the loop
	if(curr_block==-1)
   		break;
	//change the next blk
	next_block=fseek(disk,curr_block*2048+12, SEEK_SET);
	printf("current block:%d\n",curr_block);
	printf("next block:%d\n",next_block);
	}while(1);
fclose(disk);
fclose(fptr);
}

void read_attr_cat(char *rel_name,int no_attributes,char *exportname)
{
	printf("\n77777");
	printf("\nin readattrcat%s",exportname);
	FILE *disk = fopen("disk.txt","r");
	
	char ch;
	//set the first block as current blk
	int curr_blk=5;
	int next_blk=-1;
	int i=0;
	FILE *fptr;
	fptr = fopen(exportname, "w+");
	int j=0;
          int no_of_entries;
	do
	{
	fseek(disk,curr_blk*2048+12, SEEK_SET);
	char temp[5];
	for(int i=0;i<4;i++)
		temp[i]=getc(disk);
	temp[4]='\0';
	next_blk=atoi(temp);
          fseek(disk,curr_blk*2048+16, SEEK_SET);
	fgets(temp,5,disk);
	no_of_entries=atoi(temp);
	
	int y;
	printf("NOATTR%d",no_attributes);
	for(y=0;(j<no_attributes)&&(y<no_of_entries);y++)
	{	
		fseek(disk,curr_blk*2048+52+(y*96), SEEK_SET);
		char ch;
				int l;char str[17];
				for( l=0;(l<16)&&(ch=getc(disk))!='\0';l++)
				{	//printf("9999%c",ch);
					str[l]=ch;
				}
				str[l]='\0';
				//fgets(str,17,disk);
				//printf("//////%s",str);
		
				if((strcmp(str,rel_name))==0)
				{
				j++;
				printf("!!!!!%d\n",j);
				fseek(disk,curr_blk*2048+52+(y*96)+16, SEEK_SET);
				for( l=0;(l<16)&&(ch=getc(disk))!='\0';l++)
				{	printf("%c",ch);
					str[l]=ch;
				}
				str[l]='\0';
				fputs(str,fptr);
				//if(j==(no_attributes-1))
					//break;
				fputs(",",fptr);
				}
		}
	curr_blk=next_blk;
	printf("CURR%d",curr_blk);
 	//when next block becomes -1, exit from the loop
	if(curr_blk==-1)
   		break;
	
	
	}while(1);
	fseek(fptr,-1,SEEK_CUR);
	fputs("\n",fptr);
fclose(disk);
fclose(fptr);
	
}

void rm(char *relname)
{
	char s[5];
	FILE *disk=fopen("disk.txt","r+");
	//fseek(disk,4*2048+12,SEEK_SET);
	//Search for relation info in relcat
	int curr_blk=4;int next_blk;int i;int first_block;
	while(curr_blk!=-1)
	{
		fseek(disk,curr_blk*2048+12,SEEK_SET);
		fgets(s,5,disk);
		next_blk=atoi(s);
		int no_of_entries;
	
		fseek(disk,curr_blk*2048+16,SEEK_SET);
		fgets(s,5,disk);
		no_of_entries=atoi(s);
		//printf("%d\n",no_of_entries);
		char rel_name[17];
		for(i=0;i<no_of_entries;i++)
			{
			fseek(disk,curr_blk*2048+52+i*96,SEEK_SET);
			fgets(rel_name,17,disk);
			if(strcmp(rel_name,relname)==0)
			{
				fseek(disk,curr_blk*2048+52+i*96+48,SEEK_SET);
				fgets(rel_name,17,disk);
				first_block= atoi(rel_name);
				printf("\nInRM!!!---%d\n---",first_block);
				break;
				
					
			}
			}
		if(i!=no_of_entries)
			break;
		curr_blk=next_blk;
	}
	if(curr_blk==-1)
	{
		printf("Relation does not exist!");
		return;
	}
	int rel_pos=i;
	//printf("\nRELPOSSS%dCURRBlock%d",rel_pos,curr_blk);
	//update disk free block due to nullifying relation catalog entry
	char s1[5];
	fseek(disk,curr_blk*2048+16,SEEK_SET);
	fgets(s1,5,disk);
	//number of entries in relation catalog block containing the relation's entry.
	int no=atoi(s);
	no=no-1;
	if(no==0)
	{
		
		fseek(disk,curr_blk,SEEK_SET);	
		fputc('0',disk);
	}
	fseek(disk,curr_blk*2048+16,SEEK_SET);
	char t3[5];
	sprintf(t3,"%d",no);
	fputs(t3,disk);
	int j=0;
	while(j<6)
	{	printf("\nPrinting zeroes in relcat");
		printf("\nRELPOSSS%dCURRBlock%d",rel_pos,curr_blk);
		fseek(disk,curr_blk*2048+52+rel_pos*96+j*16,SEEK_SET);
		fputs("0000000000000000",disk);
		j++;
	}
	
	char tempo[17];
	fseek(disk,curr_blk*2048+52+rel_pos*96+0*16,SEEK_SET);
	fgets(tempo,17,disk);
	printf("\nttttt%s",tempo);
	curr_blk=5;int n_attr=0;
	while(curr_blk!=-1)
	{
		fseek(disk,curr_blk*2048+12,SEEK_SET);
		fgets(s,5,disk);
		next_blk=atoi(s);
		int no_of_entries;
	
		fseek(disk,curr_blk*2048+16,SEEK_SET);
		fgets(s,5,disk);
		no_of_entries=atoi(s);
		//printf("%d\n",no_of_entries);
		int i;char rel_name[17];
		for(i=0;i<no_of_entries;i++)
			{
			fseek(disk,curr_blk*2048+52+i*96,SEEK_SET);
			fgets(rel_name,17,disk);
			int j=0;
			if(strcmp(rel_name,relname)==0)
			{	n_attr++;
				while(j<6)
				{	
					fseek(disk,curr_blk*2048+52+i*96+j*16,SEEK_SET);
					fputs("0000000000000000",disk);
					j++;
				}	
			}
			}
		no_of_entries=no_of_entries-n_attr;
		char s2[5];
		fseek(disk,curr_blk*2048+16,SEEK_SET);
		sprintf(s2, "%d", no_of_entries);
		fputs(s2,disk);
		if(no_of_entries==0)
		{
			fseek(disk,curr_blk,SEEK_SET);
			fputc('0',disk);
		}
	curr_blk=next_blk;	
	}
	curr_blk=first_block;
	while(curr_blk!=-1)
	{		
		fseek(disk,curr_blk*2048+12,SEEK_SET);
		char s[5];
		fgets(s,5,disk);
		int next_blk=atoi(s);
		fseek(disk,curr_blk*2048,SEEK_SET);
		int j=0;
		while(j<2048)
		{
			fputc('0',disk);
			j++;
		}
		curr_blk=next_blk;
	}
	
	fclose(disk);
	
}
void export(char *rel_name,char *exportname)
{
	//printf("******!!%s",rel_name);
	FILE *disk = fopen("disk.txt","r");
	fseek(disk,4*2048+12, SEEK_SET);
	int curr_blk=4;
	char temp[5];
	for(int i=0;i<4;i++)
		temp[i]=getc(disk);
	temp[4]='\0';
	int next_blk=atoi(temp);
	printf("NEXT BLK%d\n",next_blk);
	int i;
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
	   	int no_entries=atoi(temp);
		printf("No entries%d\n",no_entries);
		for( i=0;i<no_entries;i++)
		{
			fseek(disk,curr_blk*2048+i*96+52, SEEK_SET);
			char t[17];
			char ch;int j;
			for( j=0;j<16&&(ch=getc(disk))!='\0';j++)
				t[j]=ch;
			t[j]='\0';
			printf("TEST REL%s\n",t);
			printf("FN REL%s\n",rel_name);
			if(strcmp(t,rel_name)==0)
				break;
		}
		if(i!=no_entries)
			break;
		curr_blk=next_blk;
	}
   	if(curr_blk==-1)
	{
		printf("RELATION DOES NOT EXIST!!\n");
		return;
	}
	printf("Entry%d\n",curr_blk);
	int no_attributes,no_records,f_blk,l_blk,no_slots;
	for(int m=0;m<5;m++)
	{
		fseek(disk,curr_blk*2048+i*96+52+16*(m+1), SEEK_SET);
		int j;char t[17];char ch;
		for( j=0;j<16&&(ch=getc(disk))!='\0';j++)
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
	//printf("****%d\n",curr_blk);
	fseek(disk,l_blk*2048, SEEK_SET);
	curr_blk=f_blk;
	//printf("****%d\n",curr_blk);
	//char *exp=malloc(25*sizeof(char));
	//exp=strcat(rel_name,"_export");
	//printf("%s",exp);
	FILE *fp_export=fopen(exportname,"a");
	read_attr_cat(rel_name,no_attributes,exportname);
	
	printf("!!!!!!!!-----");
	while(curr_blk!=-1)
	{	fseek(disk,curr_blk*2048+12, SEEK_SET);
		char s[5];
		fgets(s,5,disk);
		next_blk=atoi(s);
		fseek(disk,curr_blk*2048+16, SEEK_SET);
		fgets(s,5,disk);
		//printf("****%d\n",curr_blk);
		int curr_no_records=atoi(s);
		printf("%d\n",curr_no_records);
		for(int i=0;i<curr_no_records;i++)
		{
			char str[17];
			printf("%d\n",no_slots);
			printf("%d\n",no_attributes);
			for(int j=0;j<no_attributes;j++)
			{	fseek(disk,(curr_blk*2048)+32+no_slots+((i*16)*no_attributes)+(j*16), SEEK_SET);
				//printf("****%d\n",curr_blk);
				//printf("%d",(curr_blk*2048)+32+no_slots+((i*16)*no_attributes)+(j*16));
				char ch;
				
				//printf("%c",ch);
				int l;
				for( l=0;l<16&&(ch=getc(disk))!='\0';l++)
				{	//printf("%c",ch);
					str[l]=ch;
				}
				str[l]='\0';
				//fgets(str,17,disk);
				printf("EXPORT%s\n",str);
				fputs(str,fp_export);
				if(j==no_attributes-1)
					break;
				fputs(",",fp_export);
			}
			fputs("\n",fp_export);
		}
		printf("curr %d\n next %d",curr_blk,next_blk);
		curr_blk=next_blk;
		break;
	}
	printf("%s.txt created successfully",rel_name);
	fclose(fp_export);
	fclose(disk);
}


/*int main()
{
	fdisk();
	formatdisk();
	meta();
	
	parse("sample.csv");
	export("sample","dd");
	ls();
	bmap_dump();
	dump_record_block(6,"dump");
 	//read_attr();
	//read_rel_catalog();
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
	//return 0;
//
//}
