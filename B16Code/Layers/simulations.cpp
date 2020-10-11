#include<bits/stdc++.h>
//#include "cache.h"
#include "B+Tree.h"
#include "../define/constants.h"

using namespace std;

int getFormatStringDummy(char* relationName, char *format)
{
	//cout << "Inside getFormatStringDummy\n";
    if(strcmp(relationName,"relationCat")==0)
        strcpy(format,"siiiii");
    else if(strcmp(relationName,"attributeCat")==0)
        strcpy(format,"ssiiii");
    else{
    
    	//is under the assumption that the relation has an entry in the catalog cache
    	int relid = OpenRelTable::getRelId(relationName),len =0;
    	
    	RelCatEntry relcat_entry;
		RelCacheTable::getRelCatEntry(relid, &relcat_entry);
		
		int numAttrs = relcat_entry. numAttrs;
    	
    	AttrCatEntry attrcat_entry;
    	
    	
    	for(int i =0; i<numAttrs;++i){
    		AttrCacheTable::getAttrCatEntry(relid, i, &attrcat_entry);
    		if(attrcat_entry.attrType == STRING)
    			format[len++]='s';
    		else if (attrcat_entry.attrType == INT)
    			format[len++]='i';
    		else
    			format[len++]='f';
    	}
    
        format[len]='\0';
	    //cout <<"out\n";
	    }
	return 1;
}

void initCatalogs(int blockNum, ...) { 
//returns the blockNum in which the record is stored
    
    //testing RecBuffer 
    class RecBuffer recRec(blockNum);
    int numAttrs = 6;
    char format[6];
    if(blockNum == 4) //relation catalog
    	strcpy(format, "siiiii");
    else	//attribute catalog
    	strcpy(format, "ssiiii");
    //setting up header
    struct HeadInfo recHead;
    
    recHead. blockType = REC;
	recHead. pblock=-1;
	recHead. lblock=-1;//need to be changed
	recHead. rblock=-1;//need to be changed
	recHead. numEntries=1;//entry added below
	recHead. numAttrs=numAttrs; 
	recHead. numSlots=floor(2016/(16* numAttrs + 1));
	
	recRec.setHeader(&recHead);

    union Attribute recAttrs[numAttrs];
     
    va_list args;
    va_start(args, blockNum);
    for(int i =0;i<6;++i){
	    if(format[i]=='f')
		    recAttrs[i].fval = va_arg(args, double);
	    else if(format[i]=='i')
		    recAttrs[i].ival = va_arg(args, int);
	    else
		    strcpy(recAttrs[i].strval, va_arg(args, char*));
    }

    va_end(args);
    
    unsigned char slotMap[recHead. numSlots];
    for(int i=1; i<recHead. numSlots; i++)
        slotMap[i]='0';
        
    slotMap[0]='1';
    recRec.setSlotMap(slotMap);   
    recRec.setRecord(recAttrs,0);
    
}


int getRecordSimDummy(char *relationName)
{
 	
 	int relid = OpenRelTable::OpenRel(relationName);
    //int blockNum = rel_table[relid].relcat_entry.firstBlk;
    //cout<<"HERE\n";
    RelCatEntry relcat_entry;
    //OpenRelTable::printCache();
	RelCacheTable::getRelCatEntry(relid, &relcat_entry);
	int blockNum = relcat_entry.firstBlk;
	//cout<<"HERE "<<blockNum<<endl;
    unsigned char slotMap[120];
    struct HeadInfo headInfo;
    
    do{
   //StaticBuffer staticBuffer;
        class RecBuffer recBuffer(blockNum);
        
        recBuffer.getSlotMap(slotMap);
        
	    recBuffer.getHeader(&headInfo);
	    
        cout <<"\nHead attributes\n";
	    cout<<headInfo. blockType<<endl;
	    cout<<headInfo. pblock<<endl;
	    cout<<headInfo. lblock<<endl;//need to be changed
	    cout<<headInfo. rblock<<endl;//need to be changed
	    cout<<headInfo. numEntries<<endl;
	    cout<<headInfo. numAttrs<<endl; 
	    cout<<headInfo. numSlots<<endl;
	    //cout<<headInfo-> reserved<<endl;
        
        //cout<<endl<<"Inside get record sim"<<endl;
        
	    char format[125];
	    getFormatStringDummy(relationName, format);
	    
        cout <<"Table Contents\n";
		
		int numAttrs = headInfo.numAttrs;
    	
    	AttrCatEntry attrcat_entry;
    	
        //AttributeCacheEntry * head = rel_table[relid].attr_list_head;
        
        cout <<"|";
        for(int j=0; j<headInfo.numAttrs ; j++){
        	AttrCacheTable::getAttrCatEntry(relid, j, &attrcat_entry);
        	cout<<setw(15)<<attrcat_entry.attrName<<"|";
        	//head = head->next;
        }
        
        cout <<endl<<"+";
        for(int j=0; j<headInfo.numAttrs ; j++){
        	cout<<"---------------+";
        }

        for(int i=0; i<headInfo. numSlots; i++)
            if(slotMap[i]=='1')
            {
                //cout<<"Record "<<i<<":"<<endl;
                union Attribute rec[headInfo.numAttrs];
                recBuffer.getRecord((Attribute*)&rec,i);
                cout <<"\n|";
                for(int j=0; j<headInfo.numAttrs ; j++)
                    {if(format[j]=='s')
                        cout<<setw(15)<<rec[j].strval;
                     else if(format[j]=='i')
                        cout<<setw(15)<<rec[j].ival;
                     else if(format[j]=='f')
                        cout<<setw(15)<<rec[j].fval;
                    
                    
                   cout<<"|";
                   }                
            }
			else
				break;
		    blockNum = headInfo.rblock;
            
    }while(blockNum!=-1);
    //CloseRel(relid);
    return 0;
}

void addRecordToBPlusTree(int relid, union Attribute *record, recId recid){
	RelCatEntry relcat_entry;
	RelCacheTable::getRelCatEntry(relid, &relcat_entry);
	
	int numAttrs = relcat_entry.numAttrs;
	
	AttrCatEntry attrcat_entry;
	for(int i =0;i<numAttrs;++i){
		AttrCacheTable::getAttrCatEntry(relid, i, &attrcat_entry);
		
		if(attrcat_entry.rootBlock!=-1){
			BplusTree bplusTree(relid,attrcat_entry.attrName);
			int flag = bplusTree.bplusInsert(record[i], recid);
				
		}
	}
	
}
int addRecordSimDummy(char * relName)
{   

	//for regular relation table
	int relid = OpenRelTable::OpenRel(relName);
	//cout <<"\n Outside openRel function\n";
	//OpenRelTable::printCache();
	RelCatEntry relcat_entry;
	RelCacheTable::getRelCatEntry(relid, &relcat_entry);
	
    int blockNum = relcat_entry.lastBlk;
    int numAttrs = relcat_entry.numAttrs;
    int numSlots = relcat_entry.numSlotsPerBlk;
    
    char format[125];
    
	getFormatStringDummy((char *)relName, format);
	//cout<<"Format string: "<<format<<endl<<endl;
	
	union Attribute recAttrs[numAttrs];
	
	cout<<"Enter the record :\n";
    
    for(int j=0;j<numAttrs; j++)
    {
        cout<<"Enter attribute "<<j+1<<" : ";
        if(format[j]=='s')
            cin>>recAttrs[j].strval;
        else if(format[j]=='i')
            cin>>recAttrs[j].ival;
        else
            cin>>recAttrs[j].fval;
    }
    
   /*cout<<"Num attributes is: "<<numAttrs<<endl;
    cout<<"Input values: "<<endl;
    */
    int nxtFreeSlot;
    class RecBuffer *recBuffer;
    struct HeadInfo  headInfo;
    unsigned char slotMap[numSlots];
    
    if(blockNum == -1) {
    	//first entry in the table
    	recBuffer = new RecBuffer();
    	nxtFreeSlot = 0;
    }
    else {
    
    	recBuffer = new RecBuffer(blockNum);
    	
    	recBuffer->getHeader(&headInfo);
    	recBuffer->getSlotMap(slotMap);
    	
    	nxtFreeSlot = headInfo.numEntries;
	}
    
    /*cout << "next free slot is: "<< nxtFreeSlot<<endl;
	cout << "num Slots: "<<numSlots<<endl;*/
	if(nxtFreeSlot == numSlots) {
		//block is full

		class RecBuffer *oldRecBuffer = recBuffer;
		
		recBuffer = new RecBuffer(); //new record block alloted
		
		headInfo.rblock = recBuffer->getBlockNum();
		oldRecBuffer->setHeader(&headInfo);
		delete(oldRecBuffer);
		//no header to access
		//recBuffer->getHeader(&headInfo);
		nxtFreeSlot = 0;
		
	}
	
	RelCatEntry newRelCatEntry = relcat_entry;
	
	if(nxtFreeSlot ==0){
		//setting up the head info of new block
		headInfo.blockType = REC;
		headInfo.pblock = -1;
		headInfo.lblock = blockNum;
		headInfo.rblock = -1;
		headInfo.numEntries=1;
		headInfo.numAttrs=numAttrs;
		headInfo.numSlots=numSlots;
		//setting up slotmap
		for(int i =0;i < numSlots;++i)
			slotMap[i]='0';
			
		//update relation catalog cache
		
		if(blockNum ==-1){//first block
			newRelCatEntry.firstBlk = recBuffer->getBlockNum();
			newRelCatEntry.numRecs = 1;	
		}
		else
			newRelCatEntry.numRecs++;	
		newRelCatEntry.lastBlk = recBuffer->getBlockNum();
		
		
	}
	else {
		headInfo.numEntries++;
		newRelCatEntry.numRecs ++;
	}
	
	RelCacheTable::setRelCatEntry(relid, &newRelCatEntry);
	
	recBuffer->setHeader(&headInfo);

    slotMap[nxtFreeSlot] = '1';
    
    recBuffer->setSlotMap(slotMap);
    	
	cout << "\nThe entry is made in block: "<<recBuffer->getBlockNum()<<", slot: "<<nxtFreeSlot<<endl;
	recBuffer->setRecord( (Attribute*)recAttrs, nxtFreeSlot);
	
	recId recid;
	recid.block = recBuffer->getBlockNum();
	recid.slot = nxtFreeSlot;
	
	addRecordToBPlusTree(relid,(Attribute*)recAttrs, recid);
		
	delete recBuffer;
    //CloseRel(relid);
    return 0;
}

int addRecordSimDummy(char *relationName, char *attributeName, int attributeType, int primaryFlag, int offset)
{   
	//ATTRIBUTE CATALOG
	cout <<"Entry made in attribute catalog\n";
	int relid = ATTRCAT_RELID;
	//cout<<ATTRCAT_RELID<<endl;
	
	RelCatEntry relcat_entry;
	RelCacheTable::getRelCatEntry(relid, &relcat_entry);
	//cout <<"here\n";
	
    int blockNum = relcat_entry.lastBlk;
    //cout<<blockNum<<endl;
    int numAttrs = relcat_entry.numAttrs;
    int numSlots = relcat_entry.numSlotsPerBlk;
    
	
	union Attribute recAttrs[numAttrs];
	
	strcpy(recAttrs[0].strval, relationName);
	strcpy(recAttrs[1].strval, attributeName);
	recAttrs[2].ival = attributeType;
	recAttrs[3].ival = primaryFlag;
	recAttrs[4].ival = -1;//root block is initialised to -1
	recAttrs[5].ival = offset;
    
    int nxtFreeSlot;
    class RecBuffer *recBuffer;
    struct HeadInfo  headInfo;
    unsigned char slotMap[numSlots];
    
//cout<<"HEEREEE 1\n";    
	recBuffer = new RecBuffer(blockNum);

//cout<<"HEEREEE 2\n";	
	recBuffer->getHeader(&headInfo);
	recBuffer->getSlotMap(slotMap);

//cout<<"HEEREEE 2\n";	
	nxtFreeSlot = headInfo.numEntries;
    
    /*cout << "next free slot is: "<< nxtFreeSlot<<endl;
	cout << "num Slots: "<<numSlots<<endl;*/

//cout<<"HEEREEE 2\n";	
	if(nxtFreeSlot == numSlots) {
		//block is full

		class RecBuffer *oldRecBuffer = recBuffer;
		
		recBuffer = new RecBuffer(); //new record block alloted
		
		headInfo.rblock = recBuffer->getBlockNum();
		oldRecBuffer->setHeader(&headInfo);
		delete(oldRecBuffer);
		//no header to access
		//recBuffer->getHeader(&headInfo);
		nxtFreeSlot = 0;
		
	}
	
//cout<<"HEEREEE 2\n";
	RelCatEntry newRelCatEntry = relcat_entry;
	
//cout<<"HEEREEE 2\n";
	if(nxtFreeSlot ==0){
		//setting up the head info of new block
		headInfo.blockType = REC;
		headInfo.pblock = -1;
		headInfo.lblock = blockNum;
		headInfo.rblock = -1;
		headInfo.numEntries=1;
		headInfo.numAttrs=numAttrs;
		headInfo.numSlots=numSlots;
		
		//setting up slotmap
		for(int i =0;i < numSlots;++i)
			slotMap[i]='0';
			
		newRelCatEntry.lastBlk = recBuffer->getBlockNum();		
	}
	else {
		headInfo.numEntries++;
	}
	
	//total number of records increases
	newRelCatEntry.numRecs ++;
	//cout <<"here 1\n";	
	RelCacheTable::setRelCatEntry(relid, &newRelCatEntry);
	//cout <<"here 2\n";
	recBuffer->setHeader(&headInfo);

    slotMap[nxtFreeSlot] = '1';
    
    recBuffer->setSlotMap(slotMap);
    	
	cout << "\nThe entry is made in attribute catalog in block: "<<recBuffer->getBlockNum()<<", slot: "<<nxtFreeSlot<<endl;
	recBuffer->setRecord( (Attribute*)recAttrs, nxtFreeSlot);
	delete recBuffer;
	
    return 0;
}

int addRecordSimDummy(char * relName, int numAttrsArg, int numRecArg, int firstBlock, int lastBlock){

	//RELATION CATALOG
	//cout << "Entry made in relation catalog\n";
	int relid = RELCAT_RELID;
	
	RelCatEntry relcat_entry;
	RelCacheTable::getRelCatEntry(relid, &relcat_entry);
	
    int blockNum = relcat_entry.lastBlk;
    int numAttrs = relcat_entry.numAttrs;
    int numSlots = relcat_entry.numSlotsPerBlk;
    
    union Attribute recAttrs[numAttrs];
	
	strcpy(recAttrs[0].strval, relName);
	recAttrs[1].ival = numAttrsArg;
	recAttrs[2].ival = numRecArg;
	recAttrs[3].ival = firstBlock;
	recAttrs[4].ival = lastBlock;//root block is initialised to -1
	recAttrs[5].ival = floor(2016/(16* numAttrsArg + 1));
    
    //cout<<"Num attributes is: "<<numAttrs<<endl;
    
    int nxtFreeSlot;
    class RecBuffer *recBuffer;
    struct HeadInfo  headInfo;
    unsigned char slotMap[numSlots];
        
	recBuffer = new RecBuffer(blockNum);
	
	recBuffer->getHeader(&headInfo);
	recBuffer->getSlotMap(slotMap);
	
	nxtFreeSlot = headInfo.numEntries;

    
    /*cout << "next free slot is: "<< nxtFreeSlot<<endl;
	cout << "num Slots: "<<numSlots<<endl;*/

	if(nxtFreeSlot == numSlots) {
		//block is full

		class RecBuffer *oldRecBuffer = recBuffer;
		
		recBuffer = new RecBuffer(); //new record block alloted
		
		headInfo.rblock = recBuffer->getBlockNum();
		oldRecBuffer->setHeader(&headInfo);
		delete(oldRecBuffer);
		//no header to access
		//recBuffer->getHeader(&headInfo);
		nxtFreeSlot = 0;
		
	}
	
	RelCatEntry newRelCatEntry = relcat_entry;
	
	if(nxtFreeSlot ==0){
		//setting up the head info of new block
		headInfo.blockType = REC;
		headInfo.pblock = -1;
		headInfo.lblock = blockNum;
		headInfo.rblock = -1;
		headInfo.numEntries=1;
		headInfo.numAttrs=numAttrs;
		headInfo.numSlots=numSlots;
		//setting up slotmap
		for(int i =0;i < numSlots;++i)
			slotMap[i]='0';
			
		newRelCatEntry.numRecs++;	
		newRelCatEntry.lastBlk = recBuffer->getBlockNum();
		
		
	}
	else {
		headInfo.numEntries++;
		newRelCatEntry.numRecs ++;
	}
	
	RelCacheTable::setRelCatEntry(relid, &newRelCatEntry);
	
	recBuffer->setHeader(&headInfo);

    slotMap[nxtFreeSlot] = '1';
    
    recBuffer->setSlotMap(slotMap);
    	
	cout << "\nThe entry is made in relation catalog in block: "<<recBuffer->getBlockNum()<<", slot: "<<nxtFreeSlot<<endl;
	recBuffer->setRecord( (Attribute*)recAttrs, nxtFreeSlot);
	delete recBuffer;
    return 0;
}

int setInternalSim(int lchild, Attribute attrVal, char f, int rchild)
{
    class IndInternal *indexBlock = new IndInternal();
    struct HeadInfo *headInfo = new struct HeadInfo ();
    headInfo-> blockType = IND_INTERNAL;
	headInfo-> pblock=-1;
	headInfo-> lblock=-1;
	headInfo-> rblock=-1;
	headInfo-> numEntries=1;
	headInfo-> numAttrs=1;  //TODO: what is this?
	headInfo-> numSlots=100;
	strcpy((char*)headInfo-> reserved,"");
	
	
	indexBlock->setHeader(headInfo);
	
	struct HeadInfo * retHeadInfo = new struct HeadInfo ();
	indexBlock->getHeader(retHeadInfo);
	
	union Attribute attribute;
	
	struct InternalEntry *internalEntry = new struct InternalEntry();
	internalEntry->lChild = lchild;
	if(f=='s')
	    strcpy(internalEntry->attrVal.strval, attrVal.strval);
	else if(f=='i')
	    internalEntry->attrVal.ival = attrVal.ival;
	else
	    internalEntry->attrVal.fval = attrVal.fval;
	
	internalEntry->rChild = rchild;
	
	indexBlock->setEntry(internalEntry,0);
	
	struct InternalEntry *retInternalEntry = new struct InternalEntry();
	
	indexBlock->getEntry(retInternalEntry,0);
	
	/*cout<<retInternalEntry->lChild<<endl;
    if(f=='s')
	    cout<<retInternalEntry->attrVal.strval<<endl;
	else if(f=='i')
	    cout<<retInternalEntry->attrVal.ival<<endl;
	else
	cout<<retInternalEntry->attrVal.fval<<endl;
	
	cout<<retInternalEntry->rChild<<endl;
	*/
    return indexBlock->getBlockNum();
}
int addInternalEntry(int blockNum, int lchild, Attribute attrVal, char f, int rchild)
{
    class IndInternal *indexBlock = new IndInternal(blockNum);
    
    struct HeadInfo *headInfo = new struct HeadInfo ();
    indexBlock->getHeader(headInfo);
    
	headInfo-> numEntries+=1;
	indexBlock->setHeader(headInfo);
	
	struct InternalEntry *internalEntry = new struct InternalEntry();
	internalEntry->lChild = lchild;
	if(f=='s')
	    strcpy(internalEntry->attrVal.strval, attrVal.strval);
	else if(f=='i')
	    internalEntry->attrVal.ival = attrVal.ival;
	else
	    internalEntry->attrVal.fval = attrVal.fval;
	
	internalEntry->rChild = rchild;
	
	indexBlock->setEntry(internalEntry,headInfo-> numEntries-1);
return 1;	
}

int getInternalEntry(int blockNum, char f)
{
    class IndInternal *indexBlock = new IndInternal(blockNum);
     
    struct InternalEntry *internalEntry = new struct InternalEntry();
	
	struct HeadInfo * retHeadInfo = new struct HeadInfo ();
	
	indexBlock->getHeader(retHeadInfo);
	
	for(int i=0; i<retHeadInfo->numEntries; i++)
	{   indexBlock->getEntry(internalEntry,i);
	
	    cout<<endl<<"|"<<setw(15)<<internalEntry->lChild<<"|";
        if(f=='s')
	        cout<<setw(15)<<internalEntry->attrVal.strval<<"|";
	    else if(f=='i')
	        cout<<setw(15)<<internalEntry->attrVal.ival<<"|";
	    else
	    cout<<setw(15)<<internalEntry->attrVal.fval<<"|";
	
	    cout<<setw(15)<<internalEntry->rChild<<"|";
    }
}


int setLeafSim(int block, int slot, Attribute attrVal, char f)
{
    class IndLeaf *leafBlock = new IndLeaf();
    struct HeadInfo *headInfo = new struct HeadInfo ();
    headInfo-> blockType = IND_LEAF;
	headInfo-> pblock=-1;
	headInfo-> lblock=-1;
	headInfo-> rblock=-1;
	headInfo-> numEntries=1;
	headInfo-> numAttrs=1;  //TODO: what is this?
	headInfo-> numSlots=63;
	strcpy((char*)headInfo-> reserved,"");
	
	
	leafBlock->setHeader(headInfo);
	
	struct HeadInfo * retHeadInfo = new struct HeadInfo ();
	leafBlock->getHeader(retHeadInfo);
	/*
	cout <<"Head attributes\n";
	cout<<(retHeadInfo-> blockType)<<endl;
	cout<<retHeadInfo-> pblock<<endl;
	cout<<retHeadInfo-> lblock<<endl;
	cout<<retHeadInfo-> rblock<<endl;
	cout<<retHeadInfo-> numEntries<<endl;
	cout<<retHeadInfo-> numAttrs<<endl; 
	cout<<retHeadInfo-> numSlots<<endl;
	cout<<retHeadInfo-> reserved<<endl;*/
	union Attribute attribute;
	struct Index *index = new struct Index();
	index->block = block;
	index->slot = slot;
	if(f=='s')
	    strcpy(index->attrVal.strval, attrVal.strval);
	else if(f=='i')
	    index->attrVal.ival = attrVal.ival;
	else
	    index->attrVal.fval = attrVal.fval;
	memcpy(index->unused, "unused",8);
	
	leafBlock->setEntry(index,0);
	
	struct Index *retIndex = new struct Index();
	leafBlock->getEntry(retIndex,0);	
	/*
	cout<<retIndex->block<<endl;
	cout<<retIndex->slot<<endl;
    if(f=='s')
	    cout<<retIndex->attrVal.strval<<endl;
	else if(f=='i')
	    cout<<retIndex->attrVal.ival<<endl;
	else
	cout<<retIndex->attrVal.fval<<endl;
	cout<<retIndex->unused<<endl;*/
    return leafBlock->getBlockNum();
}

int addLeafEntry(int blockNum, int block, int slot, Attribute attrVal, char f)
{
    class IndLeaf *leafBlock = new IndLeaf(blockNum);
    
    struct HeadInfo *headInfo = new struct HeadInfo ();
    leafBlock->getHeader(headInfo);
    
	headInfo-> numEntries+=1;
	leafBlock->setHeader(headInfo);
	
	struct Index *leafEntry = new struct Index();
	leafEntry->block = block;
	leafEntry->slot = slot;
	if(f=='s')
	    strcpy(leafEntry->attrVal.strval, attrVal.strval);
	else if(f=='i')
	    leafEntry->attrVal.ival = attrVal.ival;
	else
	    leafEntry->attrVal.fval = attrVal.fval;
	memcpy(leafEntry->unused,"unused",8);	
	leafBlock->setEntry(leafEntry, headInfo-> numEntries-1);
    return 1;	
}

int getLeafEntry(int blockNum, char f)
{
    class IndLeaf *indexBlock = new IndLeaf(blockNum);
     
    struct Index *internalEntry = new struct Index();
	
	struct HeadInfo * retHeadInfo = new struct HeadInfo ();
	
	indexBlock->getHeader(retHeadInfo);
	
	cout <<"\nHead attributes\n";
    cout<<retHeadInfo-> blockType<<endl;
    cout<<retHeadInfo-> pblock<<endl;
    cout<<retHeadInfo-> lblock<<endl;//need to be changed
    cout<<retHeadInfo-> rblock<<endl;//need to be changed
    cout<<retHeadInfo-> numEntries<<endl;
    cout<<retHeadInfo-> numAttrs<<endl; 
    cout<<retHeadInfo-> numSlots<<endl;

	for(int i=0; i<retHeadInfo->numEntries; i++)
	{   indexBlock->getEntry(internalEntry,i);
	
	    cout<<endl<<"|"<<setw(15)<<internalEntry->block<<"|";
	    cout<<setw(15)<<internalEntry->slot<<"|";
        if(f=='s')
	        cout<<setw(15)<<internalEntry->attrVal.strval<<"|";
	    else if(f=='i')
	        cout<<setw(15)<<internalEntry->attrVal.ival<<"|";
	    else
	        cout<<setw(15)<<internalEntry->attrVal.fval<<"|";
	    //cout<<setw(15)<<internalEntry->unused<<"|";
	
    }
    return 0;
}

int setRelationCatRecord()
{
    initCatalogs(4, "relationCat", 6, 2,4, 4, 20);
    
    //RELATION CATALOG
	//cout << "Entry made in relation catalog\n";
	
    int blockNum = 4;
    int numAttrs = 6;
    int numSlots = 20;
    
    union Attribute recAttrs[numAttrs];
	
	strcpy(recAttrs[0].strval, "attributeCat");
	recAttrs[1].ival = 6;
	recAttrs[2].ival = 12;
	recAttrs[3].ival = 5;
	recAttrs[4].ival = 5;//root block is initialised to -1
	recAttrs[5].ival = floor(2016/(16* 6 + 1));
	
	
    //cout<<"Num attributes is: "<<numAttrs<<endl;
    
    int nxtFreeSlot = 1;
    class RecBuffer recBuffer(blockNum);
    struct HeadInfo  headInfo;
    unsigned char slotMap[numSlots];
        
	
	recBuffer.getHeader(&headInfo);
	recBuffer.getSlotMap(slotMap);
		
	headInfo.numEntries++;
	
	recBuffer.setHeader(&headInfo);

    slotMap[nxtFreeSlot] = '1';
    
    recBuffer.setSlotMap(slotMap);
    	
	//cout << "The entry is made in block: "<<recBuffer.getBlockNum()<<", slot: "<<nxtFreeSlot<<endl;
	recBuffer.setRecord( (Attribute*)recAttrs, nxtFreeSlot);	
    
}

void populateAttrCat(char *relationName, char * attributeName, int attributeType, int primaryFlag, int offset) {

	//ATTRIBUTE CATALOG
	//cout <<"Entry made in attribute catalog\n";
	int relid = ATTRCAT_RELID;
	
    int blockNum = 5;
    int numAttrs = 6;
    int numSlots = 20;
    
	
	union Attribute recAttrs[numAttrs];
	
	strcpy(recAttrs[0].strval, relationName);
	strcpy(recAttrs[1].strval, attributeName);
	recAttrs[2].ival = attributeType;
	recAttrs[3].ival = primaryFlag;
	recAttrs[4].ival = -1;//root block is initialised to -1
	recAttrs[5].ival = offset;
    
    int nxtFreeSlot;
    class RecBuffer recBuffer(blockNum);
    struct HeadInfo  headInfo;
    unsigned char slotMap[numSlots];
	
	recBuffer.getHeader(&headInfo);
	recBuffer.getSlotMap(slotMap);
	
	nxtFreeSlot = headInfo.numEntries;
    
    /*cout << "next free slot is: "<< nxtFreeSlot<<endl;
	cout << "num Slots: "<<numSlots<<endl;*/
	
	headInfo.numEntries++;
		
	recBuffer.setHeader(&headInfo);

    slotMap[nxtFreeSlot] = '1';
    
    recBuffer.setSlotMap(slotMap);
    	
	//cout << "The entry is made in block: "<<recBuffer.getBlockNum()<<", slot: "<<nxtFreeSlot<<endl;
	recBuffer.setRecord( (Attribute*)recAttrs, nxtFreeSlot);
}

int setAttributeCatRecord()
{
    initCatalogs(5, (char *)"relationCat", "relation_name", STRING, 0, -1, 0 );
    
	
    populateAttrCat((char *) "relationCat", (char *)"#attributes", INT, 0, 1 );   
    populateAttrCat((char *) "relationCat", (char *)"#records", INT, 0, 2 );   
    populateAttrCat((char *) "relationCat", (char *)"first block", INT, 0, 3 );   
    populateAttrCat((char *) "relationCat", (char *)"last block", INT, 0, 4 );   
    populateAttrCat((char *) "relationCat", (char *)"#slots", INT, 0, 5 );   
    
    populateAttrCat((char *) "attributeCat", (char *)"relation_name", STRING, 0, 0 );
    populateAttrCat((char *) "attributeCat", (char *)"attribute name", STRING, 0, 1 );
    populateAttrCat((char *) "attributeCat", (char *)"attribute type", INT, 0, 2 );
    populateAttrCat((char *) "attributeCat", (char *)"primary flag", INT, 0, 3 );
    populateAttrCat((char *) "attributeCat", (char *)"root block", INT, 0, 4 );
    populateAttrCat((char *) "attributeCat", (char *)"offset", INT, 0, 5 );
    
    return 1;
    
}

void initFunctions(){
	setRelationCatRecord();
    setAttributeCatRecord();	
}

int main() 
{
    static Disk disk;
    //TODO: static
    StaticBuffer staticBuffer;
//    OpenRelTable openRelTable;
    initFunctions();
    
    //AppuNote: the openRelTable constructor should be called only after initialising the relation and attribute catalog blocks
	RelCacheTable relCacheTable;
	AttrCacheTable attrCacheTable;
	OpenRelTable openRelTable;
	
    
	cout<<"\n\n\n**************INITIALISATIONS OVER**************\n\n\n";    
	
	

	char relationName[50], opStr[5];
	int choice, numAttrs, relid, intAttrType,op;
	char format[125], attrName[100], attrType[10];
	union Attribute attrVal;
	AttrCatEntry attrcat_entry;
	recId rec_id;
	do
	{
		cout <<"\n\n-----------MENU-----------\n";
		cout<<"1. Create a new relation.\n2. Add new record.\n3. Display relation.\n4. Display Relation Catalog. \n5. Display    Attribute Catalog\n\n6. Set a new Internal Entry\n7. Add to Internal Entry with block number\n8. Get Internal Entry\n\n9.Set a new Leaf Entry\n10. Add to Leaf Entry given a block number\n11. Get Leaf Entry\n\n12.Display Relation Catalog Cache\n\n13.Create B+ Tree\n14. Display B+ Tree\n15. Search B+ Tree\n\n100. Display buffer metaInfo\n101. Display disk alloc map\n\n900. Shut Down\n\n";
		cout <<"Enter your choice:" ;
	    cin>>choice;
        cout <<endl;
        BplusTree *bplusTree = NULL;
        
            switch(choice) {
                case 1:
                	//AppuNote: when creating a new table no need to use the catalog caches...
                	//Instead populate the catalog blocks bcoz in order to call open rel the entries must be first in the block
                	cout <<"---NEW RELATION---\n";
                     cout<<"Enter relation name: "; 
	                 cin>>relationName;
	                 cout<<"Enter number of attributes: "; 
	                 cin>>numAttrs;
	                 cout <<"\n";
	                 
	                 int i, blockNum;
	                 for(i=0; i<numAttrs; i++)
	                 {
	                    
	                     cout<<"Enter attribute "<<i<<" name : "; 
	                     cin>>attrName;
	                     cout<<"Enter type of attribute "<<i<<"(str/int/float) : "; 
	                     cin>>attrType;
	                     int intAttrType;
	                     if(strcmp(attrType,"str")==0)
	                     	intAttrType = STRING;
	                     else if(strcmp(attrType,"int")==0)
	                     	intAttrType = INT;
	                     else
	                     	intAttrType = FLOAT;
	                     cout <<endl;
	                     addRecordSimDummy((char *)relationName, (char *)attrName, intAttrType, 0, i);
	                     if(strcmp(attrType,"str")==0 )
	                        format[i]='s';
	                     else if(strcmp(attrType,"int")==0 )
	                        format[i]='i';
	                     else 
	                        format[i]='f';
	                 }
	                 format[i]='\0';
	                 addRecordSimDummy((char *)relationName, numAttrs, 0, -1, -1);
	                 
                    break;
                case 2:
                	cout <<"---INSERT RECORD---\n";
                    cout<<"Enter relation name: "; 
	                 cin>>relationName;
	                 addRecordSimDummy((char *)relationName);
                    
                    break;
                case 3:
                	cout <<"---DISPLAY RELATION---\n";
                    cout<<"Enter relation name: "; 
	                cin>>relationName;
	                getRecordSimDummy((char *)relationName);
	                break;
                case 4:
                	cout <<"---DISPLAY RELATION CATALOG---\n";
                    getRecordSimDummy((char *)"relationCat");
	                break;
                case 5:
                	cout <<"---DISPLAY ATTRIBUTE CATALOG---\n";
                    getRecordSimDummy((char *)"attributeCat");
	                break;
	           case 6:
		           int lchild, rchild;
		           char type;
		           Attribute attrval;
		           
		           cout <<"---ENTER INTERNAL ENTRY---\n";
		           cout<<"Enter lchild:";
		           cin>>lchild;
		           cout <<"Enter type(s/i/f):";
		           cin>>type;
		           cout <<"Enter attrVal:";
		           if(type=='s')
		               cin>>attrval.strval;
		           if(type=='i')
		               cin>>attrval.ival;
		           if(type=='f')
		               cin>>attrval.fval;
		           cout << "Enter rchild:";
		           cin>>rchild;
		           cout<<"\nBlock Num = "<<setInternalSim(lchild, attrval, type, rchild);
		           break;
               case 7:
				   cout<<"Enter block number to which new entry is to be inserted: ";

		            cin>>blockNum;
		            cout<<"Enter lchild:";
		           cin>>lchild;
		           cout <<"Enter type(s/i/f):";
		           cin>>type;
		           cout <<"Enter attrVal:";
		           if(type=='s')
		               cin>>attrval.strval;
		           if(type=='i')
		               cin>>attrval.ival;
		           if(type=='f')
		               cin>>attrval.fval;
		           cout << "Enter rchild:";
		           cin>>rchild;
		             addInternalEntry(blockNum, lchild, attrval, type, rchild);
		             break;
           case 8:          
			cout<<"Get Internal Entry\n";
			cout<<"Enter block num: ";
			cin>>blockNum;
			cout<<"Enter type(s/i/f): ";
			cin>>type;
			getInternalEntry(blockNum, type);
			break;
		case 9:
			cout<<"---SET A NEW LEAF ENTRY---\n\n";
			int block,slot;
			cout<<"Enter block : ";
			cin>>block;
			cout<<"Enter slot : ";
			cin>>slot;
			cout<<"Enter type(s/i/f) : ";
			cin>>type;
			cout<<"Enter attribute value : ";
			if(type=='s')
		   cin>>attrval.strval;
		if(type=='i')
		   cin>>attrval.ival;
		if(type=='f')
		   cin>>attrval.fval;
		cout<<"\nBlock Num = "<<setLeafSim(block,slot,attrval,type)<<endl;
			break;

			case 10:
				cout<<"---ADD A LEAF ENTRY GIVEN A BLOCK NUMBER---\n\n";
				cout<<"Enter leaf block num: ";
				cin>>blockNum;
				cout<<"Enter block : ";
				cin>>block;
				cout<<"Enter slot : ";
				cin>>slot;
				cout<<"Enter type(s/i/f) : ";
				cin>>type;
				cout<<"Enter attribute value : ";
				if(type=='s')
			   cin>>attrval.strval;
			if(type=='i')
			   cin>>attrval.ival;
			if(type=='f')
			   cin>>attrval.fval;
			addLeafEntry(blockNum, block, slot, attrval,type);
				break;
			   
			case 11:
				cout<<"---GET LEAF ENTRY---\n\n";
				cout<<"Enter block num: ";
				cin>>blockNum;
				cout<<"Enter type(s/i/f) : ";
				cin>>type;
				getLeafEntry(blockNum,type);
				break;             
			case 12:
				OpenRelTable::dispRelCatCache();
				break;
			case 13:
				cout<<"Enter relation name: "; 
	            cin>>relationName;
	            cout<<"Enter attribute name : "; 
	            cin>>attrName;
	            relid = OpenRelTable::OpenRel(relationName);
				bplusTree = new BplusTree(relid, attrName);
				//cout<<bplusTree->getRootBlock();
				break;
			case 14:
				cout<<"Enter relation name: "; 
	            cin>>relationName;
	            cout<<"Enter attribute name : "; 
	            cin>>attrName;
	            relid = OpenRelTable::OpenRel(relationName);
				bplus_display(relid, attrName);
				break;
			case 15:
				cout<<"Enter relation name: "; 
	            cin>>relationName;
	            cout<<"Enter attribute name : "; 
	            cin>>attrName;
	            relid = OpenRelTable::OpenRel(relationName);
	            AttrCacheTable::getAttrCatEntry(relid, attrName, &attrcat_entry);
	            intAttrType = attrcat_entry.attrType;
	            cout << "Enter attribute val: ";
	            if(intAttrType == STRING)
	            	cin>>attrVal.strval;
	            else if (intAttrType == INT)
	            	cin>>attrVal.ival;
	            else
	            	cin>>attrVal.fval;
	            cout <<"Enter operation:(EQ/LE/LT/GE/GT/NE): ";
	            cin >> opStr;
	            if(strcmp(opStr,"EQ")==0)
		            op = EQ;
		        else if(strcmp(opStr,"LE")==0)
		            op = LE;
	            else if(strcmp(opStr,"LT")==0)
		            op = LT;
	            else if(strcmp(opStr,"GE")==0)
		            op = GE;
	            else if(strcmp(opStr,"GT")==0)
		            op = GT;
	            else if(strcmp(opStr,"NE")==0)
		            op = NE;
		        cout<<endl;
		        bplusTree = new BplusTree(relid, attrName);
		        for(i = 0;;++i){
	            	rec_id = bplusTree->bplusSearch(attrVal, op);
	            	if(rec_id.block!=-1)
			        	cout << "Entry found in block :"<<rec_id.block<<" slot: "<<rec_id.slot;
			        else
			        	break;
			        cout <<endl<<endl;
	            }
	            if(i==0)
	            	cout << "Entry not found";
	            break;
	        
	        case 100:
				cout<<"Buffer Meta Info: \n"; 
	            StaticBuffer::dispMetaInfo();
				break;
			case 101:
				cout<<"Block Alloc Map: \n"; 
	            StaticBuffer::dispblockAllocMap();
				break;
            }                


	}while(choice!=900);
 
	return 0;
}
