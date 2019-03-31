// rename an attribute in a relation. (both specified as names)
#include "../define/constants.h"
#include "../cache/cache.h"

int renameattr(char relname[ATTR_SIZE], char OldAttrName[ATTR_SIZE], char NewAttrName){
    int relid=OpenRelTable::getRelId(relname);
    if(relid!=FAILURE){
        return FAILURE; // relation is opened
    }

    int ba_rename_attr= ba_renameattr(relname,OldAttrName,NewAttrName);

    return ba_rename_attr;
}