#include "block_access.cpp"
struct InternalEntry getEntry(int block,int iNo)
{
	struct InternalEntry rec;
	FILE *disk=fopen("disk","rb");
	fseek(disk,block*BLOCK_SIZE+32+iNo*20,SEEK_SET);
	fread(&rec,sizeof(rec),1,disk);
	fclose(disk);
	return rec;
}

int setAttrCatEntry(int rel_id, char attrname[16],union Attribute attrcat_entry[6])
{
	union Attribute attr_entry[6];
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
			getRecord(attr_entry,curr_block,i);
			//cout<<"rel name: "<<attrcat_entry[0].sval<<"\n";
			if(strcmp(attr_entry[0].sval,rel_name)==0)
			{
				if(strcmp(attr_entry[1].sval,attrname)==0)
				{
					setRecord(attrcat_entry,curr_block,i);
					return SUCCESS;
					
				}
			}
		}
		curr_block=next_block;
	}
	return E_ATTRNOTEXIST;
}

int setEntry(struct InternalEntry internalentry,int block,int offset)
{         
	if((internalentry.lChild==block)||(internalentry.rChild==block))
	{
		struct InternalEntry entry;	
		entry=getEntry(block,offset);
		if(internalentry.attrVal.ival==entry.attrVal.ival)
			return SUCCESS;
		if(internalentry.lChild==block)
			internalentry.lChild=entry.lChild;
		else
			internalentry.rChild=entry.rChild;
	}

	//cout<<"In internal\n";
	//cout<<internalentry.attrVal.ival<<" "<<internalentry.lChild<<" "<<internalentry.rChild<<"\n"<<block<<" "<<offset<<"\n\n";
	FILE *disk=fopen("disk","rb+");
	fseek(disk,block*BLOCK_SIZE+32+offset*20,SEEK_SET);
	fwrite(&internalentry,sizeof(internalentry),1,disk);
	fclose(disk);
	return SUCCESS;
}


int setLeafEntry(struct Index rec,int leaf,int offset)
{
	//cout<<"Leaf Node:\n";
	//cout<<"Value:"<<rec.attrVal.ival<<" "<<rec.block<<" "<<rec.slot<<"\nLeaf Block:"<<leaf<<" "<<offset<<"\n\n";
	FILE *disk=fopen("disk","rb+");
	fseek(disk,leaf*BLOCK_SIZE+32+offset*32,SEEK_SET);
	fwrite(&rec,sizeof(rec),1,disk);
	fclose(disk);
	return SUCCESS;
}

struct Index getLeafEntry(int leaf,int offset)
{
	struct Index rec;
	FILE *disk=fopen("disk","rb");
	fseek(disk,leaf*BLOCK_SIZE+32+offset*32,SEEK_SET);
	fread(&rec,sizeof(rec),1,disk);
	fclose(disk);
	return rec;
}

int bplus_insert(int relid,char attrname[ATTR_SIZE],union Attribute val,recId rec_id)
{
	//cout<<val.ival<<"\n";
	//get the AttrCatEntry of target attribute
	union Attribute attrcatentry[6];
	if(getAttrCatEntry(relid,attrname,attrcatentry)!=SUCCESS)
		return FAILURE;
	int root_block=attrcatentry[4].ival;
          int attrtype=attrcatentry[2].ival;
	
	//if root_block_num of target attribute is -1, return E_NOINDEX
	if(root_block==-1)
		return E_NOINDEX;
	int head=root_block;
	int blocktype=getBlockType(head);
	struct HeadInfo header;
	//cout<<"Blk "<<head<<"\n";
	//cout<<blocktype<<"\n";
	int nentries,i;
	int k=0;
	while(blocktype!=IND_LEAF) //as long as the entry is not a leaf
 	{
		k++;
  		header=getheader(head);
  		nentries=header.numEntries;
   		struct InternalEntry internalentry;
    		for(i=0;i<nentries;++i)
          	{
            		 internalentry=getEntry(head,i);
			 if(attrtype==INT)
	    		 {
				 if(internalentry.attrVal.ival>val.ival) 
              		     		break;
			 }
	   		 else if(attrtype==FLOAT)
	     		 {
				 if(internalentry.attrVal.fval>val.fval) 
              		    		 break;
	    		 }
	   		 else if(attrtype==STRING)
	   		 {
				 if(internalentry.attrVal.sval>val.sval) 
              		   		  break;
	     		 }
        		}
        		 if(i==nentries)
          	{
          		 head=internalentry.rChild; //val is greater than all internal entries
            		 blocktype=getBlockType(head);
       		}
       		else// an offset is found
          	{
    			head=internalentry.lChild;
     			blocktype=getBlockType(head);
          	}
		//cout<<"Head : "<<head<<" "<<blocktype<<" "<<IND_LEAF<<"\n";
		//if(val.ival==98&&k!=0)
		//	return -1;
          }
       header=getheader(head);
       nentries=header.numEntries;

    //  cout<<nentries<<"\n";
       struct Index indices[nentries+1];
       struct Index leafentry;
       int c=0, flag=0;
       for(i=0;i<nentries;++i)
       {
  	     leafentry= getLeafEntry(head,i);
	    // cout<<leafentry.attrVal.ival<<"\n";
               if(flag==0)
               {
		if(attrtype==INT)
	          {	if(val.ival<leafentry.attrVal.ival)
			{
		 		indices[c].attrVal.ival=val.ival;
				indices[c].block=rec_id.block;
       				indices[c].slot=rec_id.slot;
       				flag=1;
       				c++;
			}	
	    	}
	     	else if(attrtype==FLOAT)
	     	{	if(val.fval<leafentry.attrVal.fval)
		 	{	
				indices[c].attrVal.fval=val.fval;
				indices[c].block=rec_id.block;
       				indices[c].slot=rec_id.slot;
       				flag=1;
       				c++;
			}
	     	}
	          else if(attrtype==STRING)
	    	{	if(val.sval<leafentry.attrVal.sval)
			{
				strcpy(indices[c].attrVal.sval,val.sval);
				indices[c].block=rec_id.block;
       				indices[c].slot=rec_id.slot;
       				flag=1;
       				c++;
			}
	     	}	
		//cout<<c<<" "<<indices[c].attrVal.ival<<"\n";
      	     }
	      if(attrtype==INT)
	     {
		 indices[c].attrVal.ival=leafentry.attrVal.ival;
	     }
	     else if(attrtype==FLOAT)
	     {
		 indices[c].attrVal.fval=leafentry.attrVal.fval;
	     }
	     else if(attrtype==STRING)
	     {
		strcpy(indices[c].attrVal.sval,leafentry.attrVal.sval);
	     }
	     //cout<<c<<" "<<indices[c].attrVal.ival<<"\n";
               indices[c].block=leafentry.block;
               indices[c].slot= leafentry.slot;
               c++;
       }
       
       int type_val=attrtype;
       if(nentries==c)
       {
	if(type_val==INT)
		indices[c].attrVal.ival=val.ival;
	if(type_val==FLOAT)
		indices[c].attrVal.fval=val.fval;
          if(type_val==STRING)
		strcpy(indices[c].attrVal.sval,val.sval);
       	indices[i].block=rec_id.block;
       	indices[i].slot=rec_id.slot;
	c++;
       }

       if(nentries!=63)
       {
	     for(i=0;i<c;++i)
               {
    	          setLeafEntry(indices[i],head,i);
              }

              header.numEntries=header.numEntries+1;
              setheader(&header,head);
              return SUCCESS;
       }
       else
       {
   	    int leaf=getFreeBlock(IND_LEAF);
	    for(i=0;i<32;i++)
              {
    	         if(setLeafEntry(indices[i],head,i)!=SUCCESS)
    		    return FAILURE;
              }  
              for(int j=0;j<32;j++)
              {
      		if(setLeafEntry(indices[i],leaf,j)!=SUCCESS)
    		    return FAILURE;
                    i++;
              }
	    indices[0].attrVal.ival=0;
              indices[0].block=0;
              indices[0].slot= 0;
	    for(i=32;i<63;i++)
              {
    	         if(setLeafEntry(indices[0],head,i)!=SUCCESS)
    		    return FAILURE;
              }
              header.numEntries=32;
    	    //to maintain a linked list of leaf blocks
    	    header.rblock=leaf;
    	    setheader(&header,head);
	    struct HeadInfo newblkheader;
              newblkheader.numEntries=32;
              newblkheader.lblock=head;  
              int headparblock= header.pblock;
              newblkheader.pblock=headparblock;
	    newblkheader.rblock=header.rblock;
              setheader(&newblkheader,leaf);
            
             //new attrval is the value to be inserted into internal block above
             struct Index leafentry;
	   leafentry=getLeafEntry(head,31);
             union Attribute newattrval;
	   if(type_val==INT)
		newattrval.ival=leafentry.attrVal.ival;
	   if(type_val==FLOAT)
		newattrval.fval=leafentry.attrVal.fval;
             if(type_val==STRING)
		strcpy(newattrval.sval,leafentry.attrVal.sval);

       	   bool done = false;
     	   int newchild=leaf;
   	   while(!done)
    	   {
    		  if (headparblock!=-1)
      		  {
       			 struct HeadInfo parheader=getheader(headparblock);
         			 struct InternalEntry internal_entries[parheader.numEntries + 1];       
       
			 struct InternalEntry internalentry;
        			 flag=0;
           		 c=0;
        			 for (i = 0; i < parheader.numEntries; ++i)
        	 		 {
         				 internalentry=getEntry(headparblock,i);
				 if(attrtype==INT)
				 {
          			 if(internalentry.attrVal.ival>newattrval.ival && flag==0)//change this by using ival,fval,sval
          			 {
          				  internal_entries[c].attrVal.ival=newattrval.ival;
					  internal_entries[c].lChild=internalentry.lChild;
					  internal_entries[c].rChild=newchild;           
          				  flag=1;
         				            c++;
        				 }
				 }
				 if(attrtype==FLOAT)
				 {
          			 if(internalentry.attrVal.fval>newattrval.fval && flag==0)//change this by using ival,fval,sval
          			 {
          				  internal_entries[c].attrVal.fval=newattrval.fval;
           				  internal_entries[c].lChild=internalentry.lChild;
					  internal_entries[c].rChild=newchild;           
          				  flag=1;
         				            c++;
        				 }
				 }
				 if(attrtype==STRING)
				 {
          			 if(internalentry.attrVal.sval>newattrval.sval && flag==0)//change this by using ival,fval,sval
          			 {
          				  strcpy(internal_entries[c].attrVal.sval,newattrval.sval);
           				  internal_entries[c].lChild=internalentry.lChild;
					  internal_entries[c].rChild=newchild;           
          				  flag=1;
         				            c++;
        				 }
				 }
        				 if(c-1>=0)
        				 {
         					 internal_entries[c].lChild=internal_entries[c-1].rChild;
         				 }
          			 else
        				 {
        					 internal_entries[c].lChild=internalentry.lChild;
        				 }
          			 internal_entries[c].attrVal=internalentry.attrVal;
          			 internal_entries[c].rChild=internalentry.rChild;
         				 c++;
			 }
			 if(flag==0)//when newattrval is greater than all parentblock enries
       			 {
         				 internal_entries[c].lChild=internal_entries[c-1].rChild;
          			 internal_entries[c].attrVal=newattrval;
          			 internal_entries[c].rChild=newchild;
          			 flag=1;
       			 }
	
       			 if(parheader.numEntries!=100)
        		 	 {
         				 for (i= 0; i < (parheader.numEntries+1); ++i)
          			{
          				if(setEntry(internal_entries[i],headparblock,i)!=0)
          					return -1;         
          			}
          			parheader.numEntries=parheader.numEntries+1;
                                        setheader(&parheader,headparblock);
          			done=true;
			}
			else //if parentblock is full
        			{
         				 newchild=getFreeBlock(1); // to get a new internalblock
         				 //copy first 2 entries(0 to 1) to old internal block
           			 for (i = 0; i <50; ++i)
         				 {
					if((internal_entries[i].lChild==headparblock)||(internal_entries[i].rChild==headparblock))
					{
					struct InternalEntry entry;	
					entry=getEntry(headparblock,i);
					if(internalentry.attrVal.ival==entry.attrVal.ival)
						continue;
					if(internal_entries[i].lChild==headparblock)
						 internal_entries[i].lChild=entry.lChild;
					else
						internal_entries[i].rChild=entry.rChild;
					}
          				if(setEntry(internal_entries[i],headparblock,i)!=SUCCESS)
          					return FAILURE;      
          			}
         				parheader.numEntries=50;
          			setheader(&parheader,headparblock);
				i=51;
         				//copy last 2 entries (3 to 4) into new internal block
          			for (int j = 0; j< 50; ++j)
          			{
          				if(setEntry(internal_entries[i],newchild,j)!=SUCCESS)
        		  				return FAILURE;          
            				i++;
          			}

          			newblkheader.numEntries=50;     
          			head=headparblock;
          			headparblock=parheader.pblock;
          			newblkheader.pblock=parheader.pblock;
         				setheader(&parheader,newchild);
          			newattrval=internal_entries[50].attrVal;
				
			}
	}
          else //if headparblock==-1 i.e root is split now
          {
        		int newrootblock=getFreeBlock(IND_INTERNAL);//get new internal block
        		struct InternalEntry internalentry;
		if(type_val==INT)
			internalentry.attrVal.ival=newattrval.ival;
		if(type_val==FLOAT)
			internalentry.attrVal.fval=newattrval.fval;
          	if(type_val==STRING)
			strcpy(internalentry.attrVal.sval,newattrval.sval);
		//cout<<"New block "<<newrootblock<<"\n";
        		internalentry.lChild=head;
       		internalentry.rChild=newchild;
       		if(setEntry(internalentry,newrootblock,0)!=SUCCESS)
        			return FAILURE;        
        		struct HeadInfo newrootheader;
                    struct HeadInfo header1=getheader(head);
          	struct HeadInfo header2=getheader(newchild);
		newrootheader.numEntries=1;
		newrootheader.blockType=IND_INTERNAL;
		newrootheader.pblock=-1;
       		header1.pblock=newrootblock;
      		header2.pblock=newrootblock;
       		setheader(&newrootheader,newrootblock);
        		setheader(&header1,head);
       		setheader(&header2,newchild);
        		attrcatentry[4].ival=newrootblock;
		//cout<<"New blk "<<newrootblock<<"\n";
       		if(setAttrCatEntry(relid,attrcatentry[1].sval,attrcatentry)!=SUCCESS)
		{      
        			return FAILURE;
		}
		done=true;
	}
    }
}
return SUCCESS;
}

int bplus_destroy(int block_num)
{
	
	struct HeadInfo header;
	
	if(block_num < 0 || block_num >= 8192)
	{ //if the block_num is invalid
		return E_OUTOFBOUND;
	}	
	header = getheader(block_num);
	int block_type = getBlockType(block_num);
	//cout<<block_num<<" "<<block_type<<"\n";
	if(block_type ==REC)
	{ //if the block is not index block
		return FAILURE;
	}
	
	if(block_type == IND_INTERNAL)
	{
		// if block is internal node remove all children before removing it *IMPORTANT *
		int num_entries;
		num_entries = header.numEntries;
		
		int iter;
		struct InternalEntry internal_entry;
		for(iter = 0;iter < num_entries; iter++)
		{	
			// get the internal index block entries
			internal_entry=getEntry(block_num,iter);
			bplus_destroy(internal_entry.lChild);
		}
		bplus_destroy(internal_entry.rChild);
		deleteBlock(block_num);
	}
	else if(block_type == IND_LEAF)
	{
	    deleteBlock(block_num);
	}
	cout<<"INDEX DELETED\n";
	return SUCCESS;
}

int dropindex(int relid,char attrname[16])
{
	    union Attribute attrcatentry[6];
	    if(getAttrCatEntry(relid,attrname,attrcatentry)!=SUCCESS)
		return FAILURE;
	    int root_block=attrcatentry[4].ival;
	    attrcatentry[4].ival=-1;
	    setAttrCatEntry(relid,attrname,attrcatentry);
	    if(bplus_destroy(root_block)!=SUCCESS)
		return FAILURE;
	    return SUCCESS;
}

int bplus_create(int relid, char AttrName[ATTR_SIZE])
{
	int flag;
	//getting attribute catalog entry 
	union Attribute attrcatentry[6];
	flag=getAttrCatEntry(relid, AttrName, attrcatentry);
	// in case attribute does not exist
	if(flag!=SUCCESS)
		return flag;
	
	//Checking already whether an index exists for the atrribute or not
	if(attrcatentry[4].ival != -1)
	{
	 	return SUCCESS; // already index exists for the attribute
	}

	union Attribute relcatentry[6];
	if(getRelCatEntry(relid,relcatentry)!=SUCCESS)
       		return FAILURE;
	int data_block, num_attrs;
	data_block = relcatentry[3].ival; //first record block for the relation
	num_attrs = relcatentry[1].ival; //num of attributes for the relation

	// getting the free leaf index as root block
	int root_block=getFreeBlock(IND_LEAF);
	if(root_block==FAILURE)	//Problem in getting free leaf index block
		return E_DISKFULL;
	
	struct HeadInfo H;
	H.blockType=IND_LEAF;
	H.pblock=-1;
	H.lblock=0;
	H.rblock=0;
	H.numEntries=0;
	setheader(&H,root_block);

	attrcatentry[4].ival= root_block;
	cout<<root_block<<"\n";
	if(setAttrCatEntry(relid, AttrName, attrcatentry)!=SUCCESS)
		return FAILURE;

	int attroffset, attrtype;
	attroffset = attrcatentry[5].ival;
	attrtype = attrcatentry[2].ival;
	union Attribute record[num_attrs];	
	
	//inserting index entries for each record in bplus tree
	while(data_block != -1)
	{
		struct HeadInfo header; //getting header/preablem of the record block
		header = getheader(data_block);
		
		int num_slots;	//getting slotmap of the record block
		num_slots = header.numSlots;
		unsigned char slotmap[num_slots];
		getSlotmap(slotmap,data_block);
		
		int iter;
		for(iter = 0; iter < num_slots; iter++)
		{
			if(slotmap[iter] == '0')
				continue;
			
			getRecord(record, data_block,iter);	//getting iter th number record from data block
			union Attribute attrval;
			if(attrtype==INT)
			{
				attrval.ival = record[attroffset].ival;	//getting attribute value corresponding to attribute name
				//cout<<attrval.ival<<" ";
			}
			else if(attrtype==FLOAT)
			{
				attrval.fval = record[attroffset].fval;
				//cout<<attrval.fval<<" ";
			}
			else if(attrtype==STRING)
			{
				strcpy(attrval.sval,record[attroffset].sval);
				//cout<<attrval.sval<<" ";
			}
			recId recid;
			recid.block = data_block;
			recid.slot = iter;
			//cout<<relid<<" "<<AttrName<<" "<<data_block<<" "<<iter<<"\n";
			//call bplus_insert
			int res=bplus_insert(relid, AttrName, attrval, recid);
			if(res!=SUCCESS)
			{
				bplus_destroy(root_block);
				return FAILURE;
			}
		}
		data_block = header.rblock; //next data block for the relation
	   } 
	   cout<<"INDEX CREATED\n";
	    return SUCCESS;
}	



    
