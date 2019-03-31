// dropping index (or b+ tree) on specefied attribute of a relation(if present).

#include "../cache/cache.h"
#include "../define/constants.h"

int dropindex(char relname[ATTR_SIZE],char attr[ATTR_SIZE]){
    int relid=OpenRelTable::getRelId(relname);

    if(relid!=FAILURE) return FAILURE; // specified relation is opened

    int flag=bplus_destroy(relid,attr);

    return flag;
}