#include "blockBuffer.h"
#include "bits/stdc++.h"

typedef struct recId{
	int block;
	int slot;
} recId;

typedef struct IndexId{
	int block;
	int index;
} IndexId;

typedef struct OpenRelTableMetaInfo {
    bool free;
    char relName[ATTR_SIZE];
}OpenRelTableMetaInfo;

typedef struct RelCatEntry {
    char relName[ATTR_SIZE];
    int numAttrs;
    int numRecs;
    int firstBlk;          
    int lastBlk;
    int numSlotsPerBlk;      
} RelCatEntry ;

typedef struct RelCacheEntry {
	RelCatEntry entry;
    bool dirty;
    recId recid;
    recId searchIndex; 
    struct RelCacheEntry *next;
} RelCacheEntry;

typedef struct AttrCatEntry {
        char relName[ATTR_SIZE];
        char attrName[ATTR_SIZE];
        int attrType;     // can be INT/FLOAT/STR
        bool primaryFlag; // Currently unused
        int rootBlock;   
        int offset; // offset of the attribute in the relation
    } AttrCatEntry ;

typedef struct AttrCacheEntry {
	AttrCatEntry entry;
    bool dirty;
    recId recid;
    IndexId searchIndex; 
    struct AttrCacheEntry *next;
} AttrCacheEntry;

class RelCacheTable{
    
    friend class OpenRelTable;
    
    private:
    static RelCacheEntry *relCache[MAXOPEN];
    static void recordToRelCacheEntry(union Attribute entry [ATTRCAT_SIZE], RelCacheEntry *relCacheEntry);
    static void relCacheEntryToRecord(union Attribute record[ATTRCAT_SIZE], RelCacheEntry *relCacheEntry);
    
    
    public: 
    static int getRelCatEntry(int relid, RelCatEntry *relCatBuf);
	static int setRelCatEntry(int relid, RelCatEntry *relCatBuf);
	static int getSearchIndex(int relid, recId *searchIndex);
	static int setSearchIndex(int relid, recId *searchIndex);
	
};

RelCacheEntry *RelCacheTable::relCache[MAXOPEN] = {NULL};

class AttrCacheTable{

    friend class OpenRelTable;
    
    private:
    static AttrCacheEntry *attrCache[MAXOPEN];
    static void recordToAttrCacheEntry(union Attribute entry [ATTRCAT_SIZE], AttrCacheEntry *attrCacheEntry);
	static void attrCacheEntryToRecord( union Attribute record[ATTRCAT_SIZE], AttrCacheEntry *attrCacheEntry);
	
	public:
	
	static int getAttrCatEntry(int relid, char attrName[ATTR_SIZE], AttrCatEntry *attrCatBuf);
	static int getAttrCatEntry(int relid, int attrOffset, AttrCatEntry *attrCatBuf);
	static int setAttrCatEntry(int relid, char attrName[ATTR_SIZE], AttrCatEntry *attrCatBuf);
	static int setAttrCatEntry(int relid, int attrOffset, AttrCatEntry *attrCatBuf);
	static int getSearchIndex(int relid, char attrName[ATTR_SIZE], IndexId * index);
	static int getSearchIndex(int relid, int attrOffset, IndexId * index);
	static int setSearchIndex(int relid, char attrName[ATTR_SIZE], IndexId* searchIndex);
	static int setSearchIndex(int relid, int attrOffset, IndexId *searchIndex);
	
};
AttrCacheEntry *AttrCacheTable::attrCache[MAXOPEN] = {NULL};



class OpenRelTable{

private:
    static OpenRelTableMetaInfo tableMetaInfo[MAXOPEN];
	static int getFreeOpenRelTableEntry();

public:	

	static int OpenRel(char relName[ATTR_SIZE]);
	static int CloseRel(int relid);
	static int getRelId(char relName[ATTR_SIZE]);
	OpenRelTable();
   ~OpenRelTable();    
    static void dispRelCatCache();
    static void printCache();
};

OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAXOPEN];



/******************* compare Function *******************/
int compare(union Attribute attr1, union Attribute attr2, int attrType){
	//should be STRING as per the constants.h file
    if (attrType == STRING)
       return strcmp(attr1.strval, attr2.strval);
    
    else if (attrType == INT){
    	if(attr1.ival < attr2.ival)
    		return -1;
    	else if (attr1.ival == attr2.ival)
    		return 0;
    	else
    		return 1;
    }
    else if (attrType == FLOAT){
    	if(attr1.fval < attr2.fval)
    		return -1;
    	else if (attr1.fval == attr2.fval)
    		return 0;
    	else
    		return 1;
    } 
}

/******************* linear_search Function *******************/

struct recId linear_search(int relid, char attrName[ATTR_SIZE], union Attribute attrval, int op){ 	

	//get the previous record id from the relation cache corresponding to the relation with Id=relid
	recId searchIndex;
	RelCacheTable::getSearchIndex(relid, &searchIndex); 
		
	int firstBlock, lastBlock, slot;

	RelCatEntry entry;
	RelCacheTable::getRelCatEntry(relid, &entry);
	
	lastBlock =  entry.lastBlk;

	int maxSlots = entry.numSlotsPerBlk;
	int numAttrs =  entry.numAttrs;
	
	if(searchIndex.block ==-1 && searchIndex.slot == -1){ 
		//It is the first time that linear search search for the record with the attribute value attrval 		
		//get the first record block of the relation from the relation cache 		
		//using getRelCatEntry() method of OpenRelTable in cache layer 		
		//block = the first record block of the relation 
		firstBlock = entry.firstBlk;
		
		//slot = 0 	
		slot = 0;
						
	} 	
	else{ 
		//if the linear search knows the hit from previous search 		
	
		//TODO: Changes: updated firstBlock so as to consider the possibility of starting a new block//
	
		// slot = the previous record id slot 	
		slot = (searchIndex.slot + 1) % maxSlots;
	
		// block = the previous record id block
		if(slot == 0) {
	
			//TODO: WARNING: Is it necessary that the blocks will be continuos block numbers??//
			RecBuffer rec_buffer(searchIndex.block);
		
			struct HeadInfo head;
			rec_buffer.getHeader(&head);
		
			firstBlock = head.rblock;//next block
			//firstBlock = searchIndex.block+1;
		}
		else
			firstBlock = searchIndex.block;
	
	
	}
	
	//The following code searches for the next record in the relation that satisfies the given condition 	
	int block = firstBlock;
	
	//cout <<"Block to be searched: "<<block<<", slot: "<<slot<<endl;
	
	//get the record of the relation using the following buffer layer functions 
	RecBuffer rec_buffer(block);
	
	unsigned char slotMap[maxSlots];
	rec_buffer.getSlotMap(slotMap);
	
	//arguments of compare function called below
	AttrCatEntry attrcat_entry;
		
	AttrCacheTable::getAttrCatEntry(relid, attrName, &attrcat_entry);
	int attrOffset = attrcat_entry.offset;
	int attrType = attrcat_entry.attrType;
	
	//Start from block and iterate over the records of the relation{
	//while(!lastBlockSeen) {
	while(block != -1) {		
		
		//If slot is free
		if(slotMap[slot] == '1') {
			union Attribute record[numAttrs];
			rec_buffer.getRecord((Attribute *)&record, slot);
			
			//compare record's attribute value to the the given attrval as below: 		
			//storing the outcome of comparision in the variable flag 
			

			int flag = compare(attrval, record[attrOffset], attrType);
		
			//TODO: WARNING: UNSET Not defined in constants.h//
			int cond = UNSET;
			//Next task is to check whether this record satisfies the given condition. 		
			//It is determined based on the output of previous comparision and the op value received. 		
			//The following code sets the cond variable if the condition is satisfied. 		
			switch(op){ 		 		
				case NE: //if op is "not equal to" 				
					if(flag != 0){ 
						//i.e if the record's attribute value is not equal to the given attrval 					
						//SET the cond variable
						cond = SET;
					} 				
					break; 				 			
				case LT: 
					//if op is "less than" 				
					if(flag < 0){ 
						//i.e if the record's attribute value is less than the given attrval 					
						//SET the cond variable 				
						cond = SET;
					} 				
					break; 			 			
				case LE: 
					//if op is "less than or equal to" 				
					if(flag <= 0){ 
						//i.e if the record's attribute value is less than or equal to the given attrval 					
						//SET the cond variable 				
						cond = SET;
					} 				
					break; 			 			
				case EQ: 
					//if op is "equal to" 				
					if(flag == 0){ 
						//i.e if the record's attribute value is equal to the given attrval 					
						//SET the cond variable 				
						cond = SET;
					} 				
					break; 			 			
				case GT: 
					//if op is "greater than" 				
					if(flag > 0){ 
						//i.e if the record's attribute value is greater than the given attrval 					
						//SET the cond variable 				
						cond = SET;
					} 				
					break; 			 			
				case GE: 
					//if op is "greater than or equal to" 				
					if(flag >= 0){ 
						//i.e if the record's attribute value is greater than or equal to the given attrval 					
						//SET the cond variable 				
						cond = SET;
					} 				
					break; 		
				} 		 		
			if(cond == SET){
				
				//recid = {block, slot} 
				struct recId newRecId;
				newRecId.block = block;
				newRecId.slot = slot;

				//record id of the record that satisfies the given condition 			
				/*set the previous record id in the relation cache as the record id of the record that sastifies the given condition*/		 	

				RelCacheTable::setSearchIndex(relid, &newRecId);
				
				return newRecId; 		
			}
		}
		else
			break;
		 		 		
		//get the next record id by adjusting the block and slot
		slot = (slot + 1 ) % maxSlots;
		if(slot == 0) {//new block
			//block++;
			
			struct HeadInfo head;
			rec_buffer.getHeader(&head);	
			
			block = head.lblock;
			//loading new block
			
			if(block== -1)
				continue;
				
			rec_buffer = RecBuffer(block);
			
			//updating slotmap
			rec_buffer.getSlotMap(slotMap);
		}
	}
		 	
	//return {-1, -1}; //i.e., no record in the relation with Id relid satisfies the given condition 
	struct recId newRecId;
	newRecId.block = -1;
	newRecId.slot = -1;
		
	//TODO: Changes: setPrevRecord to reflect that the linear search has come to an end//
	RelCacheTable::setSearchIndex(relid, &newRecId);
	return newRecId;
}


/******************* getFreeOpenRelTableEntry Function *******************/
//NOTE: Helper function
int OpenRelTable::getFreeOpenRelTableEntry(){
	for(int i =0;i<MAXOPEN;++i) {
		if(OpenRelTable::tableMetaInfo[i].free==true)
			return i;
	}
	return E_CACHEFULL;
}


/******************* recordToAttrCacheEntry Function *******************/
void AttrCacheTable::recordToAttrCacheEntry(union Attribute entry [ATTRCAT_SIZE], struct AttrCacheEntry *attrCacheEntry){
	
	
	strcpy(attrCacheEntry-> entry.relName, entry[0].strval);
	strcpy(attrCacheEntry-> entry.attrName, entry[1].strval);
	attrCacheEntry-> entry.attrType = entry[2].ival;
	attrCacheEntry-> entry.primaryFlag = entry[3].ival;
	attrCacheEntry-> entry.rootBlock = entry[4].ival;
	attrCacheEntry-> entry.offset = entry[5].ival;
	
	attrCacheEntry-> dirty = false;
	struct recId invalidRecId;
	invalidRecId.block = -1;
	invalidRecId.slot = -1;
	attrCacheEntry->recid = invalidRecId;
	struct IndexId invalidIndId;
	invalidIndId.block = -1;
	invalidIndId.index = -1;
	attrCacheEntry->searchIndex = invalidIndId;
	attrCacheEntry->next = NULL;
}


void RelCacheTable::recordToRelCacheEntry(union Attribute entry[ATTRCAT_SIZE], RelCacheEntry *relCacheEntry)
{
	strcpy(relCacheEntry-> entry.relName, entry[0].strval);
	relCacheEntry-> entry.numAttrs = entry[1].ival;
	relCacheEntry-> entry.numRecs = entry[2].ival;
	relCacheEntry-> entry.firstBlk = entry[3].ival;
	relCacheEntry-> entry.lastBlk = entry[4].ival;
	relCacheEntry-> entry.numSlotsPerBlk = entry[5].ival;
	relCacheEntry-> dirty = false;
	struct recId invalidRecId;
	invalidRecId.block = -1;
	invalidRecId.slot = -1;
	relCacheEntry->recid = invalidRecId;
	relCacheEntry->searchIndex = invalidRecId;
	
	
}
void RelCacheTable::relCacheEntryToRecord(union Attribute entry[ATTRCAT_SIZE], RelCacheEntry *relCacheEntry)
{
	strcpy(entry[0].strval, relCacheEntry-> entry.relName);
	entry[1].ival = relCacheEntry-> entry.numAttrs;
	entry[2].ival = relCacheEntry-> entry.numRecs ;
	entry[3].ival = relCacheEntry-> entry.firstBlk;
	entry[4].ival = relCacheEntry-> entry.lastBlk;
	entry[5].ival = relCacheEntry-> entry.numSlotsPerBlk;
	
}

/******************* AttrCacheEntryToRecord Function *******************/

void AttrCacheTable::attrCacheEntryToRecord( union Attribute record[ATTRCAT_SIZE], AttrCacheEntry *attrCacheEntry) {
	
	strcpy(record[0].strval, attrCacheEntry->entry.relName);
	strcpy(record[1].strval, attrCacheEntry->entry.attrName);
	record[2].ival = attrCacheEntry->entry.attrType;
	record[3].ival = attrCacheEntry->entry.primaryFlag;
	record[4].ival = attrCacheEntry->entry.rootBlock;
	record[5].ival = attrCacheEntry->entry.offset;

}


/******************* OpenRel Function *******************/
int OpenRelTable::OpenRel(char relName[ATTR_SIZE]){

	//check if the relation with relation name relName already opened
	int relid = getRelId((char *)relName);

	if(relid != E_NOTOPEN){

		return relid;
	}
	
	//find a free slot in the relation cache
	relid = getFreeOpenRelTableEntry();
	

	
	//if free slot is not found in the relation cache
	if(relid == E_CACHEFULL)
		return E_CACHEFULL;
	
	recId invalidId;
	invalidId.block = -1;
	invalidId.slot = -1;
	RelCacheTable::setSearchIndex(RELCAT_RELID, &invalidId);
	
	union Attribute relNameAttr;
	strcpy(relNameAttr.strval , relName);
	
	//search for the relation catalog entry with relation name relName in the relation catalog 
	struct recId relcat_recid = linear_search(RELCAT_RELID, (char *)"relation_name", relNameAttr, EQ);

	if(relcat_recid.block == -1 && relcat_recid.slot == -1){
		return E_RELNOTEXIST;
	}
	
	tableMetaInfo[relid].free = false;
	strcpy(tableMetaInfo[relid].relName,relName);
	
	RelCacheTable::setSearchIndex(ATTRCAT_RELID, &invalidId);

	
	struct recId attrcat_recid = linear_search(ATTRCAT_RELID, (char *)"relation_name", relNameAttr, EQ);

	
	struct AttrCacheEntry *curr, *prev=NULL, *attrlist_head;
	
	union Attribute attrcat_record[ATTRCAT_SIZE];
	AttrCacheEntry *attrCacheEntry;
	while(attrcat_recid.block!= -1 && attrcat_recid.slot !=-1) {
        RecBuffer rec_buffer(attrcat_recid.block);
        rec_buffer.getRecord(attrcat_record, attrcat_recid.slot);
        
        attrCacheEntry = new AttrCacheEntry;
        AttrCacheTable::recordToAttrCacheEntry(attrcat_record, attrCacheEntry);  
        curr = attrCacheEntry;    
        
        curr->recid = attrcat_recid;
        if(prev!=NULL) 
        	prev->next = curr;
        else 
        	attrlist_head = curr;        
        prev = curr;        	
        attrcat_recid = linear_search(ATTRCAT_RELID, (char *)"relation_name", relNameAttr, EQ);  
	}
            


	//get the relation catalog entry with relation name relName from the relation catalog
	RecBuffer rec_buffer(relcat_recid.block); //(recid of the relation record = relcat_recid)
	
	union Attribute entry[6];

	rec_buffer.getRecord(entry, relcat_recid.slot);
	
	RelCacheEntry *relCacheEntry = new RelCacheEntry;
	RelCacheTable::recordToRelCacheEntry(entry, relCacheEntry);
	RelCacheTable::relCache[relid] = relCacheEntry;
	
	RelCacheTable::relCache[relid]->recid = relcat_recid;
	AttrCacheTable::attrCache[relid] = attrlist_head;
	
	return relid;
}

/******************* CloseRel Function *******************/
int OpenRelTable::CloseRel(int relid){

	if(relid< 0 || relid>= MAXOPEN){	//check whether relation id (relid) lies in the range
		return E_OUTOFBOUND;
	}
	
	if(tableMetaInfo[relid].free){ //slot corresponding to the relid in the relation is free
		return E_NOTOPEN;
	}

	struct AttrCacheEntry *attr_list_head = AttrCacheTable::attrCache[relid], * temp;
	RecBuffer rec_buffer;
	union Attribute record[ATTRCAT_SIZE];
	AttrCacheEntry *attrCacheEntry;
	
	while(attr_list_head != NULL) {
	
		//if the attribute catalog entry is modified (i.e, dirty flag is set){
		if(attr_list_head->dirty) {
			rec_buffer=RecBuffer(attr_list_head->recid.block);	           
			AttrCacheTable::attrCacheEntryToRecord(record, attr_list_head);
			rec_buffer.setRecord(record, attr_list_head->recid.slot); 
			
		}
		//free the attribute catlog node in the attribute cache linked list
		temp = attr_list_head;
		attr_list_head = attr_list_head->next;
		delete(temp);
	}
	
	if(RelCacheTable::relCache[relid]->dirty) {
		rec_buffer=RecBuffer(RelCacheTable::relCache[relid]->recid.block);					
		RelCacheTable::relCacheEntryToRecord(record, RelCacheTable::relCache[relid]);		
		rec_buffer.setRecord(record, RelCacheTable::relCache[relid]->recid.slot);	
	}
	
	RelCacheTable::relCache[relid] = NULL;
	AttrCacheTable::attrCache[relid] = NULL;
	
	tableMetaInfo[relid].free = true;
	strcpy(tableMetaInfo[relid].relName,"");
	return SUCCESS;
}

/******************* getRelCatEntry Function *******************/
int RelCacheTable::getRelCatEntry(int relid, RelCatEntry *relCatBuf){

	if(relid< 0 || relid>= MAXOPEN){	//check relation id (relid) lies in the range
		return E_OUTOFBOUND;
	}
	if(RelCacheTable::relCache[relid] == NULL){ //slot corresponding to the relid in the relation is free
		return E_NOTOPEN;
	}
	memcpy(relCatBuf, &(RelCacheTable::relCache[relid]->entry), sizeof(RelCatEntry));

	return SUCCESS;
}

/******************* setRelCatEntry Function *******************/
int RelCacheTable::setRelCatEntry(int relid, RelCatEntry *relCatBuf){
	if(relid< 0 || relid>= MAXOPEN){	//check relation id (relid) lies in the range
		return E_OUTOFBOUND;
	}
	if(RelCacheTable::relCache[relid]==NULL){ //slot corresponding to the relid in the relation is free
		return E_NOTOPEN;
	}
	
	//copy the relation catalog in the relCatBuf to the relation cache
	memcpy(&(RelCacheTable::relCache[relid]->entry), relCatBuf, sizeof(RelCatEntry));
	RelCacheTable::relCache[relid]->dirty = true;
	return SUCCESS;
}

/******************* getSearchIndex Function *******************/
int RelCacheTable::getSearchIndex(int relid, recId *searchIndex){

	if(relid< 0 || relid>= MAXOPEN){
		//check whether relation id (relid) lies in the range
		return E_OUTOFBOUND;
	}
	
	if(RelCacheTable::relCache[relid]==NULL){ 
		//slot corresponding to the relid in the relation is free
		return E_NOTOPEN;
	}
	
	/*copy the previous record id of the relation in the relation cache to the searchIndex*/
	*searchIndex = RelCacheTable::relCache[relid]->searchIndex;
	
	return SUCCESS;
}

/******************* setSearchIndex Function *******************/
int RelCacheTable::setSearchIndex(int relid, recId *searchIndex){
	if(relid< 0 || relid>= MAXOPEN){	//check relation id (relid) lies in the range
		return E_OUTOFBOUND;
	}
	if(RelCacheTable::relCache[relid]==NULL){ 
		//slot corresponding to the relid in the relation is free
		return E_NOTOPEN;}
	
	/*copy the previous record id of the relation from the searchIndex to the relation cache*/
	RelCacheTable::relCache[relid]->searchIndex = *searchIndex;
	
	return SUCCESS;
}

/******************* getAttrCatEntry Function *******************/
int AttrCacheTable::getAttrCatEntry(int relid, char attrName[ATTR_SIZE], AttrCatEntry *attrCatBuf){ 

	if(relid< 0 || relid>= MAXOPEN){
		//check relation id (relid) lies in the range
		return E_OUTOFBOUND;
	} 	

		if(AttrCacheTable::attrCache[relid]==NULL){ 
		//slot corresponding to the relid in the relation is free
		return E_NOTOPEN;	} 	
			 				
	//Iterate over all the attributes in the attribute cache corresponding to the relation with Id relid{ 		
	struct AttrCacheEntry * attrCacheEntry = AttrCacheTable::attrCache[relid];
	
	while(attrCacheEntry != NULL){
		//if the attrName/attrOffset of the attribute equals to the given attrName/attrOffset
		if(strcmp((attrCacheEntry-> entry). attrName , attrName)==0){
		
			//copy the attribute catalog in the attribute cache to the attrCatBuf 
			memcpy(attrCatBuf, &(attrCacheEntry-> entry), sizeof(AttrCatEntry));
			return SUCCESS; 
		}

		attrCacheEntry =attrCacheEntry->next;

	}
	
	return E_ATTRNOTEXIST;
 }

/******************* getAttrCatEntry Function *******************/
int AttrCacheTable::getAttrCatEntry(int relid, int attrOffset, AttrCatEntry *attrCatBuf){ 

	if(relid< 0 || relid>= MAXOPEN){
		//check relation id (relid) lies in the range
		return E_OUTOFBOUND;
	} 	

	if(AttrCacheTable::attrCache[relid]==NULL){ 
		//slot corresponding to the relid in the relation is free
		return E_NOTOPEN;	} 	 	
			 				
	//Iterate over all the attributes in the attribute cache corresponding to the relation with Id relid{ 		
	struct AttrCacheEntry * attrCacheEntry = AttrCacheTable::attrCache[relid];
	
	while(attrCacheEntry != NULL){
		//if the attrName/attrOffset of the attribute equals to the given attrName/attrOffset
		if((attrCacheEntry-> entry). offset == attrOffset){
		
			//copy the attribute catalog in the attribute cache to the attrCatBuf 
			memcpy(attrCatBuf, &(attrCacheEntry-> entry), sizeof(AttrCatEntry));
			return SUCCESS; 
		}

		attrCacheEntry =attrCacheEntry->next;

	}
	
	return E_ATTRNOTEXIST;
 }

/******************* setAttrCatEntry Function V1*******************/
int AttrCacheTable::setAttrCatEntry(int relid, char attrName[ATTR_SIZE], AttrCatEntry *attrCatBuf){
	if(relid< 0 || relid>= MAXOPEN){	//check relation id (relid) lies in the range
		return E_OUTOFBOUND;
	}
	if(AttrCacheTable::attrCache[relid]==NULL){ 
		//slot corresponding to the relid in the relation is free
		return E_NOTOPEN;	} 
	
	//Iterate over all the attributes in the attribute cache corresponding to the relation with Id relid{
	AttrCacheEntry * curr = AttrCacheTable::attrCache[relid];
	while(curr!= NULL) {
		//if the attrName/attrOffset of the attribute equals to the given attrName/attrOffset{
		if(strcmp( curr-> entry. attrName, attrName) == 0) {
			//copy the attrCatBuf to the corresponding attibute node in the attribute cache
			//set the dirty flag corresponding to the attribute in the attribute cache
			memcpy(&(curr-> entry), attrCatBuf, sizeof(AttrCatEntry));
			curr-> dirty = true;
			return SUCCESS;
		}
		curr = curr->next;
	}
	return E_ATTRNOTEXIST;
}

/******************* setAttrCatEntry Function V2*******************/
int AttrCacheTable::setAttrCatEntry(int relid, int attrOffset, AttrCatEntry *attrCatBuf){
	if(relid< 0 || relid>= MAXOPEN){	//check relation id (relid) lies in the range
		return E_OUTOFBOUND;
	}
	if(AttrCacheTable::attrCache[relid]==NULL){ 
		//slot corresponding to the relid in the relation is free
		return E_NOTOPEN;	} 
	
	//Iterate over all the attributes in the attribute cache corresponding to the relation with Id relid{
	AttrCacheEntry * curr = AttrCacheTable::attrCache[relid];
	while(curr!= NULL) {
		//if the attrName/attrOffset of the attribute equals to the given attrName/attrOffset{
		if(curr-> entry. offset == attrOffset) {
			//copy the attrCatBuf to the corresponding attibute node in the attribute cache
			//set the dirty flag corresponding to the attribute in the attribute cache
			memcpy(&(curr-> entry), attrCatBuf, sizeof(AttrCatEntry));
			curr-> dirty = true;
			return SUCCESS;
		}
		curr = curr->next;
	}
	return E_ATTRNOTEXIST;
}

/******************* getSearchIndex Function *******************/
int AttrCacheTable::getSearchIndex(int relid, char attrName[ATTR_SIZE], IndexId *indId){
	if(relid< 0 || relid>= MAXOPEN){	//check relation id (relid) lies in the range
		return E_OUTOFBOUND;
	}
	if(AttrCacheTable::attrCache[relid]==NULL){ 
		//slot corresponding to the relid in the relation is free
		return E_NOTOPEN;	} 
	
	//Iterate over all the attributes in the attribute cache corresponding to the relation with Id relid{
	AttrCacheEntry * curr = AttrCacheTable::attrCache[relid];
	while(curr!= NULL) {
		//if the attrName/attrOffset of the attribute equals to the given attrName/attrOffset{
		if(strcmp( curr-> entry. attrName, attrName) == 0) {
			/*copy the previous index id (in meta informatiom) corresponding to 
			  the attribute in the attribute cache to the indId*/
			memcpy(indId, &(curr-> searchIndex), sizeof(IndexId));
			return SUCCESS;
		}
		curr = curr->next;
	}
	return E_ATTRNOTEXIST;
}

/******************* getSearchIndex Function *******************/
int AttrCacheTable::getSearchIndex(int relid, int attrOffset, IndexId *indId){
	if(relid< 0 || relid>= MAXOPEN){	//check relation id (relid) lies in the range
		return E_OUTOFBOUND;
	}
	if(AttrCacheTable::attrCache[relid]==NULL){ 
		//slot corresponding to the relid in the relation is free
		return E_NOTOPEN;	} 
	
	//Iterate over all the attributes in the attribute cache corresponding to the relation with Id relid{
	AttrCacheEntry *curr = AttrCacheTable::attrCache[relid];
	while(curr!= NULL) {
		//if the attrName/attrOffset of the attribute equals to the given attrName/attrOffset{
		if(curr-> entry. offset == attrOffset) {
			/*copy the previous index id (in meta informatiom) corresponding to 
			  the attribute in the attribute cache to the indId*/
			memcpy(indId, &(curr-> searchIndex), sizeof(IndexId));
			return SUCCESS;
		}
		curr = curr->next;
	}
	return E_ATTRNOTEXIST;
}

/******************* setSearchIndex Function *******************/
int AttrCacheTable::setSearchIndex(int relid, char attrName[ATTR_SIZE], IndexId *indId){
	if(relid< 0 || relid>= MAXOPEN){	//check relation id (relid) lies in the range
		return E_OUTOFBOUND;
	}
	if(AttrCacheTable::attrCache[relid]==NULL){ 
		//slot corresponding to the relid in the relation is free
		return E_NOTOPEN;	} 
	
	//Iterate over all the attributes in the attribute cache corresponding to the relation with Id relid{
	AttrCacheEntry *curr = AttrCacheTable::attrCache[relid];
	
	while(curr!= NULL) {
		if(strcmp( curr-> entry. attrName, attrName) == 0) {
			memcpy(&(curr-> searchIndex), indId, sizeof(IndexId));
			return SUCCESS;
		}
		curr = curr->next;
	}
	return E_ATTRNOTEXIST;
}

/******************* setSearchIndex Function *******************/
int AttrCacheTable::setSearchIndex(int relid, int attrOffset, IndexId *indId){
	if(relid< 0 || relid>= MAXOPEN){	//check relation id (relid) lies in the range
		return E_OUTOFBOUND;
	}
	if(AttrCacheTable::attrCache[relid]==NULL){ 
		//slot corresponding to the relid in the relation is free
		return E_NOTOPEN;	} 
	
	//Iterate over all the attributes in the attribute cache corresponding to the relation with Id relid{
	AttrCacheEntry * curr = AttrCacheTable::attrCache[relid];
	while(curr!= NULL) {
		//if the attrName/attrOffset of the attribute equals to the given attrName/attrOffset{
		if(curr-> entry. offset == attrOffset) {
			/*copy the indId to the previous index id (in meta informatiom)
			  corresponding to the attribute in the attribute cache*/
			memcpy(&(curr-> searchIndex), indId, sizeof(IndexId));
			return SUCCESS;
		}
		curr = curr->next;
	}
	return E_ATTRNOTEXIST;
}

/******************* getRelId Function *******************/
//NOTE: relId is an integer that denotes the entry in relation catalog
int OpenRelTable::getRelId(char relName[ATTR_SIZE]){
	//Iterate over the relation cache{
	for(int i =0;i<MAXOPEN;++i) {
		//if relation with relation name relName is opened in the relation cache

		if(!tableMetaInfo[i].free){	
			if(strcmp(tableMetaInfo[i].relName, relName)==0)
				//return corresponding relid
				return i; 
		}
		
	}
	return E_NOTOPEN;
}

OpenRelTable::OpenRelTable(){

	for(int i = 0;i < MAXOPEN;++i) {
	    RelCacheTable::relCache[i] = NULL;
	    AttrCacheTable::attrCache[i] = NULL;
		tableMetaInfo[i].free = true;
		strcpy(tableMetaInfo[i].relName, "");
	}
	strcpy(tableMetaInfo[0].relName, "relationCat");
	tableMetaInfo[0].free = false;
	strcpy(tableMetaInfo[1].relName, "attributeCat");
	tableMetaInfo[1].free = false;
	
	/************Loading the relation catalog entries***********/
    
    
    int block_num = 4; //Let block_num and slot_num be the block and slot numbers of the relation catalog
	RecBuffer buffer(block_num);	
	
	union Attribute relcat_rec[RELCAT_SIZE];
	buffer.getRecord(relcat_rec,0); //get the record corresponding to relation catalog into relcat_rec
	
	RelCacheEntry *relCacheEntry = new RelCacheEntry;
	RelCacheTable::recordToRelCacheEntry(relcat_rec, relCacheEntry);
	RelCacheTable::relCache[0] = relCacheEntry;
    
    recId recid;
	recid.slot = 0 ;
	recid.block = 4;
	RelCacheTable::relCache[0]->recid = recid;	
	
	union Attribute attrcat_rec[RELCAT_SIZE];
	buffer.getRecord(attrcat_rec,1); 
	
	RelCacheEntry *relCacheEntry2 = new RelCacheEntry;
	RelCacheTable::recordToRelCacheEntry(attrcat_rec, relCacheEntry2);
	RelCacheTable::relCache[1] = relCacheEntry2;
	
	recid.slot = 1 ;
	recid.block = 4;
	RelCacheTable::relCache[1]->recid = recid;	
		
	/************Loading the attribute catalog entries***********/
	
	block_num = 5;
	buffer = RecBuffer(block_num);
	int i;
	AttrCacheEntry *head = NULL,*tail = NULL, *curr;
    union Attribute attrcat_entry[ATTRCAT_SIZE];
    
	AttrCacheEntry *attrCacheEntry;
	for(i=0;i<6;++i){
		buffer.getRecord(attrcat_entry,i);
		
		attrCacheEntry = new AttrCacheEntry;
	    AttrCacheTable::recordToAttrCacheEntry(attrcat_entry, attrCacheEntry);
        curr = attrCacheEntry;		
		curr->recid.block = block_num;
		curr->recid.slot = i;
		if(head == NULL)
			head = curr;
		else
			tail->next = curr;		
		tail = curr;
	}
	
	AttrCacheTable::attrCache[0] = head;
	
	// Next 6 slots in this block corresponds to the attribute catalog entries of the relation catalog	
	tail = head = NULL;
	for(i=6;i<12;++i){
		buffer.getRecord(attrcat_entry,i);
		
		attrCacheEntry = new AttrCacheEntry;
	    AttrCacheTable::recordToAttrCacheEntry(attrcat_entry, attrCacheEntry);
        curr = attrCacheEntry;			
		curr-> recid.block = block_num;
		curr-> recid.slot = i;
		
		if(head == NULL)
			head = curr;
		else
			tail->next = curr;
			
		tail = curr;
	}
	
	AttrCacheTable::attrCache[1] = head;
        
}

OpenRelTable::~OpenRelTable(){

    int i;
    for(i=2; i<MAXOPEN; i++)
        if(tableMetaInfo[i].free == false)
            OpenRelTable::CloseRel(i);
               

    if(RelCacheTable::relCache[RELCAT_RELID] != NULL)
    {	if(RelCacheTable::relCache[ATTRCAT_RELID]->dirty == true)
	    {
	        union Attribute relCatEntry[RELCAT_SIZE]; 
	        RelCacheTable::relCacheEntryToRecord(relCatEntry,RelCacheTable::relCache[ATTRCAT_RELID]);
	        recId recid = RelCacheTable::relCache[ATTRCAT_RELID]->recid;
	        RecBuffer recBuffer(recid.block);
	        recBuffer.setRecord(relCatEntry, recid.slot);
	
	    }
	    if(RelCacheTable::relCache[RELCAT_RELID]->dirty == true)
	    {
	        union Attribute relCatEntry[RELCAT_SIZE]; 
	        RelCacheTable::relCacheEntryToRecord(relCatEntry,RelCacheTable::relCache[RELCAT_RELID]);
	        recId recid = RelCacheTable::relCache[RELCAT_RELID]->recid;
	        RecBuffer recBuffer(recid.block);
	        recBuffer.setRecord(relCatEntry, recid.slot);
	
	    }
    }
    
    AttrCacheEntry *current = AttrCacheTable::attrCache[ATTRCAT_RELID];
    while(current!=NULL)
    {   if(current->dirty == true)
        {
            union Attribute attrCatEntry[ATTRCAT_SIZE]; 
    	    AttrCacheTable::attrCacheEntryToRecord(attrCatEntry, current);    
            recId recid = current->recid;
	        RecBuffer recBuffer(recid.block);
	        recBuffer.setRecord(attrCatEntry, recid.slot);                
        }
        current = current->next;
    
    } 
    
    current = AttrCacheTable::attrCache[RELCAT_RELID];
    while(current!=NULL)
    {   if(current->dirty == true)
        {
            union Attribute attrCatEntry[ATTRCAT_SIZE]; 
    	    AttrCacheTable::attrCacheEntryToRecord(attrCatEntry, current);    
            recId recid = current->recid;
	        RecBuffer recBuffer(recid.block);
	        recBuffer.setRecord(attrCatEntry, recid.slot);                
        }
        current = current->next;
    
    } 
    
}

/******************* dispRelCatCache Function *******************/

void OpenRelTable::dispRelCatCache(){
	for(int i = 0;i < MAXOPEN; ++i) {
	
		if(tableMetaInfo[i].free)
			break;
		cout<<"_______________\n";
		cout << "Relation "<<i+1<<endl;
		cout<<"_______________\n";
		
		cout <<"\nRelation Catalog Entries:\n";
		cout<<"|"<<setw(16)<<"Relation Name|"<<setw(16)<<"Num Of Attrs|"<<setw(16)<<"Num Of Recs|"<<setw(16)<<"First Block|"<<setw(16)<<"Last Block|"<<setw(16)<<"Num Slots / Blk|\n";
		cout<<"+---------------+---------------+---------------+---------------+---------------+---------------+\n";
		cout <<"|" <<setw(15) << RelCacheTable::relCache[i]->entry.relName<<"|"
		<<setw(15)<<RelCacheTable::relCache[i]->entry.numAttrs<<"|"
		<<setw(15)<<RelCacheTable::relCache[i]->entry.numRecs<<"|"
		<<setw(15)<<RelCacheTable::relCache[i]->entry.firstBlk<<"|"
		<<setw(15)<<RelCacheTable::relCache[i]->entry.lastBlk<<"|"
		<<setw(15)<<RelCacheTable::relCache[i]->entry.numSlotsPerBlk<<"|\n";
		
		cout <<"\nMeta Data\n";
		
		cout <<"|"<<setw(16)<<"Free|"
		<<setw(16)<<"Dirty|"
		<<setw(16)<<"Rec Id|"
		<<setw(16)<<"Prev Rec Id|\n";
		cout<<"+---------------+---------------+---------------+---------------+\n";
		
		ostringstream str1;
		ostringstream str2;
		
		str1 << RelCacheTable::relCache[i]->recid.block; 
		str2 << RelCacheTable::relCache[i]->recid.slot; 
		
		ostringstream str3;
		ostringstream str4;
		
		str3 << RelCacheTable::relCache[i]->searchIndex.block; 
		str4 << RelCacheTable::relCache[i]->searchIndex.slot; 
		
		cout <<"|"<<setw(15)<<(tableMetaInfo[i].free?"True":"False")
			<<"|"
			<<setw(15)<<(RelCacheTable::relCache[i]->dirty?"True":"False")
			<<"|"
			<<setw(15)<< string("(") +str1.str()+string(", ")+str2.str()+string(")")<<"|"
			<<setw(15)<< string("(") +str3.str()+string(", ")+str4.str()+string(")")<<"|"<<endl;
			//"("<<str3.str()<<", "<<str4.str()<<")"<<"|\n";
		
		cout <<"\nAttribute Catalog Entries:\n";
		AttrCacheEntry * head = AttrCacheTable::attrCache[i];
		
		for(int j =0; ;++j) {
			
			if(head==NULL)
				break;
			cout<<"***************\n";	
			cout <<"Attribute "<<j + 1<<endl;
			cout<<"***************\n\n";
			
			cout <<"|"<<setw(16)<<"Relation Name|"<<setw(16)<<"Attr Name|"<<setw(16)<<"Attr Type|"<<setw(16)<<"Primary flag|"<<setw(16)<<"Root Block|"<<setw(16)<<"Offset|"<<endl;
			cout<<"+---------------+---------------+---------------+---------------+---------------+---------------+\n";
			cout <<"|"<<setw(15)<< head->entry.relName<<"|"
				<<setw(15)<<head->entry. attrName<<"|"
				<<setw(15)<<head-> entry.attrType<<"|"
				<<setw(15)<<head-> entry.primaryFlag<<"|"
				<<setw(15)<<head-> entry.rootBlock<<"|"
				<<setw(15)<<head-> entry.offset<<"|"<<endl;
			
			cout <<"\nMeta Data\n";
			cout <<"|"<<setw(16) <<"Dirty|"<<setw(16)<<"Rec Id|"<<setw(16)<<"Prev Ind Id|\n";
			cout<<"+---------------+---------------+---------------+\n";
			ostringstream str1;
			ostringstream str2;
		
			str1 << head-> recid.block; 
			str2 << head-> recid.slot; 
		
			ostringstream str3;
			ostringstream str4;
		
			str3 << head->searchIndex.block; 
			str4 << head->searchIndex.index;
		
			cout <<"|"<<setw(15)<< (head->dirty?"True":"False")<<"|"
				<<setw(16)<<string("(") + str1.str() + ", " + str2.str() +")|"
				<<setw(16)<<string("(") + str3.str() + ", " + str4.str() +")|"<<endl;
			
			head = head->next;
			cout <<endl;
		}
		
	}
}
