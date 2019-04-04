/* searches the btree and returns recId of next hit */
#include "../define/id.h"
int compare(union Attribute attr1, union Attribute attr2, int AttrType){
	if(AttrType == STR){//String
		return (strcmp(attr1.strval, attr2.strval));
	}
	else if(AttrType == INT){//Interger
		if(attr1.ival < attr2.ival){
			return -1;
		}
		else if(attr1.ival == attr2.ival){
			return 0;
		}
		else{
			return 1;
		}
	}
	else if(AttrType == FLOAT){//Float
		if(attr1.fval < attr2.fval){
			return -1;
		}
		else if(attr1.fval == attr2.fval){
			return 0;
		}
		else{
			return 1;
		}
	}
}

struct recId bplus_search(relId relid, char AttrName[ATTR_SIZE], union Attribute AttrVal, int op){
	recId recid = {-1, -1};
	SearchIndexId sid;
	AttrCatEntry attrcatentry;
	int root_block, block_num;
	int index_num;
	int num_entries;	// in each index block
	int block_type;
	int attr_type;
	int iter;
	struct HeadInfo header;
	struct InternalEntry internal_entry;
	struct Index leaf_entry;
	IndBuffer *ind_buffer;
	int flag, cond; //FLAGS
	
	OpenRelTable::getAttrCatEntry(relid, AttrName, &attrcat_entry);
	root_block = attrcat_entry.root_block;
	attr_type = attrcat_entry.attr_type;
	
	sid = OpenRelTable::getSearchIndexId(relid, AttrName);
	if(sid.sblock == -1 && sid.sindex == -1){
		block_num = root_block;
		index_num = 0;
	}
	else{	// sid is set for leaf nodes only IMPORTANT
		block_num = sid.sblock;
		index_num = sid.sindex + 1;
	}
	
	if(block_num == root_block && index_num == 0){ //It is searching for attrval for the first time
		while(block_num != -1){
			ind_buffer = Buffer::getIndBuffer(block_num);
			header = ind_buffer->getheader();
			block_type = header.block_type;
			num_entries = header.num_entries;
			
			if(block_type == IND_LEAF){
				delete ind_buffer;
				break;
			}
			
			for(iter = 0; iter < num_entries, iter++){ 
				ind_buffer->getEntry(&internal_entry, iter);
				flag = compare(AttrVal, internal_entry.attrval, attr_type);
			
				cond = 0;
				switch(op){
					case LT:
						cond = 1;
						break;
					case LE:
						cond = 1;
						break;
					case EQ:
						if(flag <= 0){
							cond = 1;
						}
						break;
					case GT:
						if(flag < 0){
							cond = 1;	
						}
						break;
					case GE:
						if(flag <= 0){
							cond = 1;	
						}
						break;
				}
						
				if(cond == 1){
					delete ind_buffer;
					block_num = internal_entry.lchild;
					break;
				}
					
				if(iter == num_entries){
					delete ind_buffer;
					block_num = internal_entry.rchild; // go to right child if attrval is greater than
														// all attribute values of internal node
				}
			}
		}
	}
	
	//NOTE from here index block is leaf node WHY?
	
	ind_buffer = Buffer::getIndBuffer(block_num);
	header = ind_buffer->getheader();
	//block_type = header.block_type;  if needed verify that index block is leaf;
	num_entries = header.num_entries;
	
	if(index_num == num_entries){ // attrval equals to last index value
		delete ind_buffer;
		block_num = header.rblock; // next leaf child
		if(block_num == -1){
			sid.sblock = -1;
			sid.sindex = -1;
			OpenRelTable::setSearchIndexId(relid, AttrName, sid);
			return recid;
		}
			
		ind_buffer = Buffer::getIndBlock(block_num);
		header = ind_buffer->getheader();
		//block_type = header.block_type;
		num_entries = header.num_entries;
		index_num = 0;
	}
	
	for(iter = index_num; iter < num_entries; iter++){
		ind_buffer->getEntry(&leaf_entry, iter);
		flag = compare(AttrVal, leaf_entry.attrval, attr_type);
		
		cond = 0;
		switch(op){
			case LT:
				if(flag < 0){
						cond = 1;	
				}
				else{
					cond = -1; 
				}
				break;
			case LE:
				if(flag <= 0){
					cond = 1;	
				}
				else{
					cond = -1;
				}
				break;
			case EQ:
				if(flag == 0){
					cond = 1;
				}
				else if(flag > 0){
					cond = -1;
				}
				break;
			case GT:
				if(flag > 0){
					cond = 1;
				}
				break;
			case GE:
				if(flag >= 0){
					cond = 1;
				}
				break;
			}
						
			if(cond == 1){ // setting search index id
				recid.block = leaf_entry.block; 
				recid.slot = leaf_entry.slot;
				sid.sblock = block_num;
				sid.sindex = iter;
				OpenRelTable::setSearchIndexId(relid, AttrName, sid);
				break;
			}
			else if(cond == -1){ //attr val greats than index value then attr val cann't be found
				sid.sblock = -1;
				sid.sindex = -1;
				OpenRelTable::setSearchIndexId(relid, AttrName, sid);
				break;
			}
	}
	delete ind_buffer;
	
	return recid;
}
