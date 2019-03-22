int bplus_destroy(relId relid, char AttrName[ATTR_SIZE]){
	int flag;
	
	//getting attribute catalog entry from open relation table
	AttrCatEntry *attrcatentry;
	flag = OpenRelTable::getAttrCatEntry(relid, AttrName, attrcatentry);
	if(flag == FAILURE)	//if there is problem in getting attribute catalog entry
		return FAILURE;
	
	//Checking already whether an index exists for the atrribute or not
	if(attrcatentry.root_block == -1){
	 	return SUCCESS; // since no index exists for the attribute
	 }
	 
	int root_block = attrcatentry.root_block;
	return bplus_deleteblock(root_block);
}

int bplus_deleteblock(int block_num){
	IndBuffer *ind_buffer;
	int block_type;
	struct HeadInfo header;
	
	ind_buffer = getIndBlock(block_num);
	if(ind_buffer == NULL)	//problem in accessing block of given block number
		return FAILURE;
		
	header = ind_buffer->getheader();
	block_type = header.block_type;
	
	if(block_type == INDINT){	// if block is internal node remove all children before removing it *IMPORTANT *
		int num_entries;
		num_entries = header.num_entries;
		
		int iter;
		struct InternalEntry *internal_entry;
		for(iter = 0;iter < num_entries; iter++){	// get the internal index block entries
			if(ind_buffer->getEntry(iter, internal_entry)) == FAILURE)
				return FAILURE //NOTE BUT SOME INDEX BLOCKS ARE ALREADY DELETED  
			bplus_deleteblock(internal_entry->lchild);
		}
		bplus_deleteblock(internal_entry->rchild);
		Buffer::deleteblock(block_num);
	}
	else if(block_type == INDLEAF){
		Buffer::deleteblock(block_num);
	}
	else{
		return FAILURE;	//if given block is not index block
	}
	
	return SUCCESS;
}
