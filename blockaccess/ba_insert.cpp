recId getFreeSlot(int block_num){
	recId recid = {-1, -1};
	int prev_block_num, next_block_num;
	RecBuffer *rec_buffer;
	struct HeadInfo header;
	int num_slots;
	int iter;
	
	// finding free slot
	while(block_num != -1){
		rec_buffer = Buffer::getRecBuffer(block_num);
		header = rec_buffer->getheader();
		num_slots = header.num_slots;
		next_block_num = header.rblock; // next block
		//getting slotmap entries
		unsigned char slotmap[num_slots];
		rec_buffer->getSlotmap(slotmap);
		//searching for free slot in block (block_num)
		for(iter = 0; iter <  num_slots; iter++){
			if(slotmap[iter] == 0){
				break;
			}
		}
		delete rec_buffer;
		if(iter < num_slots){
			recid = {block_num, iter};
			return recid;
		}
		prev_block_num = block_num;
		block_num = next_block_num;
	}
	
	//no free slot is found in existing records if block_num = -1
	rec_buffer = Buffer::getFreeRecBuffer();
	if(rec_buffer == NULL){
		// no blocks are available in disk
		return recid; // no free slot can be found
	}
	block_num = rec_buffer->getBlockNum();
	//setting header values
	header = rec_buffer->getheader();
	header.lblock = prev_block_num;
	header.num_slots = num_slots;
	rec_buffer->setheader(header);
	//setting slotmap
	unsigned char slotmap[num_slots];
	rec_buffer->getSlotmap(slotmap);
	memset(slotmap, 0, sizeof(slotmap)); //all slots are free
	rec_buffer->setSlotmap(slotmap);
	delete rec_buffer;
	//recid
	recid = {block_num, 0};
	//setting prev_block_num rblock to new block
	rec_buffer = Buffer::getRecBuffer(prev_block_num);
	header = rec_buffer->getheader();
	header.rblock = block_num;
	rec_buffer->setheader(header);
	delete rec_buffer;
	return recid;
}

int ba_insert(relId relid, union Attribute *rec){
	RelCatEntry relcat_entry;
	AttrCatEnrty attrcat_entry;
	int first_block;
	int num_attrs;
	int num_slots;
	int root_block;
	recId recid;
	int iter;
	RecBuffer *rec_buffer;
	struct HeadInfo header;
	
	OpenRelTable::getRelCatEntry(relid, &relcat_entry);
	first_block = relcat_entry.first_blk;
	num_slots = relcat_entry.num_slots_blk;
	num_attrs = relcat_entry.num_attr;
	
	unsigned char slotmap[num_slots];
	//getting free slot
	recid = getFreeSlot(first_block);
	if(recid.block == -1 && recid.slot == -1){ //free slot can not be found
		return FAILURE;
	}
	rec_buffer = Buffer::getRecBuffer(recid.block);
	header = rec_buffer->getheader();
	//inserting record in the free slot given by getFreSlot
	rec_buffer->setRecord(rec, recid.slot);
	//since record is inserted number of entries is increased by 1
	header.num_entries = header.num_entries + 1; // increased number of entires in block
	rec_buffer->setheader();
	//settting corresponding slotmap entry as used
	rec_buffer->getSlotmap(slotmap);
	slotmap[recid.slot] = '1';
	rec_buffer->setSlotmap(slotmap);
	delete rec_buffer;
	//increasing number of entries in relation catalog entry
	relcat_entry.num_entries = relcat_entry.num_entries + 1;
	OpenRelTable::setRelCatEntry(relid, &relcat_entry);
	
	//inserting entries into index block if exits for attributes
	for(iter = 0; iter < num_attrs; iter++){
		OpenRelTable::getAttrCatEntry(relid, iter, &attrcat_entry);
		if(attrcat_entry.root_block != -1){ //if index presents for the attribute
			bplus_insert(relid, attrcat_entry.attr_name, rec[iter], recid); //inserting bplus tree
			/* WRITE FAILURE CONDITION */
		}	
	}
	
	return SUCCESS;
}
