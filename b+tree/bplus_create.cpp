int bplus_create(relId relid, char AttrName[ATTR_SIZE]){
	int flag;
	
	//getting attribute catalog entry from open relation table
	AttrCatEntry attrcatentry;
	OpenRelTable::getAttrCatEntry(relid, AttrName, &attrcatentry);
	
	//Checking already whether an index exists for the atrribute or not
	if(attrcatentry.root_block != -1){
	 	return SUCCESS; // already index exists for the attribute
	 }
	 
	RelCatEntry relcatentry;
	OpenRelTable::getRelCatEntry(relid, &relcatentry);
	
	int data_block, num_attrs;
	data_block = relcatentry.first_blk; //first record block for the relation
	num_attrs = relcatentry.num_attr; //num of attributes for the relation
	 
	// getting the free leaf index as root block
	IndLeaf *leaf_buffer;
	leaf_buffer = Buffer::getFreeIndLeaf();
	if(leaf_buffer == NULL)	//Problem in getting free leaf index block
		return FAILURE;
	 	
	int root_block;
	root_block = leaf_buffer->getBlockNum(); //getting root block number from buffer
	if(root_block == -1){
		//error
		return FAILURE;
	}
	attrcatentry.root_block = root_block;
	OpenRelTable::setAttrCatEntry(relid, AttrName, &attrcatentry);

	int attroffset, attrtype;
	attroffset = attrcatentry.offset;
	attrtype = attrcatentry.attr_type;
	
	union Attribute record[num_attrs];	//
	
	//inserting index entries for each record in bplus tree
	while(data_block != -1){
		RecBuffer *rec_buffer;
		rec_buffer = Buffer::getRecBlock(data_block); 
		
		if(rec_buffer == NULL){	//problem in getting record buffer corresponding to relation
			//call bplus_destroy
			bplus_destroy(relid, AttrName);
			return FAILURE;
		}
	
		struct HeadInfo header; //getting header/preablem of the record block
		header = rec_buffer->getheader();
		
		int num_slots;	//getting slotmap of the record block
		num_slots = header.num_slots;
		unsigned char slotmap[num_slots];
		rec_buffer->getslotmap(slotmap);
		
		int iter;
		for(iter = 0; iter < num_slots; iter++){
			if(slotmap[iter] == '0')
				continue;
			
			rec_buffer->getRecord(record, iter)	//getting iter th number record from data block
			union Attribute attrval;
			attrval = record[attroffset];	//getting attribute value corresponding to attribute name
			
			recId recid;
			recid.block = data_block;
			recid.slot = iter;
			//call bplus_insert
			bplus_insert(relid, AttrName, attrval, recid);
		}
		delete rec_buffer;
		data_block = header.rblock; //next data block for the relation
	}
			
	return SUCCESS;
}	
