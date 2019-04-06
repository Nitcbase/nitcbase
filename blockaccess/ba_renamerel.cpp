#include "../define/id.h"
int ba_renamerel(char OldRelName[ATTR_SIZE], char NewRelName[ATTR_SIZE]){
	recId relcat_recid = linear_search(0, "RelName", OldRelName, EQ); //relid 0 corresponds to relation catalog
	/* CHECK WHETHER NEW RELATION EXIST*/
	if(relcat_recid.block == -1 && relcat_recid.slot == -1){
		return FAILURE;
	}
	// rename relation name in relation catalog entry
	RecBuffer *rec_buffer = Buffer::getRecBuffer(relcat_recid.block);
	union Attribute relcat_rec[5]; // number of relation catalog is 5;
	rec_buffer->getRecord(relcat_rec, relcat_recid.slot); // getting the relation catalog entry corresponding to RelName
	strncpy(relcat_rec[0].strval, NewRelName, ATTR_SIZE); // setting new relation name in relation catalog
	rec_buffer->setRecord(relcat_rec, relcat_recid.slot);
	delete rec_buffer;
	
	int num_attrs = relcat_rec[1].ival; //number of attributes corresponding to the relation
	
	// rename relation name in attribute catalog entries
	recId attrcat_recid;
	union Attribute attrcat_rec[6]; // number of attribute catalog is 6
	
	for(int iter = 0; iter < num_attrs; iter++){
		int root_block;
		attrcat_recid = linear_search(1, "RelName", RelName, EQ); //relid 1 corresponds to attribute 
	
		/*if(attrcat_recid.block == -1 && attrcat_recid.slot == -1){
			return FAILURE;
		*/
		
		RecBuffer *rec_buffer = Buffer::getRecBuffer(attrcat_recid.block);
		rec_buffer->getRecord(attrcat_rec, attrcat_recid.slot); // getting the attribute catalog entry corresponding to RelName
		strncpy(attrcat_rec[0].strval, NewRelName, ATTR_SIZE); // setting new relation name in attribute catalog
		rec_buffer->setRecord(attrcat_rec, attrcat_recid.slot);
		delete rec_buffer;
	}
	
	return SUCCESS;
}
