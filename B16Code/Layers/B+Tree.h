#include "cache.h"

class BplusTree
{
    private :
    int rootBlock, relId;
    char attrName[ATTR_SIZE];
    int bplusDestroy(int blockNum);
    
    public:
    BplusTree(int relId, char attrName[ATTR_SIZE]);
    int bplusInsert(union Attribute val, recId recordId);
    struct recId bplusSearch(union Attribute attrVal, int op);
    int getRootBlock();
    
    
};

int BplusTree::getRootBlock()
{
    return this->rootBlock;
}

int BplusTree::bplusDestroy(int blockNum){

	if(blockNum < 0 || blockNum >= DISK_BLOCKS){ //if the blockNum is invalid
		return E_OUTOFBOUND;
	}
	
	
	if(StaticBuffer::getStaticBlockType(blockNum) == IND_LEAF){ //if the block is leaf index block
		//delete the block from the disk
		IndLeaf leafBlk(blockNum);
		leafBlk.releaseBlock();
		return SUCCESS;
	}
	else if(StaticBuffer::getStaticBlockType(blockNum) == IND_INTERNAL){ 	//if the block is internal index block

		IndInternal internalBlk(blockNum);		
		HeadInfo internalHead;
		internalBlk.getHeader(&internalHead);
		
		InternalEntry entry;
		for(int i =0; i< internalHead.numEntries;++i) {
			//delete all the child index blocks corresponding to each internal index block by calling
			
			internalBlk.getEntry(&entry, i);
			if(i ==0)
				bplusDestroy(entry.lChild);
			bplusDestroy(entry.rChild);
		}
		cout <<"Deleting internal entry: "<< blockNum<<endl;
		internalBlk.releaseBlock();
		return SUCCESS;
	}	
	else {
		return E_INVALIDBLOCK;
	}
	
}

int BplusTree::bplusInsert(union Attribute val,recId recordId)
{
	cout <<"Inside bplus tree insert\n";
	
    //get the AttrCatEntry of target attribute using getAttrCatEntry() of cache layer.
    AttrCatEntry attrcatEntry;
	
	//AppuChanges: The return value of B+ create should also have the values E_OUTOFBOUND, E_ATTRNOTEXIST, E_NOTOPEN
	int status = AttrCacheTable::getAttrCatEntry(relId, attrName, &attrcatEntry);
	
	if( status != SUCCESS)
		return status;
	
    //let blockNum be the rootBlock of AttrCatEntry 
    int blockNum = attrcatEntry. rootBlock;
    
    int parentBlock = blockNum; //TODO : this is only for the purpose of deleting the entire B+ tree when the need arise

    if(blockNum==-1) 
    	return E_NOINDEX;
    	

    int attrType = attrcatEntry.attrType;
    
    //cout << "Attribute type is: "<< attrType<<endl;
    
    class IndBuffer *block=NULL;
	HeadInfo blockHeader;
	
    // while block is not LEAF:
    while(StaticBuffer::getStaticBlockType(blockNum)!= IND_LEAF) {

        cout <<"It is an internal block\n";
        block = new IndInternal(blockNum);
            	
        block->getHeader(&blockHeader);
          
        int nEntries = blockHeader.numEntries;
        
        int i;

        InternalEntry indEntry;
        
        for(i =0; i< nEntries; ++i) {
			
				block->getEntry(&indEntry, i);
				
				int flag = compare(indEntry.attrVal, val, attrType);
				
				if(flag >= 0)
					break;
			
        }
        
        //if no such offset is found:
        if(i == nEntries){

              blockNum = indEntry.rChild;
              delete(block);
         }
         else {
         	
         	  blockNum = indEntry.lChild;
              delete(block);
         }
    }

    //now block is the leaf block to which we need to insert Index into.
    cout <<"It is now a leaf block\n";
    block = new IndLeaf(blockNum);
    
	Index indexval;
	indexval.block = recordId.block;
	indexval.slot =  recordId.slot;
	indexval.attrVal = val;
	
	cout<<"Index to be inserted is val: ";
	if(attrType == STRING)
		cout<<val.strval;
	else if(attrType == INT)
		cout<<val.ival;
	else
		cout<<val.fval;
	cout <<" recid: ("<<indexval.block<<", "<<indexval.slot<<")\n";
	

    block->getHeader(&blockHeader);
    int nEntries = blockHeader.numEntries;

    //let indices[nEntries+1] be an array of type struct Index.
	Index indices[nEntries+1], temp;
	
	bool inserted = false;
    //iterate through 0 to nEntries-1:
    for(int i =0,j=0;i< nEntries;++i,++j){

        block->getEntry(&temp,i);
        
        int flag = compare(temp.attrVal, val, attrType);
        if(!inserted && flag >= 0){
        	indices[j++] = indexval;
        	inserted = true;
        }
        indices[j] = temp;
	}
    
	if(!inserted){
		indices[nEntries] = indexval;
	}
    
    //if (nEntries!= 63)// ( max. no of entries in leaf)
    if (nEntries!= 3)// ( max. no of entries in leaf)
    {
        
        blockHeader.numEntries++;
        block->setHeader(&blockHeader);
        
        //iterate through 0 to nEntries-1:
        cout <<"No of entries is :"<<nEntries<<" so the leaf: "<< block->getBlockNum()<<" is NOT full\n";
        for(int i =0;i<=nEntries;++i){
           
           block->setEntry(&indices[i],i);
		}
        
        //Delete block. (calls destructor)
        delete(block);
        
        cout<<endl;
        return SUCCESS;
    }
    else {
    	cout <<"No of entries is :"<<nEntries<<" so the leaf is full\n";
    	
        class IndBuffer * newRightBlk= (class IndLeaf *) new IndLeaf();
        class IndBuffer * leftBlk = (class IndLeaf *)block;
        		
		int newRightBlkNum = newRightBlk->getBlockNum();
		int leftBlkNum=blockNum;
		
		if(newRightBlkNum==FAILURE){
			cout<<"Return point 1\n";
			cout <<"\nDisk Full!!!\n";
			cout <<"Deleting tree...\n\n";
			bplusDestroy(parentBlock);
			AttrCacheTable::getAttrCatEntry(relId, attrName, &attrcatEntry);
	        attrcatEntry. rootBlock = -1;
	        AttrCacheTable::setAttrCatEntry(relId, attrName, &attrcatEntry);
	        this->rootBlock = E_DISKFULL;
			return E_DISKFULL;
		}
        //let newRightBlkHeader be of type struct HeadInfo
        struct HeadInfo newRightBlkHeader;
        struct HeadInfo leftBlkHeader = blockHeader;
			
		//AppuChanges: first set the leftBlkHeader
		int prevRblock = leftBlkHeader.rblock;
		
		//leftBlkHeader.numEntries= 32;
		leftBlkHeader.numEntries= 2;
		
		leftBlkHeader.rblock = newRightBlk->getBlockNum();
		
		leftBlk->setHeader(&leftBlkHeader);
		
		newRightBlk->getHeader(&newRightBlkHeader);
		newRightBlkHeader.pblock = leftBlkHeader.pblock;
		newRightBlkHeader.lblock = leftBlk->getBlockNum();
		newRightBlkHeader.rblock = prevRblock;
		
		//newRightBlkHeader.numEntries = 32;
		newRightBlkHeader.numEntries = 2;

		//newRightBlkHeader.numSlots = 63;
		newRightBlkHeader.numSlots = 3;

		newRightBlk->setHeader(&newRightBlkHeader);
		

        //for(int i =0;i<32;++i){
        for(int i =0;i<2;++i){
            
            leftBlk->setEntry(&indices[i],i);
            
            //newRightBlk->setEntry(&indices[i+32],i);
            newRightBlk->setEntry(&indices[i+2],i);
            
        }
        
        int parBlkNum = leftBlkHeader.pblock;
        
        union Attribute newAttrval = indices[1].attrVal;
		
		
        delete leftBlk;
        delete newRightBlk; // delete calls destructor.
		
		cout <<"Splitting into 2 with left block: "<<leftBlkNum<<", right block: "<<newRightBlkNum<<" and parent block: "<<parBlkNum<<" and attribute to be inserted is: ";
		
		if(attrType == STRING)
			cout << newAttrval.strval;
		else if(attrType == INT)
			cout << newAttrval.ival;
		else
			cout << newAttrval.fval;
		cout <<endl;
        
        // let done be a bool with value false.
        bool done = false;
        while(!done){
            if (parBlkNum!=-1) {
            
                class IndInternal *parblk= new IndInternal(parBlkNum);
            
                struct HeadInfo parheader;
                parblk->getHeader(&parheader);

                struct InternalEntry internalEntries[parheader.numEntries+1], temp;
                
                bool justInserted = false;
				
				for(int i =0,j=0;i< parheader.numEntries;++i,++j){
					
					parblk->getEntry(&internalEntries[j],i);
					
					if((i==0) && (internalEntries[j].lChild == leftBlkNum)){
						cout <<"Left child!\n";
						temp = internalEntries[j];
						internalEntries[j].lChild = leftBlkNum;
    		            internalEntries[j].attrVal= newAttrval; 
		                internalEntries[j].rChild = newRightBlkNum;
		                justInserted = true;
		                internalEntries[++j] = temp;
		                
					}
					
					
					if(justInserted) {
						internalEntries[j].lChild = newRightBlkNum;
						justInserted=false;
					}
					
					if(internalEntries[j].rChild == leftBlkNum){

						cout <<"Right child!\n";
		                j++;
						internalEntries[j].lChild = leftBlkNum;
    		            internalEntries[j].attrVal= newAttrval; 
		                internalEntries[j].rChild = newRightBlkNum;
		                justInserted = true;
					}
		
					
				}
		
				
                //if(parheader.numEntries!=100){ //parBlkNum not full
                if(parheader.numEntries!=4){ //max entry is 4
                	
                	cout <<"No of entries is :"<<parheader.numEntries<<" so the internal block is NOT full\n";
                    
                    parheader.numEntries++;
                    parblk->setHeader(&parheader);
                    
                    for(int i =0;i<parheader.numEntries;++i)
                    	parblk->setEntry(&internalEntries[i],i);
                    
                    delete parblk;
                    done=true;
                }
                else {
                	cout <<"No of entries is :"<<leftBlkHeader.numEntries<<" so the internal block is full\n";
                    
                    class IndInternal* dummy = new IndInternal();
                    
                    if(dummy->getBlockNum()==FAILURE){
                    	cout<<"Return point 2\n";
                    	cout <<"\nDisk Full!!!\n";
						cout <<"Deleting detached right subtree...\n\n";
                    	bplusDestroy(newRightBlkNum);
                    	
                    	cout <<"\nDeleting tree...\n\n";
				        bplusDestroy(parentBlock);
				        AttrCacheTable::getAttrCatEntry(relId, attrName, &attrcatEntry);
				        attrcatEntry. rootBlock = -1;
				        AttrCacheTable::setAttrCatEntry(relId, attrName, &attrcatEntry);
				        this->rootBlock = E_DISKFULL;
						return E_DISKFULL;
                    }
                    
                    newRightBlk= (class IndInternal* )dummy;
                    leftBlk= (class IndInternal* )parblk;
                    
                    newRightBlkNum = newRightBlk->getBlockNum();
                    leftBlkNum = parBlkNum;
                    
                    newRightBlk->getHeader(&newRightBlkHeader);
                    leftBlkHeader = parheader;
                    
                    
                    //leftBlkHeader.numEntries = 50;
                    leftBlkHeader.numEntries = 2;
                    leftBlk->setHeader(&leftBlkHeader);
                    
                    newRightBlkHeader.pblock = leftBlkHeader.pblock;
                    
                    //newRightBlkHeader.numEntries = 50;
                    newRightBlkHeader.numEntries = 2;
                    
                    //newRightBlkHeader.numSlots = 100;
                    newRightBlkHeader.numSlots = 4;
                    
                    newRightBlk->setHeader(&newRightBlkHeader);
                    
                    //iterating through 0 to 50:
                    //for(int i=0;i<50;++i){
                    for(int i=0;i<2;++i){
                        
                        leftBlk->setEntry(&internalEntries[i],i);
                    
                    	//newRightBlk->setEntry(&internalEntries[i + 51],i);
                    	newRightBlk->setEntry(&internalEntries[i + 3],i);
                    }
                    
                    //iterating through 2nd 50 entries of internalEntries
                    IndBuffer *childBuff;
                    struct HeadInfo childBuffHead;
                    
                    //AppuDoubt: Is splitting into 2 cases necessary??
                    
                    //int type = StaticBuffer::getStaticBlockType(internalEntries[50].rChild);
	                int type = StaticBuffer::getStaticBlockType(internalEntries[2].rChild);
                	
                    //for(int i =50;i<=100;++i){
                    for(int i =2;i<5;++i){
                        
		                if(type == IND_LEAF)
			                childBuff = (IndBuffer*) new IndLeaf(internalEntries[i].rChild);
			            else
			            	childBuff = (IndBuffer*) new IndInternal(internalEntries[i].rChild);
                        childBuff->getHeader(&childBuffHead);
		            	childBuffHead.pblock = newRightBlkNum;
		            	childBuff->setHeader(&childBuffHead);	
  						delete childBuff;                    
                    }
                    
                    delete leftBlk;
                    delete newRightBlk;

     				parBlkNum=leftBlkHeader.pblock;
                    
                    //newAttrval=internalEntries[50].attrVal;
                    newAttrval=internalEntries[2].attrVal;
                    //newRightBlkNum already set
                    
                    cout <<"Splitting into 2 with left block: "<<leftBlkNum<<", right block: "<<newRightBlkNum<<" and parent block: "<<parBlkNum<<" and attribute to be inserted is: ";
		
					if(attrType == STRING)
						cout << newAttrval.strval;
					else if(attrType == INT)
						cout << newAttrval.ival;
					else
						cout << newAttrval.fval;
					cout <<endl;
            	}
            }
            else { //if parBlkNum==-1

                class IndInternal* newRootBlk=new IndInternal(); //if fails..delete indexing
                                
                int newRootBlkNum=newRootBlk->getBlockNum();
                if(newRootBlkNum == FAILURE) {
	                cout<<"Return point 3\n";
            		cout <<"\nDisk Full!!!\n";
					cout <<"Deleting detached right subtree...\n\n";
					
            		bplusDestroy(newRightBlkNum);
                	
                	cout <<"\nDeleting tree...\n\n";
                	bplusDestroy(parentBlock);
			        AttrCacheTable::getAttrCatEntry(relId, attrName, &attrcatEntry);
			        attrcatEntry. rootBlock = -1;
			        AttrCacheTable::setAttrCatEntry(relId, attrName, &attrcatEntry);
			        this->rootBlock = E_DISKFULL;
            		return E_DISKFULL;
	           	}
                struct HeadInfo newRootBlkHeader;
                newRootBlk->getHeader(&newRootBlkHeader);

            	cout <<"Yay! New root block with block: "<<newRootBlkNum<<endl;
                
                InternalEntry entry;
                entry.lChild = leftBlkNum;
                entry.attrVal = newAttrval;
                entry.rChild = newRightBlkNum;
				
				newRootBlkHeader.numEntries = 1;

				//newRootBlkHeader.numSlots = 100;
				newRootBlkHeader.numSlots = 4;
				
				newRootBlk->setHeader(&newRootBlkHeader);
				newRootBlk->setEntry(&entry,0);
				

				//similarly update headers of leftBlkNum, newRightBlkNum with pblock=newRootBlkNum
				if(StaticBuffer::getStaticBlockType(leftBlkNum) ==IND_LEAF){
					leftBlk= new IndLeaf(leftBlkNum);
					newRightBlk = new IndLeaf(newRightBlkNum);
					cout<<"Doubt2: Leaf needed\n";
				}
				else {
					leftBlk= new IndInternal(leftBlkNum);
					newRightBlk = new IndInternal(newRightBlkNum);
					cout<<"Doubt2: Internal entry needed\n";
				}
				
				leftBlk->getHeader(&leftBlkHeader);
				leftBlkHeader.pblock = newRootBlkNum;
				leftBlk->setHeader(&leftBlkHeader);
				
				newRightBlk->getHeader(&newRightBlkHeader);
				newRightBlkHeader.pblock = newRootBlkNum;
				newRightBlk->setHeader(&newRightBlkHeader);
				
				delete leftBlk;
				delete newRightBlk;
				
				attrcatEntry. rootBlock= newRootBlkNum;
				AttrCacheTable::setAttrCatEntry(relId, attrName, &attrcatEntry);
				this->rootBlock =newRootBlkNum;
                delete newRootBlk;
                done=true;
        	}
    	}
    }
    cout<<endl;
   return SUCCESS;
}

BplusTree::BplusTree(int relId, char attrName[ATTR_SIZE])
{
	cout <<"Inside bplus create\n";
	
	this->relId = relId;
	strcpy(this->attrName, attrName);
	
	AttrCatEntry attrcatBuff;
	
	//AppuChanges: The return value of B+ create should also have the values E_OUTOFBOUND, E_ATTRNOTEXIST, E_NOTOPEN
	int status = AttrCacheTable::getAttrCatEntry(relId, attrName, &attrcatBuff);
	
	if( status != SUCCESS){
		cout << "getAttrCatEntry did not work with status: "<< status<<endl;
		this->rootBlock = status;
		//return status;
	}
	int rootBlockNum = attrcatBuff. rootBlock;
	
	if(rootBlockNum != -1)
	{
		//Index already exists for the attribute
		this->rootBlock = rootBlockNum;
		return;
	}
	
	//get a free leaf block using getFreeBlock() of BufferLayer
	IndLeaf rootBlock;
	//AppuChanges: header is initialised to default values in getFreeBlock()
	
	if(rootBlock.getBlockNum()==FAILURE)
	{ //disk is full (i.e unable to get new record block from the disk)
		cout <<"\nDisk Full!!!\n";
		this->rootBlock = E_DISKFULL;
		return ;
	}
	else 
		this->rootBlock = rootBlock.getBlockNum();
		
	//update the root block in the attribute cache correponding to the
	attrcatBuff. rootBlock = rootBlock.getBlockNum();
	AttrCacheTable::setAttrCatEntry(relId, attrName, &attrcatBuff);
	//AppuNote: the return value will always be SUCCESS here
	
	cout << "The new root block alloted is: "<< rootBlock.getBlockNum()<<endl;
	
	//Iterate over all the records of the relation with Id relId
	RelCatEntry relcatBuff;
	RelCacheTable::getRelCatEntry(relId, &relcatBuff);
	//AppuNote: the return value will always be SUCCESS here
	
	int block = relcatBuff. firstBlk;
	//, slot = 0;

	int numAttr = relcatBuff.numAttrs;
	int numSlots = relcatBuff.numSlotsPerBlk;

	int attrOffset = attrcatBuff.offset;	

	
	while(block!=-1) {
	
		RecBuffer recBlk(block);
	
		unsigned char slotMap[numSlots];
		recBlk.getSlotMap(slotMap);
		
		for(int slot =0; slot < numSlots; ++slot) {
			if(slotMap[slot]=='1') {
		
				//insert the index entries for each record in the bplus tree
				struct recId recordId;
				recordId.block = block;
				recordId.slot = slot;
			
				union Attribute rec[numAttr];
				recBlk.getRecord(rec, slot);
			
				cout <<endl;
				cout <<"Inserting record with block: "<< block<<" slot: "<<slot<<" with attribute val: ";
				cout <<endl;
				if(attrcatBuff.attrType == STRING)
					cout<< rec[attrOffset].strval;
				else if(attrcatBuff.attrType == INT)
					cout<< rec[attrOffset].ival;
				else
					cout<< rec[attrOffset].fval;
				cout <<endl;
				int flag = bplusInsert(rec[attrOffset], recordId);
			
				//where the recid is record id corresponding to the record
				if(flag == E_DISKFULL)
				{
					this->rootBlock = E_DISKFULL;
					return ;
				}
			}
			else //all records has been traversed
				break;
		}	
		
		HeadInfo blkHead;
		recBlk.getHeader(&blkHead);
		
		block = blkHead.rblock;
		
	}
}

struct recId BplusTree::bplusSearch(union Attribute attrVal, int op){

	cout<<"Inside B+ Search\n";
	
	AttrCatEntry attrcatEntry;
	AttrCacheTable::getAttrCatEntry(relId, attrName, &attrcatEntry);
	
	IndexId prevIndexid, indexId = {-1, -1};
	AttrCacheTable::getSearchIndex(relId, attrName, &prevIndexid);
	AttrCacheTable::setSearchIndex(relId, attrName, &indexId);
	
	int block, index;
	
	recId returnId={-1, -1};	
	
	if(prevIndexid.block ==-1 && prevIndexid.block ==-1){

		block = attrcatEntry.rootBlock;
		if(block == -1) //B+ Tree doesn't exist for the attribute
			return returnId;
		index =0;
	}
	else{
		
		block = prevIndexid.block;
		index = prevIndexid.index+1;
		
		IndLeaf leaf(block);
			
		HeadInfo leafHead;
		leaf.getHeader(&leafHead);
		
		if(index == leafHead.numEntries) {
			
			block = leafHead.rblock;
			index = 0;
			if(block == -1) 
				 return returnId;
		} 
	}
	
	InternalEntry intEntry;
	IndInternal *internalBlk=NULL;
	HeadInfo intHead;
	int cond;
	
	//cout<<"HERE\n";
	
	while(StaticBuffer::getStaticBlockType(block)== IND_INTERNAL)	//index will be 0 in the begining
	{

		internalBlk= new IndInternal(block);
		internalBlk->getHeader(&intHead);
		cond = 0; //unset

		for(int i = 0; i< intHead.numEntries; ++i) {
			internalBlk->getEntry(&intEntry, i);
	
			int flag = compare(intEntry.attrVal, attrVal, attrcatEntry.attrType);
			switch(op){
				case  EQ:
					if(flag >=0)	//searches for 1st entry that is greater than or equal to attrVal and moves to left
						cond = 1;
					break;
				case LE:
					cond = 1;
					break;
				case LT:
					cond = 1;
					break;
				case GE:
					if(flag >=0)
						cond = 1;
					break;
				case GT:
					if(flag >0)
						cond = 1;
					break;
				case NE:
					cond = 1;
					break;
			}
			if(cond == 1){
				block = intEntry.lChild;
				delete internalBlk;
				break;
			}
		}
		
		
		if(cond == 0) {
			block = intEntry.rChild;
			delete internalBlk;
		}
	}
	
	//cout<<"HERE2\n";
	//Traverse through leaf index entries in the leaf index block (block) from index entry numbered as index
	Index leafEntry ;
	IndLeaf * leafBlk= NULL;
	cond = 0; //not found but need to search more; cond=1: got a right answer; cond =-1 : stop searching

	while(block!=-1){
		
		leafBlk = new IndLeaf(block);
	
		HeadInfo leafHead;
		leafBlk->getHeader(&leafHead);
		while(index<leafHead.numEntries) {

			leafBlk->getEntry(&leafEntry, index);
			int flag = compare(leafEntry.attrVal, attrVal, attrcatEntry.attrType);

			switch(op){
				case  EQ:
					if(flag ==0)
						cond = 1;
					else if(flag > 0)
						cond = -1;
					break;
				case LE:
					if(flag <=0)
						cond = 1;
					else
						cond = -1;
					break;
				case LT:
					if(flag <0)
						cond = 1;
					else
						cond = -1;
					break;
				case GE:
					if(flag >=0)
						cond = 1;
					break;
				case GT:
					if(flag >0)
						cond = 1;
					break;
				case NE:
					if(flag !=0)
						cond = 1;
					break;
			}
			if(cond == 1){
				returnId.block = leafEntry.block;
				returnId.slot = leafEntry.slot;
				
				indexId.block = block;
				indexId.index = index;
				
				AttrCacheTable::setSearchIndex(relId, attrName, &indexId);
				
				cout <<"The value satisfying the condition is: ";
				if(attrcatEntry.attrType==STRING)
					cout << leafEntry.attrVal.strval;
				else if(attrcatEntry.attrType==INT)
					cout << leafEntry.attrVal.ival;
				else
					cout << leafEntry.attrVal.fval;
				cout<<endl;
				delete leafBlk;
				return returnId;
			}
			else if(cond == -1){
				delete leafBlk;
				return returnId;
			}
			++index;
		}
		if(op!= NE)	//only for NE do we have to check for the entire chain 
			break;	//for all the other op it is guaranteed that the block being searched will have an entry satisying that op
		block = leafHead.rblock;
		delete leafBlk;
		index = 0;
	}
	//cout<<"HERE 3\n";
	if(op!=NE)
	 delete leafBlk;
	return returnId;
}


void bplus_display_helper(int rootBlockNum, int attrType){
	//cout <<"Attribute type : "<<attrType<<endl;
	if(StaticBuffer::getStaticBlockType(rootBlockNum)==IND_LEAF){
		cout <<"---------------------------\n";
		cout <<"*****Leaf Block "<<rootBlockNum<<"*****"<<endl;
    	IndLeaf leaf(rootBlockNum);
    	HeadInfo head;
    	leaf.getHeader(&head);
    	int nEntries=head.numEntries;
    	Index entry;
    	for(int i =0;i<nEntries; ++i) {
    		leaf.getEntry(&entry,i);
    		if(attrType == STRING)
    			cout <<entry.attrVal.strval;
    		else if(attrType == INT)
    			cout <<entry.attrVal.ival;
    		else
    			cout <<entry.attrVal.fval;
    		cout <<" ("<<entry.block<<", "<<entry.slot<<")\n";
    	}
    	cout <<"###########################\n\n";
    }
    else {
    	cout <<"---------------------------\n";
    	cout<<"*****Internal Block: "<<rootBlockNum<<"*****"<<endl;
    	
    	IndInternal intblk(rootBlockNum);
    	HeadInfo head;
    	intblk.getHeader(&head);
    	int nEntries=head.numEntries;
    	
    	InternalEntry entry;
    	for(int i =0;i<nEntries; ++i) {
    		intblk.getEntry(&entry, i);
    		if(i==0){
    			bplus_display_helper(entry.lChild, attrType);
    		}
    		cout << "Internal entry val: ";
    		if(attrType == STRING)
    			cout <<entry.attrVal.strval;
    		else if(attrType == INT)
    			cout <<entry.attrVal.ival;
    		else
    			cout <<entry.attrVal.fval;
    		cout <<" in block: "<<rootBlockNum;
    		cout<<endl;
    		bplus_display_helper(entry.rChild, attrType);
    	
    	}
    	cout <<"###########################\n\n";
    }
}

void bplus_display(int relId, char attrName[ATTR_SIZE]){
	AttrCatEntry attrcatEntry;
	
	//AppuChanges: The return value of B+ create should also have the values E_OUTOFBOUND, E_ATTRNOTEXIST, E_NOTOPEN
	int status = AttrCacheTable::getAttrCatEntry(relId, attrName, &attrcatEntry);
	
	if( status != SUCCESS)
		cout<<"Invalid status\n";
	
    //let rootBlockNum be the rootBlock of AttrCatEntry 
    int rootBlockNum = attrcatEntry. rootBlock;
    
    //if rootBlockNum of target attribute is -1, return E_NOINDEX
    if(rootBlockNum==-1) 
    	{cout<<"B+ Tree does not exist\n";
    	return;}
    int attrType = attrcatEntry.attrType;
    
    bplus_display_helper(rootBlockNum, attrType);	
    
}

