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
	int flag;
	int root_block;
	struct recId recid;
	recid.block = -1;
	recid.slot = -1; 
	
	//getting attribute catalog entry from open relation table
	AttrCatEntry *attrcatentry;
	flag = OpenRelTable::getAttrCatEntry(relid, AttrName, attrcatentry);
	if(flag == FAILURE)	//if there is problem in getting attribute catalog entry
		return FAILURE;
	
	//Checking already whether an index exists for the atrribute or not
	root_block = attrcatentry.root_block;
	if(root_block != -1){
	 	return FAILURE; // already index exists for the attribute
	 }
	 
	AttrType = attrcatentry.attr_type;
	SearchIndexId sid;
	sid = OpenRelTable::getSearchIndexId(relid, AttrName);
	 
	IndBuffer *ind_buffer;
	int block_type;
	struct HeadInfo header;
	int num_entries;
	int block_num, next_block_num;
	int iter;
	int cond;
	
	if(sid.sblock == -1 && sid.sindex == -1){ //if it is searching for the first for given attrval
	 	block_num = root_block;
	 	
	 	while(block_num != -1){ 
			ind_buffer = getIndBlock(block_num);
			if(ind_buffer == NULL)	//problem in accessing block of given block number
				return FAILURE;
			
			header = ind_buffer->getheader();
			block_type = header.block_type;
			num_entries = header.num_entries;
			
			if(block_type == INDINT){// Internal Index Block
				for(iter = 0; iter < num_entries; iter++){ //Comparing with internal entries
					struct InternalEntry *internal_entry;
					if(ind_buffer->getEntry(iter, internal_entry) == FAILURE){
						Buffer::releaseBlock(block_num);
						return FAILURE;
					}
					union Attribute InterAttrVal = internal_entry->attrval // getting internal attrval 
					flag = compare(AttrVal, InterAttrVal, AttrType);
					cond = 0;
					
					switch(op){
						case LT:
							next_block_num = internal_entry->lchild;
							cond = 1;
							break;
						case LE:
							next_block_num = internal_entry->lchild;
							cond = 1;
							break;
						case EQ:
							if(flag <= 0){
								next_block_num = internal_entry->lchild;
								cond = 1;
							}
							break;
						case GT:
							if(flag < 0){
								next_block_num = internal_entry->lchild;
								cond = 1;	
							}
							break;
						case GE:
							if(flag <= 0){
								next_block_num = internal_entry->lchild;
								cond = 1;	
							}
							break;
					}
						
					if(cond == 1){
						Buffer::releaseBlock(block_num);
						block_num = next_block_num;
						break;
					}
					
					if(iter == num_entries){
						Buffer::releaseBlock(block_num);
						block_num = internal_entry->lchild; // go to right child if attrval is greater than
															// all attribute values of internal node
					}				
				}
			}
			else if(block_type == INDLEAF){ //Leaf Index Block
				for(iter = 0; iter < num_entries; iter++){ //Comparing with internal entries
					struct Index *leaf_entry;
					if(ind_buffer->getEntry(iter, leaf_entry) == FAILURE){
						Buffer::releaseBlock(block_num);
						return FAILURE;
					}
					union Attribute LeafAttrVal = leaf_entry->attrval // getting leaf attrval 
					flag = compare(AttrVal, LeafAttrVal, AttrType);
					cond = 0;
					
					switch(op){
						case LT:
						if(flag < 0){
								recid.block = leaf_entry->block; 
								recid.slot = leaf_entry->slot;
								cond = 1;	
							}
							else{
								cond = -1; 
							}
							break;
						case LE:
							if(flag <= 0){
								recid.block = leaf_entry->block; 
								recid.slot = leaf_entry->slot;
								cond = 1;	
							}
							else{
								cond = -1;
							}
							break;
						case EQ:
							if(flag == 0){
								recid.block = leaf_entry->block; 
								recid.slot = leaf_entry->slot;
								cond = 1;
							}
							else if(flag > 0){
								cond = -1;
							}
							break;
						case GT:
							if(flag > 0){
								recid.block = leaf_entry->block; 
								recid.slot = leaf_entry->slot;
								cond = 1;
							}
							break;
						case GE:
							if(flag >= 0){
								recid.block = leaf_entry->block; 
								recid.slot = leaf_entry->slot;
								cond = 1;
							}
							break;
					}
						
					if(cond == 1){ // setting search index id
						sid.sblock = block_num;
						sid.sindex = iter;
						OpenRelTable::setSearchIndexId(relid, AttrName, sid);
						Buffer::releaseBlock(block_num);
						break;
					}
					else if(cond == -1){ //attr val greats than index value then attr val cann't be found
						sid.sblock = -1;
						sid.sindex = -1;
						OpenRelTable::setSearchIndexId(relid, AttrName, sid);
						Buffer::releaseBlock(block_num);
						return recid;
					}
				}
					
				if(iter == num_entries){
					sid.sblock = -1;
					sid.sindex = -1;
					OpenRelTable::setSearchIndexId(relid, AttrName, sid);
					Buffer::releaseBlock(block_num);
					return recid;
				}
				block_num = -1;
					
			}
			else{
				//Error 
			}
		}
	}
	else{// if attr value already searched sid.sblock != -1 && sid.sindex != -1
		block_num = sid.sblock;
		ind_buffer = getIndBlock(block_num);
		if(ind_buffer == NULL)	//problem in accessing block of given block number
			return FAILURE;
		
		header = ind_buffer->getheader();
		block_type = header.block_type;
		num_entries = header.num_entries;
		
		if(block_type != INDLEAF){
			//error
			return recid;
		}
		
		if(sid.sindex == (num_entries - 1)){ // attrval equals to last index value
			Buffer::releaseBlock(block_num);
			block_num = header.rblock; // next leaf child
			if(block_num == -1){
				sid.sblock = -1;
				sid.sindex = -1;
				OpenRelTable::setSearchIndexId(relid, AttrName, sid);
				return recid;
			}
			
			ind_buffer = getIndBlock(block_num);
			if(ind_buffer == NULL)	//problem in accessing block of given block number
				return FAILURE;
				
			header = ind_buffer->getheader();
			block_type = header.block_type;
			num_entries = header.num_entries;
			sid.sindex = -1;
			if(block_type != INDLEAF){
				//error
				return recid;
			}
		}
			
		for(iter = sid.sindex+1; iter < num_entries; iter++){
			struct Index *leaf_entry;
			if(ind_buffer->getEntry(iter, leaf_entry) == FAILURE){
				Buffer::releaseBlock(block_num);
				return FAILURE;
			}
			union Attribute LeafAttrVal = leaf_entry->attrval // getting leaf attrval 
			flag = compare(AttrVal, LeafAttrVal, AttrType);
			cond = 0;
						
			switch(op){
				case LT:
					if(flag < 0){
						recid.block = leaf_entry->block; 
						recid.slot = leaf_entry->slot;								
						cond = 1;	
					}
					else{
						cond = -1; 
					}
					break;					
				case LE:
					if(flag <= 0){
						recid.block = leaf_entry->block; 
						recid.slot = leaf_entry->slot;
						cond = 1;	
					}
					else{
						cond = -1;
					}
					break;
				case EQ:
					if(flag == 0){
						recid.block = leaf_entry->block; 
						recid.slot = leaf_entry->slot;
						cond = 1;
					}
					else if(flag > 0){
							cond = -1;
					}
					break;
				case GT:
					if(flag > 0){
						recid.block = leaf_entry->block; 
						recid.slot = leaf_entry->slot;
						cond = 1;
					}
					break;
				case GE:
					if(flag >= 0){
						recid.block = leaf_entry->block; 
						recid.slot = leaf_entry->slot;
						cond = 1;
					}
					break;
			}
						
			if(cond == 1){ // setting search index id
				sid.sblock = block_num;
				sid.sindex = iter;
				OpenRelTable::setSearchIndexId(relid, AttrName, sid);
				Buffer::releaseBlock(block_num);
				break;
			}
			else if(cond == -1){
				sid.sblock = -1;
				sid.sindex = -1;
				OpenRelTable::setSearchIndexId(relid, AttrName, sid);
				Buffer::releaseBlock(block_num);
				return recid;
			}
		}
	}
	return recid;
}
