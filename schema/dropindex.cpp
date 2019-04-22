// dropping index (or b+ tree) on specefied attribute of a relation(if present).

#include "../cache/cache.h"
#include "../define/constants.h"

int dropindex(char relname[ATTR_SIZE],char attr[ATTR_SIZE]){
    int relid=OpenRelTable::getRelId(relname);

    if(relid==E_NOTOPEN) return E_NOTOPEN; // specified relation is not opened

    AttrCatEntry tmpattrcat;

    int flag=OpenRelTable::getAttrCatEntry(relid,attr,&tmpattrcat);
    if(flag!=SUCCESS){
        return flag;
    }

    int flag=bplus_destroy(tmpattrcat.root_block); /* ARGUMENT SHOULD BE ROOT BLOCK */

    tmpattrcat.root_block=-1; //deleting index on attribute

    flag=OpenRelTable::setAttrCatEntry(relid,attr, &tmpattrcat);
    return flag;
}