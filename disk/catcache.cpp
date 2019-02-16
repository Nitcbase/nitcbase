#include "catcache.h"
#include "buffer.h"
#include "blockBuffer.h"
#include <cstring>
#include <cstdlib>

relId OpenRelTable::getFreeOpenRelTableEntry(){
	for(relId rel_id=0; rel_id<8; rel_id++){
		if(RelCache[rel_id].free){
			return rel_id;
		}
	}
	return FAILURE;
}

relId OpenRelTable::OpenRel(char rel_name[16]){
	int rel_id;
	RecBuffer *buffer;
	int block_num= 4,slot_num;	// block num= 4 corresponds to first block of attribute catalog
	struct HeadInfo header;
	int num_attrs;
	
	rel_id= getFreeOpenRelTableEntry();
	if(rel_id== -1){
		return FAILURE;
	}
	
	while(block_num!=-1){	//getting relation catalog entries into Open Relation Table
		buffer= Buffer::getRecBlock(block_num);
		if(buffer= NULL){
			return FAILURE;
		}
		
		header= buffer->getheader();
		union Attribute rec[5];	//5 attributes per record in relation catalog
		unsigned char slotmap[header.num_slots];	//24 slots for a relation catalog 
		buffer->getSlotmap(slotmap);
		
		for(slot_num= 0;slot_num< header.num_slots;slot_num++){		
			if(slotmap[slot_num]!='1'){
				continue;
			}
			buffer->getRecord(rec,slot_num);
			if(strcmp(rel_name,rec[0].strval)==0){	//comparing relation name with given rel_name
				break;
			}
		}
		
		delete buffer;	//releasing buffer block
		//buffer.releaseBlock(block_num);
		if(slot_num< header.num_slots){	//updatinng cache entries with relation catalog entries
			strcpy(RelCache[rel_id].relcat_entry.rel_name,rel_name);
			RelCache[rel_id].relcat_entry.num_attr= rec[1].ival;
			RelCache[rel_id].relcat_entry.num_rec= rec[2].ival;
			RelCache[rel_id].relcat_entry.first_blk= rec[3].ival;
			RelCache[rel_id].relcat_entry.num_slots_blk= rec[4].ival;
			RelCache[rel_id].rec_id.block= block_num;
			RelCache[rel_id].rec_id.slot= slot_num;
			RelCache[rel_id].attr_list_head= NULL;
			break;
		}
		block_num= header.rblock;
	}
	
	if(block_num== -1){
		return FAILURE;
	}
	num_attrs= RelCache[rel_id].relcat_entry.num_attr;
	block_num= 5;	//first block of attribute catalog
	
	while(num_attrs> 0){
		buffer= Buffer::getRecBlock(block_num);
		if(buffer= NULL){
			return FAILURE;
		}
		
		header= buffer->getheader();
		union Attribute rec[6];	//6 attributes per record in attribute catalog
		unsigned char slotmap[header.num_slots];	//20 slots for an attribute catalog
		buffer->getSlotmap(slotmap);
		
		for(slot_num= 0;slot_num< header.num_slots;slot_num++){		
			if(slotmap[slot_num]!='1'){
				continue;
			}
			buffer->getRecord(rec,slot_num);
			if(strcmp(rel_name,rec[0].strval)==0){	//comparing relation name with given rel_name and 
													//updating attribute cache entries in Open Relation Table	
				num_attrs--;
				AttributeCache *temp= (AttributeCache*)malloc(sizeof(AttributeCache));
				strcpy(temp->attrcat_entry.rel_name,rel_name);
				strcpy(temp->attrcat_entry.attr_name,rec[1].strval);
				temp->attrcat_entry.attr_type= rec[2].ival;
				temp->attrcat_entry.primary_flag= rec[3].ival;
				temp->attrcat_entry.root_block= rec[4].ival;
				temp->attrcat_entry.offset= rec[5].ival;	
				temp->sid.sblock= -1;
				temp->sid.sindex= -1;
				temp->free= true;
				temp->dirty= false;
				temp->rec_id.block= block_num;
				temp->rec_id.slot= slot_num;
				temp->next= RelCache[rel_id].attr_list_head;
				RelCache[rel_id].attr_list_head= temp;
			}
			if(num_attrs== 0){
					break;
			}
		}
		
		delete buffer;	//releasing buffer block
		//buffer.releaseBlock(block_num);
		block_num= header.rblock;
	}
	RelCache[rel_id].free= false;	
	return rel_id;
}

int OpenRelTable::CloseRel(relId rel_id){
	if(rel_id< 0 || rel_id>= 8 || RelCache[rel_id].free){	//check for validity of relation id
		return FAILURE;
	}
	RecBuffer *buffer;
	AttributeCache *head,*temp;
	head= RelCache[rel_id].attr_list_head;
	int block_num,slot_num;
	
	while(head){	//free the attribute catalog entries from Open Relation Table
		temp= head->next;
		if(head->dirty){	//commit attribute catalog entries if they are modified(dirty)
			block_num= head->rec_id.block;
			slot_num= head->rec_id.slot;
			union Attribute rec[6];		
			buffer= Buffer::getRecBlock(block_num);
	
			strcpy(rec[0].strval,head->attrcat_entry.rel_name);
			strcpy(rec[1].strval,head->attrcat_entry.attr_name);
			rec[2].ival= head->attrcat_entry.attr_type;
			rec[3].ival= head->attrcat_entry.primary_flag;
			rec[4].ival= head->attrcat_entry.root_block;
			rec[5].ival= head->attrcat_entry.offset;
			
			buffer->setRecord(rec, slot_num);
			delete buffer;
		}
		free(head);
		head= temp;
	}
	
	RelCache[rel_id].attr_list_head= NULL;
	
	if(RelCache[rel_id].dirty){	//commit the relation catalog entry if it is modified(dirty)
		block_num= RelCache[rel_id].rec_id.block;
		slot_num= RelCache[rel_id].rec_id.slot;
		union Attribute rec[5];
		buffer= Buffer::getRecBlock(block_num);
		
		strcpy(rec[0].strval, RelCache[rel_id].relcat_entry.rel_name);
		rec[1].ival= RelCache[rel_id].relcat_entry.num_attr;
		rec[2].ival= RelCache[rel_id].relcat_entry.num_rec;
		rec[3].ival= RelCache[rel_id].relcat_entry.first_blk;
		rec[4].ival= RelCache[rel_id].relcat_entry.num_slots_blk;
		
		buffer->setRecord(rec, slot_num);
		delete buffer;	
	}
	
	RelCache[rel_id].free= true;
	RelCache[rel_id].dirty= false;
	return SUCCESS;
}

int OpenRelTable::getRelCatEntry(relId rel_id, RelCatEntry relcat_rec){
	if(rel_id< 0 || rel_id>= 8 || RelCache[rel_id].free){	//check for validity of relation id
		return FAILURE;
	}
	relcat_rec= RelCache[rel_id].relcat_entry;
	return SUCCESS;
}	
	
int OpenRelTable::setRelCatEntry(relId rel_id, RelCatEntry relcat_rec){
	if(rel_id< 0 || rel_id>= 8 || RelCache[rel_id].free){	//check for validity of relation id
		return FAILURE;
	}
	RelCache[rel_id].relcat_entry= relcat_rec;
	RelCache[rel_id].dirty= true;
	return SUCCESS;
}

int OpenRelTable::getAttrCatEntry(relId rel_id, char attr_name[16], AttrCatEntry attrcat_rec){
	if(rel_id< 0 || rel_id>= 8 || RelCache[rel_id].free){	//check for validity of relation id
		return FAILURE;
	}
	AttributeCache *head;
	head= RelCache[rel_id].attr_list_head;
	while(head){
		if(strcmp(head->attrcat_entry.attr_name,attr_name)== 0){
			attrcat_rec= head->attrcat_entry;
			return SUCCESS;
		}
		head= head->next;
	}
	return FAILURE;
}

int OpenRelTable::getAttrCatEntry(relId rel_id, int attr_offset, AttrCatEntry attrcat_rec){
	if(rel_id< 0 || rel_id>= 8 || RelCache[rel_id].free){	//check for validity of relation id
		return FAILURE;
	}
	AttributeCache *head;
	head= RelCache[rel_id].attr_list_head;
	while(head){
		if(head->attrcat_entry.offset== attr_offset){
			attrcat_rec= head->attrcat_entry;
			return SUCCESS;
		}
		head= head->next;
	}
	return FAILURE;
}

int OpenRelTable::setAttrCatEntry(relId rel_id, char attr_name[16], AttrCatEntry attrcat_rec){
	if(rel_id< 0 || rel_id>= 8 || RelCache[rel_id].free){	//check for validity of relation id
		return FAILURE;
	}
	AttributeCache *head;
	head= RelCache[rel_id].attr_list_head;
	while(head){
		if(strcmp(head->attrcat_entry.attr_name,attr_name)== 0){
			head->attrcat_entry= attrcat_rec;
			head->dirty= true;
			return SUCCESS;
		}
		head= head->next;
	}
	return FAILURE;
}

int OpenRelTable::setAttrCatEntry(relId rel_id, int attr_offset, AttrCatEntry attrcat_rec){
	if(rel_id< 0 || rel_id>= 8 || RelCache[rel_id].free){	//check for validity of relation id
		return FAILURE;
	}
	AttributeCache *head;
	head= RelCache[rel_id].attr_list_head;
	while(head){
		if(head->attrcat_entry.offset== attr_offset){
			head->attrcat_entry= attrcat_rec;
			head->dirty= true;
			return 	SUCCESS;
		}
		head= head->next;
	}
	return FAILURE;
}

int OpenRelTable::getSearchIndexId(relId rel_id, char attr_name[16], SearchIndexId sid){
	if(rel_id< 0 || rel_id>= 8 || RelCache[rel_id].free){	//check for validity of relation id
		return FAILURE;
	}
	AttributeCache *head;
	head= RelCache[rel_id].attr_list_head;
	while(head){
		if(strcmp(head->attrcat_entry.attr_name,attr_name)== 0){
			sid= head->sid;
			return SUCCESS;
		}
		head= head->next;
	}
	return FAILURE;	
}

int OpenRelTable::getSearchIndexId(relId rel_id, int attr_offset, SearchIndexId sid){
	if(rel_id< 0 || rel_id>= 8 || RelCache[rel_id].free){	//check for validity of relation id
		return FAILURE;
	}
	AttributeCache *head;
	head= RelCache[rel_id].attr_list_head;
	while(head){
		if(head->attrcat_entry.offset== attr_offset){
			sid= head->sid;
			return SUCCESS;
		}
		head= head->next;
	}
	return FAILURE;
}

int OpenRelTable::setSearchIndexId(relId rel_id, char attr_name[16], SearchIndexId sid){
	if(rel_id< 0 || rel_id>= 8 || RelCache[rel_id].free){	//check for validity of relation id
		return FAILURE;
	}
	AttributeCache *head;
	head= RelCache[rel_id].attr_list_head;
	while(head){
		if(strcmp(head->attrcat_entry.attr_name,attr_name)== 0){
			head->sid= sid;
			return SUCCESS;
		}
		head= head->next;
	}
	return FAILURE;
}

int OpenRelTable::setSearchIndexId(relId rel_id, int attr_offset, SearchIndexId sid){
	if(rel_id< 0 || rel_id>= 8 || RelCache[rel_id].free){	//check for validity of relation id
		return FAILURE;
	}
	AttributeCache *head;
	head= RelCache[rel_id].attr_list_head;
	while(head){
		if(head->attrcat_entry.offset== attr_offset){
			head->sid= sid;
			return SUCCESS;
		}
		head= head->next;
	}
	return FAILURE;
}

relId OpenRelTable::getRelId(char rel_name[16]){
	for(relId rel_id=0; rel_id<8; rel_id++){
		if(RelCache[rel_id].free){
			continue;
		}
		if(strcmp(RelCache[rel_id].relcat_entry.rel_name,rel_name)== 0){
			return rel_id;
		}	
	}
	return FAILURE;
}
