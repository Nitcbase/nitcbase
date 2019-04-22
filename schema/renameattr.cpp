// rename an attribute in a relation. (both specified as names)
#include "../define/constants.h"
#include "../cache/cache.h"

int renameattr(char relname[ATTR_SIZE], char OldAttrName[ATTR_SIZE], char NewAttrName){
    int relid=OpenRelTable::getRelId(relname);
    if(relid!=E_NOTOPEN){
        return E_OPEN; // relation is open
    }

    int flag= ba_renameattr(relname,OldAttrName,NewAttrName);

    return flag;
}