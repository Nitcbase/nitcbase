//ToDo 
//memset in getFreeslot 
//why are we not setting pblock and rblock after getFreeRecBlock in getFreeSlot
//ToDo: After getFreeslot we should update block allocation map if new block is allocated...needed in ba_insert anyway.
//Not sure about the order in compare () in linear search.should attrval be first argument?
// NOt updating blocktype,no of entries,p block etc after a new block is obtained. when to do it?

/*#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<bits/stdc++.h>*/
#include"disksimulator.cpp"

int getBlockType(int blocknum)
{
	FILE *disk=fopen("disk","rb");
	fseek(disk,0,SEEK_SET);
	unsigned char blockAllocationMap[4*BLOCK_SIZE];
	fread(blockAllocationMap,4*BLOCK_SIZE,1,disk);
	fclose(disk);
	return (int32_t)(blockAllocationMap[blocknum]);
	
}

struct HeadInfo getheader(int blockNum)
{
	//int BlockType=getBlockType(blockNum);
	struct HeadInfo header;
	FILE *disk=fopen("disk","rb");
	fseek(disk,blockNum*BLOCK_SIZE,SEEK_SET);
	fread(&header,32,1,disk);
	fclose(disk);
	return header;

}

int deleteBlock(int curr_block)
{
	FILE *disk;
	disk=fopen("disk","rb+");
	fseek(disk,BLOCK_SIZE*curr_block,SEEK_SET);
	unsigned char ch= 0;
	for(int i=0;i<BLOCK_SIZE;i++)
		fputc(0,disk);
	//fwrite(&ch,BLOCK_SIZE,1,disk);
	fseek(disk,curr_block,SEEK_SET);
	fputc((unsigned char)UNUSED_BLK,disk);
	fclose(disk);
}

void setheader(struct HeadInfo *header,int blockNum)
{
	FILE *disk=fopen("disk","rb+");
	fseek(disk,blockNum*BLOCK_SIZE,SEEK_SET);
	fwrite(header,32,1,disk);
	fclose(disk);
}



void getSlotmap(unsigned char * SlotMap,int blockNum)
{
	FILE *disk=fopen("disk","rb+");
	fseek(disk,blockNum*2048,SEEK_SET);
	struct recBlock R;
	fread(&R,2048,1,disk);
	int numSlots=R.numSlots;
	memcpy(SlotMap,R.slotMap_Records,numSlots);
	fclose(disk);
}

void setSlotmap(unsigned char * SlotMap,int no_of_slots,int blockNum)
{
	FILE *disk=fopen("disk","rb+");
	fseek(disk,blockNum*2048+32,SEEK_SET);
	fwrite(SlotMap,no_of_slots,1,disk);
	fclose(disk);
}

int getFreeBlock(int block_type)
{
	
	FILE *disk=fopen("disk","rb+");
	fseek(disk,0,SEEK_SET);
	unsigned char blockAllocationMap[4*BLOCK_SIZE];
	fread(blockAllocationMap,4*BLOCK_SIZE,1,disk);
	int iter;
	for(iter=0;iter<4*BLOCK_SIZE;iter++)
	{
		if((int32_t)(blockAllocationMap[iter])==UNUSED_BLK)
		{
			//cout<<"in getfreerecblk: "<<"\n";
			blockAllocationMap[iter]=(unsigned char)block_type;
			fseek(disk,0,SEEK_SET);
			fwrite(blockAllocationMap,2048*4,1,disk);
			fclose(disk);
			return iter;
		}
	}
	
	return FAILURE;

}

int getFreeRecBlock()
{
	
	FILE *disk=fopen("disk","rb+");
	fseek(disk,0,SEEK_SET);
	unsigned char blockAllocationMap[4*BLOCK_SIZE];
	fread(blockAllocationMap,4*BLOCK_SIZE,1,disk);
	int iter;
	for(iter=0;iter<4*BLOCK_SIZE;iter++)
	{
		if((int32_t)(blockAllocationMap[iter])==UNUSED_BLK)
		{
			//cout<<"in getfreerecblk: "<<"\n";
			blockAllocationMap[iter]=(unsigned char)REC;
			fseek(disk,0,SEEK_SET);
			fwrite(blockAllocationMap,2048*4,1,disk);
			fclose(disk);
			return iter;
		}
	}
	
	return FAILURE;

}

recId getFreeSlot(int block_num)
{
	int arg_blk=block_num;
	recId recid = {-1, -1};
	int prev_block_num, next_block_num;
	int num_slots;
	int iter,num_attrs;
	struct HeadInfo header;
	// finding free slot
	while(block_num != -1)
	{
		//rec_buffer = Buffer::getRecBuffer(block_num);
		//header = rec_buffer->getheader();
		header=getheader(block_num);
		num_slots = header.numSlots;
		next_block_num = header.rblock; 
		num_attrs=header.numAttrs;
		// next block
		//getting slotmap entries
		unsigned char slotmap[num_slots];
		getSlotmap(slotmap,block_num);
		//searching for free slot in block (block_num)
		int iter;
		for(iter = 0; iter< num_slots; iter++)
		{
			if(slotmap[iter] =='0')
			{
				break;
			}
		}
		if(iter < num_slots)
		{
			slotmap[iter]='1';
			setSlotmap(slotmap,num_slots,block_num);
			recid = {block_num, iter};
			return recid;
		}
		prev_block_num = block_num;
		block_num = next_block_num;
	}
	
	//no free slot is found in existing records if block_num = -1
	block_num =getFreeRecBlock();
	if(block_num ==-1)
	{
		// no blocks are available in disk
		return recid; // no free slot can be found
	}
	//block_num = rec_buffer->getBlockNum();
	//setting header values
	header = getheader(block_num);
	header.lblock = prev_block_num;
	header.numSlots = num_slots;
	header.rblock=-1;
	//was not there before: Athira
	header.numAttrs=num_attrs;
	setheader(&header,block_num);
	//setting slotmap
	unsigned char slotmap[num_slots];
	getSlotmap(slotmap,block_num);
	memset(slotmap,'0', sizeof(slotmap)); //all slots are free
          slotmap[0]='1';
	setSlotmap(slotmap,num_slots,block_num);
	//delete rec_buffer;
	//recid
	recid = {block_num, 0};
	//setting prev_block_num rblock to new block
	header=getheader(prev_block_num);
	header.rblock = block_num;
	setheader(&header,prev_block_num);
	return recid;
}



int getRecord(union Attribute *rec,int blockNum,int slotNum)
{ 
	struct HeadInfo Header;
	Header=getheader(blockNum);
	int numOfSlots=Header.numSlots;
	if(slotNum < 0 || slotNum > (numOfSlots - 1))
		return E_OUTOFBOUND;
	FILE *disk=fopen("disk","rb+");
	int BlockType=getBlockType(blockNum);
	if(BlockType==REC)
	{
		struct recBlock R;
		fseek(disk,blockNum*BLOCK_SIZE,SEEK_SET);
		fread(&R,BLOCK_SIZE,1,disk);
		int numSlots=R.numSlots;
		unsigned char slotMap[numSlots];

		if(*((int32_t*)(R.slotMap_Records+ slotNum)) == 0)
	    	return E_FREESLOT;
		int numAttrs=R.numAttrs;
		// it should be R.slotmprecords+32+...???
		memcpy(rec,(R.slotMap_Records+numSlots+(slotNum*numAttrs*ATTR_SIZE)), numAttrs*16);
		fclose(disk);
		return SUCCESS;

	}
	else if(BlockType==IND_INTERNAL)
	{
		//to be done

	}	
	else if(BlockType==IND_LEAF)
	{
		//to be done
	}
	else
	{
		fclose(disk);
		return FAILURE;
	}
	/*unsigned char* bufferPtr = getBufferPtr();
	int numOfAttrib=((int32_t) (bufferPtr + 5*4));
	int numOfSlots=((int32_t) (bufferPtr + 6*4));
	if(slotNum < 0 || slotNum > numOfSlots - 1)
		return E_OUTOFBOUND;  
	unsigned char *slotMap = new unsigned char;
	getSlotMap(slotMap);
	if((int32_t*)(slotMap + slotNum) == 0)
	    return E_FREESLOT;
	memcpy((void*)(bufferPtr + 32 + numOfSlots + (slotNum*numOfAttrib)ATTR_SIZE), (void)rec, numOfAttrib*ATTR_SIZE);
	return SUCCESS;*/
}

int setRecord(union Attribute *rec,int blockNum,int slotNum)
{ 
	//cout<<"setrrrr\n";
	struct HeadInfo header=getheader(blockNum);
	int numOfSlots=header.numSlots;
	int numAttrs=header.numAttrs;
	//cout<<numOfSlots;
	
	if(slotNum < 0 || slotNum > numOfSlots - 1)
		return E_OUTOFBOUND;
	//cout<<"hereee!!!\n";
	
	int BlockType=getBlockType(blockNum);
	//cout<<BlockType<<endl;
          //cout<<"BLOCK "<<blockNum<<"\n";
	FILE *disk=fopen("disk","rb+");
	if(BlockType==REC)
	{
		/*struct recBlock R;
		fseek(disk,blockNum*BLOCK_SIZE,SEEK_SET);
		fread(&R,BLOCK_SIZE,1,disk);
		int numAttrs=R.numAttrs;
		int numSlots=R.numSlots;*/
		//if(blockNum==5&&slotNum>11)
		fseek(disk,blockNum*BLOCK_SIZE+32+numOfSlots+slotNum*numAttrs*ATTR_SIZE,SEEK_SET);
		fwrite(rec,numAttrs*ATTR_SIZE,1,disk);
		fclose(disk);
		return SUCCESS;

	}
	else if(BlockType==IND_INTERNAL)
	{
		//to be done

	}	
	else if(BlockType==IND_LEAF)
	{
		//to be done
	}
	else
	{
		fclose(disk);
		return FAILURE;
	}
	/*unsigned char* bufferPtr = getBufferPtr();
	int numOfAttrib=((int32_t) (bufferPtr + 5*4));
	int numOfSlots=((int32_t) (bufferPtr + 6*4));
	if(slotNum < 0 || slotNum > numOfSlots - 1)
		return E_OUTOFBOUND;  
	unsigned char *slotMap = new unsigned char;
	getSlotMap(slotMap);
	if((int32_t*)(slotMap + slotNum) == 0)
	    return E_FREESLOT;
	memcpy((void*)(bufferPtr + 32 + numOfSlots + (slotNum*numOfAttrib)ATTR_SIZE), (void)rec, numOfAttrib*ATTR_SIZE);
	return SUCCESS;*/
}




int getRelCatEntry(int rel_id, union Attribute *relcat_entry)
{
	if(rel_id< 0 || rel_id>= MAXOPEN)
		return E_OUTOFBOUND;
	if(strcmp(OpenRelTable[rel_id],"NULL")==0)
		return E_NOTOPEN;
          char rel_name[16];
	strcpy(rel_name, OpenRelTable[rel_id]);
	for(int i=0;i<20;i++)
	{
		getRecord(relcat_entry,4,i);
		if(strcmp(relcat_entry[0].sval,rel_name)==0)
			return SUCCESS;
			
	}
	return FAILURE;	
}

 int setRelCatEntry(int rel_id,union Attribute * relcat_entry)
{
	if(rel_id< 0 || rel_id>= MAXOPEN)
		return E_OUTOFBOUND;
	if(strcmp(OpenRelTable[rel_id],"NULL")==0)
		return E_NOTOPEN;
	char rel_name[16];
	
	strcpy(rel_name, OpenRelTable[rel_id]);
	union Attribute  relcat_entry1[6];
	for(int i=0;i<20;i++)
	{
		getRecord(relcat_entry1,4,i);
		if(strcmp(relcat_entry1[0].sval,rel_name)==0)
		{	
			setRecord(relcat_entry,4,i);
			return SUCCESS;
		}			
	}
}

int getAttrCatEntry(int rel_id, char attrname[16],union Attribute *attrcat_entry)
{

	if(rel_id< 0 || rel_id>= MAXOPEN)
		return E_OUTOFBOUND;
	if(strcmp(OpenRelTable[rel_id],"NULL")==0)
		return E_NOTOPEN;
          char rel_name[16];
	strcpy(rel_name, OpenRelTable[rel_id]);
	int curr_block=5;
	int next_block=-1;
	while(curr_block!=-1)
	{
		struct HeadInfo header;
		header= getheader(curr_block);
		next_block=header.rblock;
		for(int i=0;i<20;i++)
		{
			getRecord(attrcat_entry,curr_block,i);
			//cout<<"rel name: "<<attrcat_entry[0].sval<<"\n";
			if(strcmp(attrcat_entry[0].sval,rel_name)==0)
			{
				if(strcmp(attrcat_entry[1].sval,attrname)==0)
					return SUCCESS;
			}
		}
		curr_block=next_block;
	}
	return E_ATTRNOTEXIST;
	
}


int ba_insert(int relid, union Attribute *rec)
{
	union Attribute attrcat_entry[6];
	struct HeadInfo header;
	union Attribute relcat_entry[6];

	getRelCatEntry(relid, relcat_entry);
	int first_block = relcat_entry[3].ival;
	int num_slots = relcat_entry[5].ival;
	int num_attrs = relcat_entry[1].ival;
	unsigned char slotmap[num_slots];
	int blkno=first_block;

	if(first_block==-1)
	{        
	          blkno=getFreeRecBlock();
		relcat_entry[3].ival=blkno;
		struct HeadInfo *H=(struct HeadInfo*)malloc(sizeof(struct HeadInfo));
		H->blockType=REC;
		H->pblock=-1;
		H->lblock=-1;
		H->rblock=-1;
		H->numEntries=0;
		H->numAttrs=num_attrs; 
		H->numSlots=num_slots;
                    setheader(H,blkno);
		getSlotmap(slotmap,blkno);
	          memset(slotmap,'0', sizeof(slotmap)); //all slots are free
                    setSlotmap(slotmap,num_slots,blkno);
	}
	
	recId recid = getFreeSlot(blkno);
	
	relcat_entry[4].ival=recid.block;
	//cout<<"blk no: "<<relcat_entry[4].ival<<" ";
	setRelCatEntry(relid, relcat_entry);
          getRelCatEntry(relid, relcat_entry);
          //cout<<relcat_entry[3].ival<<"\n";
           if(recid.block == -1 && recid.slot == -1)
	{         //free slot can not be found
		return FAILURE;
	}
	setRecord(rec,recid.block, recid.slot);
	
	//since record is inserted number of entries is increased by 1
	header= getheader(recid.block);  //arg
	header.numEntries = header.numEntries + 1; // increased number of entires in block
	setheader(&header,recid.block); //arg
	
	//increasing number of entries in relation catalog entry
	relcat_entry[2].ival = relcat_entry[2].ival + 1;
	setRelCatEntry(relid, relcat_entry);
	
	return SUCCESS;
}

int compare(union Attribute attr1, union Attribute attr2, int attrType)
{
    if (attrType == STRING)
    {
       //cout<<"compareeeee"<<endl;
       //cout<<attr1.sval<<" "<<attr2.sval;
       return strcmp(attr1.sval, attr2.sval); 
    }

    /* else if attrType == INT -1, 0, 1 depending on whether the value attr1.ival
       is less than, equal to or greater than the value attr2.ival */ 
    
    /* else if attrType==FLOAT return -1, 0, 1 depending on whether the value attr1.fval
       is less than, equal to or greater than the value attr2.fval */ 

    if (attrType == INT)
    {
    	if(attr1.ival<attr2.ival)
    		return -1;
    	else if(attr1.ival==attr2.ival)
    		return 0;
    	else
    		return 1;
    }

    if(attrType==FLOAT)
    {
    	if(attr1.fval<attr2.fval)
    		return -1;
    	else if(attr1.fval==attr2.fval)
    		return 0;
    	else
    		return 1;
    }
    
}

recId linear_search(relId relid, char attrName[ATTR_SIZE], union Attribute attrval, int op, recId * prev_recid)
{
	//get the record corresponding to the relation name
	union Attribute relcat_entry[6];
	getRelCatEntry(relid,relcat_entry);
	//cout<<"linear search relid is"<<relid<<endl;
	//cout<<prev_recid->block<<prev_recid->slot<<endl;
	/*cout<<relcat_entry[0].sval<<endl;
	cout<<relcat_entry[1].ival<<endl;
	cout<<relcat_entry[2].ival<<endl;
	cout<<relcat_entry[3].ival<<endl;*/
	int offset,attr_type;
	
	//get the record itself in relcat_entry array of attributes
	int curr_block,curr_slot,next_block=-1;
	int no_of_attributes=relcat_entry[1].ival;
	int no_of_slots=relcat_entry[5].ival;
	//cout<<"slots"<<no_of_slots<<endl;
	if(op!=PRJCT)
	{	union Attribute attrcat_entry[6];
		getAttrCatEntry(relid,attrName,attrcat_entry);
		offset= attrcat_entry[5].ival;
		attr_type=attrcat_entry[2].ival;
	}
	recId ret_recid;

	if((prev_recid->block==-1)&& prev_recid->slot==-1)
	{
		//cout<<"yessss"<<endl;
		curr_block = relcat_entry[3].ival;
		curr_slot = 0;
		//cout<<curr_block<<curr_slot<<endl;
	}
	
	else
	{ 
		//if the linear search knows the  hit from previous search
		 curr_block = prev_recid->block;
		 curr_slot = prev_recid->slot+1;
	}
	unsigned char slotmap[no_of_slots];
	//cout<<"In linear searchhh"<<curr_block;
	while(curr_block!=-1)
	{
		int i=curr_slot;
		struct HeadInfo header;
		header= getheader(curr_block);
		next_block=header.rblock;
		getSlotmap(slotmap,curr_block);
		for(;i<no_of_slots;i++)
		{
			//cout<<"iiiii"<<endl;
			union Attribute record[no_of_attributes];
			if(slotmap[i]=='0')
			{
				//cout<<"skippinggg"<<endl;
				continue;
			}	
			//cout<<"before get record "<<curr_block<<" "<<i<<endl;
			//cout<<"$"<<endl;
			getRecord(record,curr_block,i);
			bool cond=false;
			/*if(retval==E_FREESLOT)
			{
				continue;
			}*/
			//cout<<"attr type"<<attr_type<<endl;
			//cout<<"record[offset] "<<record[0].sval<<endl;
			if(op!=PRJCT)
			{
				//cout<<"op"<<op<<endl;
				int flag=compare(record[offset],attrval,attr_type);
			//cout<<"flaag   "<<flag<<endl;
			
			switch(op)
			{
				case NE: 
							if(flag != 0)
								cond=true;
							break;
				case LT: 
							if(flag < 0)
								cond=true;
							break;
				case LE: 
							if(flag <= 0)
								cond=true;
							break;
				case EQ: 
							if(flag == 0)
								cond=true;
							break;
				case GT: 
							if(flag > 0)
								cond=true;
							break;
				case GE: 
							if(flag >= 0)
								cond=true;
			}
			}
			
			if(cond==true||op==PRJCT)
			{
				//record id of the record that satisfies the given condition
				//cout<<"hereeeeeeee"	<<curr_block<<i<<endl;
				ret_recid = {curr_block, i}; 
				//set the prev_recid 		
				*prev_recid= ret_recid;
				return ret_recid;
			}
		}
		curr_block=next_block;
		curr_slot=0;
	}
	
	return {-1, -1}; //i.e., no record in the relation with Id relid satisfies the given condition
}		

int ba_renamerel(char oldName[ATTR_SIZE], char newName[ATTR_SIZE])
{
	//search for the relation with name newName in relation catalog 
	recId prev_recid={-1,-1};
	recId relcat_recid, attrcat_recid;
	union Attribute temp;
	strcpy(temp.sval,newName);
	relcat_recid = linear_search(RELCAT_RELID, "RelName",temp, EQ,&prev_recid);
	//If relation with name newName already exits
	if(!((relcat_recid.block==-1)&&(relcat_recid.slot==-1)))
		return E_RELEXIST;
	
	
	//search for the relation with name oldName in relation catalog 
	prev_recid={-1,-1};
	strcpy(temp.sval,oldName);
	relcat_recid = linear_search(RELCAT_RELID, "RelName",temp, EQ, &prev_recid);
	//If relation with name relName does not exits
	if((relcat_recid.block==-1)&&(relcat_recid.slot==-1))
		return E_RELNOTEXIST;
	
	//get the relation catalog record from the relation catalog (recid of the relation catalog record = relcat_recid)
	union Attribute relcat_record[6];
	getRecord(relcat_record,relcat_recid.block,relcat_recid.slot);
	strcpy(relcat_record[0].sval,newName);
	
	//update the relation catalog record in the relation catalog with relation name newName
	setRecord(relcat_record, relcat_recid.block,relcat_recid.slot);

	/*update all the attribute catalog entries in the attribute catalog corresponding to the 
	  relation with relation name oldName to the relation name newName*/
	prev_recid={-1,-1};
	while(1)
	{
		attrcat_recid = linear_search(ATTRCAT_RELID, "RelName", temp, EQ,&prev_recid);
		if(!((attrcat_recid.block==-1)&&(attrcat_recid.slot==-1)))
		{
			union Attribute attrcat_record[6];
			getRecord(attrcat_record,attrcat_recid.block,attrcat_recid.slot);
			if(strcmp(attrcat_record[0].sval,oldName)==0)
			{
				strcpy(attrcat_record[0].sval,newName);
				setRecord(attrcat_record,attrcat_recid.block,attrcat_recid.slot);
			}
		}
		else
			break;
	}
	
	return SUCCESS;
}

int ba_renameattr(char relName[ATTR_SIZE], char oldName[ATTR_SIZE], char newName[ATTR_SIZE])
{
	//search for the relation with name relName in relation catalog 
	recId prev_recid={-1,-1};
	recId relcat_recid,attrcat_recid;
	union Attribute attrcat_record[6];
	union Attribute temp;
	strcpy(temp.sval,relName);
	relcat_recid = linear_search(RELCAT_RELID, "RelName",temp, EQ,&prev_recid);
	
	if((relcat_recid.block==-1) && (relcat_recid.slot==-1))
	{ //If relation with name relName does not exits
		return E_RELNOTEXIST;
	}
	
	//Iterate over all the attributes corresponding to the relation{
		//search for the attributes with relation name relName in attribute catalog 
	prev_recid={-1,-1};
	 while(1)
	{
	 	attrcat_recid = linear_search(ATTRCAT_RELID, "RelName",temp, EQ,&prev_recid);
	 	if(!((attrcat_recid.block==-1) && (attrcat_recid.slot==-1)))
	 	{
	 		getRecord(attrcat_record,attrcat_recid.block,attrcat_recid.slot);
	   		if(strcmp(attrcat_record[1].sval,newName)==0)
	  			return E_ATTREXIST;
	 	}
	   	else
	   		break;

    }
	
	//Iterate over all the attributes corresponding to the relation{
	//search for the attributes with relation name relName in attribute catalog 
     
    prev_recid={-1,-1};
	 while(1)
	{
	 	attrcat_recid = linear_search(ATTRCAT_RELID, "RelName",temp, EQ, &prev_recid);
	 	if(!((attrcat_recid.block==-1) && (attrcat_recid.slot==-1)))
	 	{
	 		getRecord(attrcat_record,attrcat_recid.block,attrcat_recid.slot);
	   		if(strcmp(attrcat_record[1].sval,oldName)==0)
	  		{
	  			strcpy(attrcat_record[1].sval,newName);
	  			setRecord(attrcat_record,attrcat_recid.block,attrcat_recid.slot);
	  			return SUCCESS;
	  		}

	 	}
	   	else
	  		return E_ATTRNOTEXIST;

    }
}	


//Todo: updating last block of attribute catalog
int ba_delete(char relName[ATTR_SIZE])
{
	for(int ind=0;ind<12;ind++)
      	{  
	if(strcmp(relName,OpenRelTable[ind])==0)
           	{
			cout<<"Cannot delete relation which is open\n";
			return FAILURE;
		}
         }
	/* search for relation with name relName in relation catalog */
	recId relcat_recid,prev_recid={-1,-1};
	union Attribute temp;
	strcpy(temp.sval,relName);
	//cout<<"called from delete\n";
	relcat_recid = linear_search(RELCAT_RELID, "RelName",temp, EQ,&prev_recid);
	
	if((relcat_recid.block==-1) && (relcat_recid.slot==-1))
	{ //If relation with relName does not exits
		//cout<<"ereldoesnotexist\n";
		return E_RELNOTEXIST;
	}
	
	union Attribute relcat_rec[6];
	/*getting the relation catalog entry corresponding to relation with relName*/
	getRecord(relcat_rec, relcat_recid.block,relcat_recid.slot);

	//get the first record block of the relation (first_block)
	int curr_block= relcat_rec[3].ival;
	int no_of_slots=relcat_rec[5].ival;
	int no_of_attrs=relcat_rec[1].ival;
	int next_block=-1;

    //Delete all the record blocks of the relation by getting the next record blocks (rblock) from header and by calling
	while(curr_block!=-1)
	{
		struct HeadInfo header=getheader(curr_block);
		next_block=header.rblock;
		deleteBlock(curr_block);
		curr_block=next_block;
	}
	union Attribute attrcat_rec[6]; recId attrcat_recid;
	prev_recid={-1,-1};unsigned char slotmap[20];int root_block;
	union Attribute a[6];
	//getRecord(a,5,16);
	/*cout<<"dddddd\n";
	cout<<a[0].sval<<endl;*/
	for(int i=0;i<no_of_attrs;i++)
	{
		//cout<<"$$$attr$$$"<<endl;
		//cout<<prev_recid.block<<prev_recid.slot<<endl;
		//getRecord(a,5,16);
	    //cout<<"dddddd\n";
	    //cout<<a[0].sval<<endl;
		attrcat_recid= linear_search(ATTRCAT_RELID, "RelName",temp, EQ,&prev_recid);
		//cout<<"@@@@@@@Before disk read"<<endl;
		//getRecord(a,5,16);
	    /*cout<<"dddddd\n";
	    cout<<a[0].sval<<endl;*/
		FILE *disk=fopen("disk","rb+");
		//cout<<"attrcat recid values are"<<attrcat_recid.block<<" "<<attrcat_recid.slot;
		//cout<<"slots attrs"<<no_of_slots<<no_of_attrs<<endl;
		fseek(disk,(attrcat_recid.block)*BLOCK_SIZE+32+20+(attrcat_recid.slot)*6*ATTR_SIZE,SEEK_SET);
		unsigned char ch=0;
		for(int i=0;i<16*6;i++)
			fputc(0,disk);
		//cout<<"@@@@@@@@@@After write"<<endl;
		fclose(disk);
		//getRecord(a,5,16);
	    //cout<<"dddddd\n";
	    //cout<<a[0].sval<<endl;
		struct HeadInfo header=getheader(attrcat_recid.block);
		getSlotmap(slotmap,attrcat_recid.block);
		slotmap[attrcat_recid.slot]='0';
		setSlotmap(slotmap,20,attrcat_recid.block);
		header.numEntries=header.numEntries-1;
		setheader(&header,attrcat_recid.block);	
		
		root_block=attrcat_rec[4].ival;
		//if(root_block != -1)//index exists for the attribute
			//bplus_destroy(root_block); //delete the index blocks corresponding to the attribute	
		if(header.numEntries==0)
		{
			//header.lblock will never be -1
			struct HeadInfo prev_header=getheader(header.lblock);
			prev_header.rblock=header.rblock;
			setheader(&prev_header,header.lblock);
			if(header.rblock!=-1)
			{
				struct HeadInfo next_header=getheader(header.rblock);
				next_header.lblock=header.lblock;
				setheader(&next_header,header.rblock);
			}
			deleteBlock(attrcat_recid.block);
			continue;
			
		}


		
	}
	
	
	//delete the relation catalog entry corresponding to the relation from relation catalog
	//Adjusting the number of entries in the block (decrease by 1) corresponding to the relation catalog entry
	
	//update relation catalog (i.e number of records in relation catalog is decreased by 1)
	//update attribute catalog (i.e number of records in attribute catalog is decreased by num_attrs)
	
	unsigned char relcat_slotmap[20];
	getSlotmap(relcat_slotmap,4);
	relcat_slotmap[relcat_recid.slot]='0';
	setSlotmap(relcat_slotmap,20,relcat_recid.block);
	struct HeadInfo relcat_header=getheader(4);
	relcat_header.numEntries=relcat_header.numEntries-1;
	setheader(&relcat_header,4);

	getRecord(relcat_rec,4,0);
	relcat_rec[2].ival=relcat_rec[2].ival-1;
	setRecord(relcat_rec,4,0);
	getRecord(relcat_rec,4,1);
	relcat_rec[2].ival=relcat_rec[2].ival-no_of_attrs;
	setRecord(relcat_rec,4,1);
	FILE *disk=fopen("disk","rb+");
	fseek(disk,(relcat_recid.block)*BLOCK_SIZE+32+20+relcat_recid.slot*6*ATTR_SIZE,SEEK_SET);
	for(int i=0;i<16*6;i++)
		fputc(0,disk);
	fclose(disk);
	//cout<<"successfully exited ba_delete"<<endl;
	return SUCCESS;
}	 


int ba_search(relId relid, union Attribute *record, char attrName[ATTR_SIZE], union Attribute attrval, int op,recId * prev_recid)
{
	
	/*get the attribute catalog entry from the attribute cache corresponding 
	  to the relation with Id=relid and with attribute_name=attrName using
	  OpenRelTable::getAttrCatEntry(relid, attrName, &attrcat_entry); of cache layer */
	//get root_block from the attribute catalog entry (attrcat_entry)
	recId recid;
	if(op==PRJCT)
	{
		//cout<<"hereee in ba search"<<endl;
		recid = linear_search(relid, attrName, attrval, op,prev_recid);

	}
	else
	{
		union Attribute attrcat_entry[6];
		getAttrCatEntry(relid,attrName,attrcat_entry);
		int root_block=attrcat_entry[4].ival;
	
		if(root_block == -1)
		{ 	//if Index does not exist for the attribute
			//search for the record id (recid) correspoding to the attribute with attribute name attrName and with value attrval  
			recid = linear_search(relid, attrName, attrval, op,prev_recid);
		}
		else
		{ 	//if Index exists for the attribute
	  		//search for the record id (recid) correspoding to the attribute with attribute name attrName and with value attrval
			//recid = bplus_search(relid, attrName, attrval, op,&prev_recid);
		}
	}
	
	
	if((recid.block==-1) && (recid.slot==-1))
	{ //if it fails to find a record satisfying the given condition
		return FAILURE;
	}
	
	 //recid.block is the block that contains record
	getRecord(record, recid.block,recid.slot); //recid.slot is the slot that contains record
	//cout<<"successful ba search"<<endl;
	return SUCCESS;
}


void meta()
{
          struct HeadInfo h;
	union Attribute rec[6];
	struct HeadInfo *H=(struct HeadInfo*)malloc(sizeof(struct HeadInfo));
	//unsigned char *slot_map=(unsigned char*)malloc(sizeof(slot_map));
    
	H->blockType=REC;
	H->pblock=-1;
	H->lblock=-1;
	H->rblock=-1;
	H->numEntries=2;
	H->numAttrs=6; 
	H->numSlots=20;
	setheader(H,4);
	
	unsigned char slot_map[20];
	for(int i=0;i<20;i++)
	{
		if(i==0||i==1)
			slot_map[i]='1';
		else
			slot_map[i]='0';
	}
	setSlotmap(slot_map,20,4);
	unsigned char temp[20];
	/*FILE * disk=fopen("disk","rb+");
	fseek(disk,4*2048+32,SEEK_SET);
	fread(temp,20,1,disk);
	for(int i=0;i<20;i++)
		if(temp[i]=='1')
			cout<<"hi!!";
	cout<<endl;*/
	/*fseek(disk,4*2048,SEEK_SET);
	fread(&h,32,1,disk);
	cout<<h.numSlots<<endl;*/

	strcpy(rec[0].sval,"RELATIONCAT");
          rec[1].ival=6;
	rec[2].ival=2;
	rec[3].ival=4;
	rec[4].ival=4;
	rec[5].ival=20;
	setRecord(rec,4,0);
	
	strcpy(rec[0].sval,"ATTRIBUTECAT");
          rec[1].ival=6;
	rec[2].ival=12;
	rec[3].ival=5;
	rec[4].ival=5;
	rec[5].ival=20;
	setRecord(rec,4,1);
	//cout<<"setrecord done\n";
	H->blockType=REC;
	H->pblock=-1;
	H->lblock=-1;
	H->rblock=-1;
	H->numEntries=12;
	H->numAttrs=6; 
	H->numSlots=20;
	setheader(H,5);
	//cout<<"set header\n";
	

	for(int i=0;i<20;i++)
	{
		if(i>=0&&i<=11)
			slot_map[i]='1';
		else
			slot_map[i]='0';
	}
	setSlotmap(slot_map,20,5);
	//cout<<"wrote slotmap\n";
	
    strcpy(rec[0].sval,"RELATIONCAT");
    strcpy(rec[1].sval,"RelName");
	rec[2].ival=STRING;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=0;
	setRecord(rec,5,0);
	//cout<<"set record done\n";
         

	strcpy(rec[0].sval,"RELATIONCAT");
    strcpy(rec[1].sval,"#Attributes");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=1;
	setRecord(rec,5,1);
	//cout<<"set record done\n";

	strcpy(rec[0].sval,"RELATIONCAT");
    strcpy(rec[1].sval,"#Records");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=2;
	setRecord(rec,5,2);
	//cout<<"set record done\n";
	
	strcpy(rec[0].sval,"RELATIONCAT");
    strcpy(rec[1].sval,"FirstBlock");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=3;
	setRecord(rec,5,3);
	//cout<<"set record done\n";
	
	strcpy(rec[0].sval,"RELATIONCAT");
    strcpy(rec[1].sval,"LastBlock");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=4;
	setRecord(rec,5,4);
	//cout<<"set record done\n";
	
    strcpy(rec[0].sval,"RELATIONCAT");
    strcpy(rec[1].sval,"#Slots");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=5;
	setRecord(rec,5,5);
	//cout<<"set record done\n";
	
	strcpy(rec[0].sval,"ATTRIBUTECAT");
    strcpy(rec[1].sval,"RelName");
	rec[2].ival=STRING;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=0;
	setRecord(rec,5,6);
	//cout<<"set record done\n";
	
	strcpy(rec[0].sval,"ATTRIBUTECAT");
    strcpy(rec[1].sval,"AttributeName");
	rec[2].ival=STRING;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=1;
	setRecord(rec,5,7);
	//cout<<"set record done\n";

	strcpy(rec[0].sval,"ATTRIBUTECAT");
    strcpy(rec[1].sval,"AttributeType");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=2;
	setRecord(rec,5,8);
	//cout<<"set record done\n";


	strcpy(rec[0].sval,"ATTRIBUTECAT");
    strcpy(rec[1].sval,"PrimaryFlag");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=3;
	setRecord(rec,5,9);
	//cout<<"set record done\n";
	
	strcpy(rec[0].sval,"ATTRIBUTECAT");
    strcpy(rec[1].sval,"RootBlock");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=4;
	setRecord(rec,5,10);
	//cout<<"set record done\n";

	strcpy(rec[0].sval,"ATTRIBUTECAT");
    strcpy(rec[1].sval,"Offset");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=5;
	setRecord(rec,5,11);
	//cout<<"set record done\n
	
}

