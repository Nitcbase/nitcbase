#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fn.c"

int main()
{
	while(1)
	{
		printf("\n# ");
		char ch;
		//Reading a line 
		char *command=(char *)malloc(sizeof(char));
		int i=1;
		scanf("%c",&ch);
		while(ch!='\n')
		{
			command[i-1]=ch;
			i++;
			command=realloc(command,i);
			scanf("%c",&ch);
		}
		command[i-1]='\0';
		printf("%s\n\n\n",command);
		//Done reading the line into "command"
		//If command is exit,return else keep looping
		if(strcmp(command,"exit")==0)
		{
			return;
		}
		else if(strcmp(command,"ls")==0)
		{
			ls();
		}
		else if(strcmp(command,"fdisk")==0)
		{
			fdisk();
		}
		else if(strcmp(command,"dump attrcat")==0)
		{
			read_attr();
		}
		else if(strcmp(command,"dump relcat")==0)
		{
			read_attr();
		}
		else if(strcmp(command,"dump bmap")==0)
		{
			printf("DUMPBMAP");
			bmap_dump();
		}
		else
		{	//Taking first word from command
			char *first_word=(char *)malloc(sizeof(char));
	
			int i=1;
			//scanf("%c",&ch);
			while(command[i-1]!=' ')
			{
				first_word[i-1]=command[i-1];
				i++;
				first_word=realloc(first_word,i);
				//scanf("%c",&ch);
			}
			first_word[i-1]='\0';
			printf("%s\n",first_word);
			
			if(strcmp("import",first_word)==0)
			{
				//printf("imported");
				char *filename=(char *)malloc(sizeof(char));
				//int i=1;
				int j=0;
				while(command[i]!='\0')
				{
				filename[j++]=command[i];
				i++;
				filename=realloc(filename,j+1);
				}
				/*scanf("%c",&ch);
				while(ch!='\n')
				{
					filename[i-1]=ch;
					i++;
					realloc(filename,i);
					scanf("%c",&ch);
				}*/
				filename[j]='\0';
				printf("\nFilename%s",filename);
				parse(filename);
				free(filename);
			     
			}
			else if(strcmp("rm",first_word)==0)
			{
				//printf("imported");
				char *filename=(char *)malloc(sizeof(char));
				//int i=1;
				int j=0;
				while(command[i]!='\0')
				{
				filename[j++]=command[i];
				i++;
				filename=realloc(filename,j+1);
				}
				/*scanf("%c",&ch);
				while(ch!='\n')
				{
					filename[i-1]=ch;
					i++;
					realloc(filename,i);
					scanf("%c",&ch);
				}*/
				filename[j]='\0';
				printf("\nFilename%s",filename);
				rm(filename);
				free(filename);
			     
			}
			else if(strcmp("export",first_word)==0)
			{
				char *relname=(char *)malloc(sizeof(char));
				//i is the position after first space
				//scanf("%c",&ch);
				int j=0;
				while(command[i]!=' ')
				{
					relname[j++]=command[i++];
					relname=realloc(relname,(j+1));
				}
				relname[j]='\0';
				printf("eeeRELNAME%s",relname);
				char *filename=(char *)malloc(sizeof(char));
				 j=0;
				i=i+1;
				while(command[i]!='\0')
				{
				filename[j++]=command[i++];
				filename=realloc(filename,j+1);
				}
				/*scanf("%c",&ch);
				while(ch!='\n')
				{
					filename[i-1]=ch;
					i++;
					filename=realloc(filename,i);
					scanf("%c",&ch);
				}*/
				filename[j]='\0';
				printf("eeeFilename%s",filename);
				export(relname,filename);// Here it should be one argument(filename is default EXPORT) but need to change to 2 args anyway.
				free(filename);
			       

			}
			else if(strcmp(first_word,"dump_record_block")==0)
			{	

				char *relname=(char *)malloc(sizeof(char));
				//i is the position after first space
				//scanf("%c",&ch);
				int j=0;
				while(command[i]!=' ')
				{
					relname[j++]=command[i++];
					relname=realloc(relname,(j+1));
				}
				relname[j]='\0';
				printf("eeeRELNAME%s",relname);
				char *filename=(char *)malloc(sizeof(char));
				 j=0;
				i=i+1;
				while(command[i]!='\0')
				{
				filename[j++]=command[i++];
				filename=realloc(filename,j+1);
				}
				/*scanf("%c",&ch);
				while(ch!='\n')
				{
					filename[i-1]=ch;
					i++;
					filename=realloc(filename,i);
					scanf("%c",&ch);
				}*/
				filename[j]='\0';
				printf("eeeFilename%s",filename);
				dump_record_block(atoi(relname),filename);
				//Here it should be one argument(filename is default EXPORT) but need to change to 2 args anyway.
				free(filename);
			       
				/*char *blkno=(char *)malloc(sizeof(char));
				int i=1;
				scanf("%c",&ch);
				while(ch!='\n')
				{
					blkno[i-1]=ch;
					i++;
					blkno=realloc(blkno,i);
					scanf("%c",&ch);
				}
				blkno[i]='\0';
				char *filename=(char *)malloc(sizeof(char));
				i=1;
				scanf("%c",&ch);
				while(ch!='\n')
				{
					filename[i-1]=ch;
					i++;
					filename=realloc(filename,i);
					scanf("%c",&ch);
				}
				filename[i]='\0';
				dump_record_block(atoi(blkno),filename);
				free(filename);*/
	

			}
			free(first_word);
		}
		
		free(command);
		
	}
	return 0;
}
