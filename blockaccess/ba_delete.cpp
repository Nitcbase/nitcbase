#include "../define/id.h"
int ba_delete(char RelName[ATTR_SIZE]){
	recId relcat_recid = linear_search(0, "RelName", RelName, EQ); //relid 0 corresponds to relation catalog
	
	if(relcat_recid.block == -1 && relcat_recid.slot == -1){
		return FAILURE;
	}
	
	RecBuffer *rec_buffer = Buffer::getRecBuffer(relcat_recid.block);
	union Attribute relcat_rec[5]; // number of relation catalog is 5;
	rec_buffer->getRecord(relcat_rec, relcat_recid.slot); // getting the relation catalog entry corresponding to RelName
	delete rec_buffer;
	
	int block_num = relcat_rec[3].ival; // fist record block of the relation
	int num_attrs = relcat_rec[1].ival;
	int next_block_num;
	struct HeadInfo header;
	
	// Deleting record blocks of the relation
	while(block_num == -1){
		rec_buffer = Buffer::getRecBuffer(block_num);
		header = rec_buffer->getheader();
		next_block_num = header.rblock;
		delete rec_buffer;
		Buffer::deleteBlock(block_num);
		block_num = next_block_num;
	}
	
	recId attrcat_recid;
	union Attribute attrcat_rec[6]; // number of attribute catalog is 6
	int num_slots = 20; // for attribute catalog record block
	int num_entries;
	unsigned char attrcat_slotmap[num_slots];
	
	//deleting index blocks corresponding to the RelName and attribute catalog entries
	for(int iter = 0; iter < num_attrs; iter++){
		int root_block;
		attrcat_recid = linear_search(1, "RelName", RelName, EQ); //relid 1 corresponds to attribute 
	
		/*if(attrcat_recid.block == -1 && attrcat_recid.slot == -1){
			return FAILURE;
		*/
		
		RecBuffer *rec_buffer = Buffer::getRecBuffer(attrcat_recid.block);
		rec_buffer->getRecord(attrcat_rec, attrcat_recid.slot); // getting the attribute catalog entry corresponding to RelName
		
		//deleting attribute catalog entries
		header = rec_buffer->getheader();
		rec_buffer->getSlotmap(attrcat_slotmap);
		attrcat_slotmap[attrcat_recid.slot] = '0';	//making the slot free
		rec_buffer->setSlotmap(attrcat_slotmap);
		header.num_entries = header.num_entries - 1; //since one slot is made free
		rec_buffer->setheader(header);
		delete rec_buffer;
		
		// deleting index blocks of the attribute
		root_block = attrcat_rec[4]; //getting root block for the attribute
		if(root_block != -1){
			// call bplus destroy
			bplus_destroy(root_block);
		} 
	}
	
	num_slot = 24; //for relation catalog record block
	unsigned char relcat_slotmap[num_slots];
	//deleting relation catalog entry corresponding to the RelName
	rec_buffer = Buffer::getRecBuffer(relcat_recid.block);
	header = rec_buffer->getheader();
	relcat_slotmap[relcat_recid.slot] = '0';
	rec_buffer->setSlotmap(relcat_slotmap);
	header.num_entries = header.num_entries - 1;
	rec_buffer->setheader(header);
	delete rec_buffer;
	
	//Adjusting the Relation Catalog and Attribute Catalog 
	rec_buffer = Buffer::getRecBuffer(4); //block num 4 corresponds to relation catalog record block
	rec_buffer->getRecord(relcat_rec, 1); //slot num 1 corresponds to relation catalog record
	relcat_rec[2] = relcat_rec[2] - 1; //3rd entry corresponds to number of records & since relation is deleted
	rec_buffer->setRecord(relcat_rec, 1);
	rec_buffer->getRecord(relcat_rec, 2); //slot num 2 corresponds to attribute catalog record
	relcat_rec[2] = relcat_rec[2] - num_attrs; //all the attribute entries of the relation are deleted
	rec_buffer->setRecord(relcat_rec, 2);
	delete rec_buffer;
	
	return SUCCESS;
}

