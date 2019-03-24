#include "../define/id.h"
int ba_renamerel(char RelName[ATTR_SIZE], char OldAttrName[ATTR_SIZE], char NewAttrName[ATTR_SIZE]){
	recId relcat_recid = linear_search(0, "RelName", OldRelName, EQ); //relid 0 corresponds to relation catalog
	
	if(relcat_recid.block == -1 && relcat_recid.slot == -1){
		return FAILURE;
	}
	
	recId attrcat_recid;
	union Attribute attrcat_rec[6]; // number of attribute catalog is 6
	
	for(int iter = 0; iter < num_attrs; iter++){
		int root_block;
		attrcat_recid = linear_search(1, "RelName", RelName, EQ); //relid 1 corresponds to attribute 
	
		/*if(attrcat_recid.block == -1 && attrcat_recid.slot == -1){
			return FAILURE;
		*/
		if(strncmp(attrcat_rec[1], OldAttrName) == 0){
			RecBuffer *rec_buffer = Buffer::getRecBuffer(attrcat_recid.block);
			rec_buffer->getRecord(attrcat_rec, attrcat_recid.slot); //getting the attribute catalog entry corresponding to AttrName
			strncpy(attrcat_rec[1].strval, NewAttrName, ATTR_SIZE); //setting new attribute name in attribute catalog
			rec_buffer->setRecord(attrcat_rec, attrcat_recid.slot);
			Buffer::releaseBlock(attrcat_recid.block);
			break;
		}
	}
	
	return SUCCESS;
}
