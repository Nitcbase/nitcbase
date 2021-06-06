#include "../define/constants.h"
#include "disk_structure.h"
#include<stdio.h>
#include<stdlib.h>
#include<bits/stdc++.h>
using namespace std;
int createRel(char relname[16],int nAttrs, char attrs[][ATTR_SIZE],int attrtype[])
{
cout<<"In Create fn!\n";
}

int openRel(char RelName[16])
{
cout<<"In openRel fn!\n";
}

int closeRel(int relid) 
{
cout<<"In closeRel fn!\n";
}

int deleteRel(char relname[ATTR_SIZE])
{
cout<<"In deleteRel fn!\n";
}

int ba_insert(int relid, union Attribute *rec)
{
cout<<"In Insert fn!\n";
}


int select(char srcrel[ATTR_SIZE],char targetrel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strval[ATTR_SIZE])
{
cout<<"In select fn!\n";
}

int project(char srcrel[ATTR_SIZE],char targetrel[ATTR_SIZE],int tar_nAttrs, char tar_attrs[][ATTR_SIZE])
{
cout<<"In project fn!\n";
}

int join(char srcrel1[ATTR_SIZE],char srcrel2[ATTR_SIZE],char targetrel[ATTR_SIZE], char attr1[ATTR_SIZE], char attr2[ATTR_SIZE])
{
cout<<"In join fn!\n";
}

int getRelId(char relname[16])
{
}

int bplus_create(int relid, char AttrName[ATTR_SIZE])
{
cout<<"In bplusCreate fn!\n";
}

int dropindex(int relid,char attrname[16])
{
cout<<"In drop index fn!\n";
}

int ba_renamerel(char oldName[ATTR_SIZE], char newName[ATTR_SIZE])
{
cout<<"In Rename fn!\n";
}

int getRelCatEntry(int rel_id, union Attribute *relcat_entry)
{
}

struct HeadInfo getheader(int blockNum)
{
}

void getSlotmap(unsigned char * SlotMap,int blockNum)
{
}

int getRecord(union Attribute *rec,int blockNum,int slotNum)
{
}

int ba_renameattr(char relName[ATTR_SIZE], char oldName[ATTR_SIZE], char newName[ATTR_SIZE])
{
cout<<"In Rename fn!\n";
}

int insert_val(vector <string> s,char tablename[16])
{
	cout<<"In insert fn!\n";
 	char ch;
	int relid=getRelId(tablename);
	if(relid==E_CACHEFULL||relid==E_RELNOTEXIST||E_RELNOTOPEN)
	{
		return relid;
	}
   	union Attribute relcatentry[6];
   	int y;
   	y=getRelCatEntry(relid,relcatentry);
	if(y!=SUCCESS)
	{
		//cout<<"INSERT NOT POSSIBLE\n";
       		return y;
	}
	int count;
	count = relcatentry[1].ival;
	int type[count];
	//getting the types of all attributes
	union Attribute attr[6];
	int attr_blk=5;
	char Attr_name[6][16];
          int j=0;
	struct HeadInfo h;
	while(attr_blk != -1)
	{
		h=getheader(attr_blk);
		unsigned char slotmap[h.numSlots];
		getSlotmap(slotmap,attr_blk);
		for(int i=0;i<20;i++)
      		{
        	    	     getRecord(attr,attr_blk,i);
		     if((char)slotmap[i]=='0')
		     {
			continue;
		     }
		     if(strcmp(attr[0].sval,tablename)==0) 
		     {
	              	type[attr[5].ival]=attr[2].ival;
		     } 
		}
		attr_blk =h.rblock;
	}
	//-----------------------------------------------------
          char record_array[count][16];
	if(s.size()!=count)
	{
		cout<<"Mismatch in no of arguments\n";
		return E_NATTRMISMATCH;
	}
          //cout<<"hi\n";
	//cout<<count<<"\n";
       	for(int i=0;i<count;i++)
	{
		string x=s[i];
		char p[16];
		int j;
		for(j=0;j<15&&j<x.size();j++)
		{
			p[j]=x[j];
		}
		p[j]='\0';
		strcpy(record_array[i],p);
	}	
	
          union Attribute rec[count];
          for(int l=0;l<count;l++)
	{
	   	if(type[l]==FLOAT)
		       {
		      // cout<<record_array[l]<<"\n";
                           rec[l].fval=atof(record_array[l]);
		       //cout<<rec[l].fval<<"\n";
		       }
		if(type[l]==INT)
		       {
		       //cout<<record_array[l]<<"\n";
                           rec[l].ival=strtoull(record_array[l],NULL,10);
		       //cout<<rec[l].ival<<"\n";
		       }
		if(type[l]==STRING)
		       {
		       //cout<<record_array[l]<<"\n";
                           strcpy(rec[l].sval,record_array[l]);
		       //cout<<rec[l].sval<<"\n";
		       }
	}
          int r; 
	
          r=ba_insert(relid,rec);
	if(r==SUCCESS)
	{cout<<"SUCCESS\n";
		return SUCCESS;
	}
	else
	{
		cout<<"FAILURE\n";
		return FAILURE;
	}	
                   
}

int insert(char tablename[16],char *filename)
{
	cout<<"In insert fn!\n";
	FILE *file=fopen(filename,"r");
	char ch;
	int relid=getRelId(tablename);
	if(relid==E_CACHEFULL||relid==E_RELNOTEXIST||E_RELNOTOPEN)
	{
		//cout<<"INSERT NOT POSSIBLE\n";
		return relid;
	}
   	union Attribute relcatentry[6];
   	int y=getRelCatEntry(relid,relcatentry);
	if(y!=SUCCESS)
	{
		//cout<<"INSERT NOT POSSIBLE\n";
       		return y;
	}
	int count;
	count = relcatentry[1].ival;
	int type[count];

	//getting the types of all attributes
	union Attribute attr[6];
	int attr_blk=5;
	char Attr_name[6][16];
          int j=0;
	struct HeadInfo h;
	char s[16];
	while(attr_blk != -1)
	{
		h=getheader(attr_blk);
		unsigned char slotmap[h.numSlots];
		getSlotmap(slotmap,attr_blk);
		for(int i=0;i<20;i++)
      		{
        	    	     getRecord(attr,attr_blk,i);
		     if((char)slotmap[i]=='0')
		     {
			continue;
		     }
		     if(strcmp(attr[0].sval,tablename)==0) 
		     {
	              	type[attr[5].ival]=attr[2].ival;
		     } 
		}
		attr_blk =h.rblock;
	}
	//-----------------------------------------------------
	char *record=(char*) malloc(sizeof(char));
	int len=1;
          while(1)
	{
	 ch= fgetc(file);
	 if(ch==EOF)	
                   break;
           len=1;
	 while((ch  != '\n')&&(ch!=EOF)) 
	 {
        		 record[len-1]=ch;
       		 len++;
       		 record=(char*) realloc(record,(len)*sizeof(char));
                     ch=fgetc(file);
    	}
          record[len-1]='\0';
    	int i=0;
         //record contains each record in the file (seperated by commas)
          char record_array[count][16];
    	j=0;
	while(j<count)
	{ 	int k=0;
       		while(((record[i]!=',')&&(record[i]!='\0'))&&(k<16))
		{
	    		record_array[j][k++]=record[i++];        
		}
                    i++;
                    record_array[j][k]='\0';
                    j++;
	}
          union Attribute rec[count];
          for(int l=0;l<count;l++)
	{
	   	if(type[l]==FLOAT)
		       {
                           rec[l].fval=atof(record_array[l]);
		       }
		if(type[l]==INT)
		       {
                           rec[l].ival=strtoull(record_array[l],NULL,10);
		       }
		if(type[l]==STRING)
		       {
                           strcpy(rec[l].sval,record_array[l]);
		       }
	}
          int r; 
          r=ba_insert(relid,rec);

	
          //Assuming insert function returns 1 for successsful insert and 0 for unsuccessful insert
        
	 if(ch==EOF)	
                   break;
          }
          //closeRel(relid);
	fclose(file);
          return SUCCESS;
}

















