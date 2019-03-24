recId ba_search(relId relid, char AttrName[ATTR_SIZE], union Attribute AttrVal, int op){
	recId recid = {-1, -1};
	AttrCatEntry *attrcat_entry;
	
	//get Attribute catalog entry corresponding to the relid, AttrName
	OpenRelTable::getAttrCatEntry(relid, AttrName, attrcat_entry);
	
	if(attrcat_entry->root_block == -1){ //No index for the attribute
		if(op == RST){
			recId prev_recid ={-1, -1};
			OpenRelTable::setPrevRecId(relid, prev_recid);
			return recid;
		}
		recid = linear_search(relid, AttrName, AttrVal, op);
	}
	else{ //index presents for the attribute
		if(op == RST){
			SearchIndexId sid = {-1, -1};
			OpenRelTable::setSearchIndexId(relid, AttrName, sid);
			return recid;
		}
		recid = bplus_search(relid, AttrName, AttrVal, op);
	}
	
	return recid;
}
