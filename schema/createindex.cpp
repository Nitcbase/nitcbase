// creating index (or b+ tree) on specefied attribute of a relation.

#include "../cache/cache.h"
#include "../define/constants.h"

int createindex(char relname[ATTR_SIZE],char attr[ATTR_SIZE]){
    int relid=OpenRelTable::getRelId(relname);

    if(relid==FAILURE) return FAILURE; // specified relation not opened

    int flag=bplus_create(relid,attr);

    return flag;
}