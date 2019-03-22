int bplus_destroy(int block_num){
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
			bplus_destroy(internal_entry->lchild);
		}
		bplus_destroy(internal_entry->rchild);
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
