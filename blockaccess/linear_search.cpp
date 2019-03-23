/* searches the records linearly and returns recId of next hit */

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

struct recId linear_search(relId relid, char AttrName[ATTR_SIZE], union Attribute AttrVal, int op){
	RelCatEntry *relcat_entry;
	AttrCatEntry *attrcat_entry;
	int first_block, block_num, next_block_num;
	int num_attrs;
	int attr_offset;
	int attr_type;
	int num_slots;
	int slot_num;
	recId recid = {-1, -1}, prev_recid;
	struct HeadInfo header;
	RecBuffer *rec_buffer;
	
	//getting neccessary entries from relation catalog
	OpenRelTable::getRelCatEntry(relid, relcat_entry);
	first_block = relcat_entry->first_blk;
	num_attrs = relcat_entry->num_attr;
	num_slots = relacat_entry->num_slots_blk;
	
	unsigned char slotmap[num_slots];
	union Attribute rec[num_attrs];
	
	//getting neccessary entries from attribute catalog
	OpenRelTable::getAttrCatEntry(relid, AttrName, attrcat_entry);
	attr_offset = attrcat_entry->offset;
	attr_type = attrcat_entry->attr_type;
	
	int flag,cond; //FLAGS
	
	prev_recid = OpenRelTabel::getPrevRecId(relid);
	if(prev_recid.block == -1 && prev_recid.slot == -1){
		block_num = first_block;
		slot_num = 0;
	}
	else{
		block_num = prev_recid.block;
		slot_num = prev_recid.slot + 1;
	}
	
	//searching every record of the relation (relid)
	while(block_num != -1){
		rec_buffer = Buffer::getRecBuffer(block_num);
		
		header = rec_buffer->getheader();
		next_block_num = header.rblock;
		rec_buffer->getSlotmap(slotmap);
		
		for(int iter = slot_num; iter < num_slots; iter++){
			if(slotmap[iter] == 0){
				continue;
			}
			
			rec_buffer->getRecord(rec, iter);
			flag = compare(AttrVal, rec[attr_offset], attr_type);
			cond = 0;
			
			switch(op){
				case NE:
					if(flag != 0){
						cond = 1;
					}
					break;
				case LT:
					if(flag < 0){
						cond = 1;
					}
					break;
				case LE:
					if(flag <= 0){
						cond = 1;
					}
					break;
				case EQ:
					if(flag == 0){
						cond = 1;
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
			
			if(cond == 1){
				recid = {block_num, iter};
				prev_recid = recid;
				OpenRelTable::setPrevRecId(relid, prev_recid);
				return recid;
			}
			block_num = next_block_num;
			slot_num = 0;
		}
		
	}
	
	return recid;
}
