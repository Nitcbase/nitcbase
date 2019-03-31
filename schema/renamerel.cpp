// renaming an already present relation 
#include "../cache/cache.h"
#include "../define/constants.h"


int renamerel(char Oldrelname[ATTR_SIZE],char Newrelname[ATTR_SIZE]){
    int relid=OpenRelTable::getRelId(Oldrelname);
    if(relid!=FAILURE){
        return FAILURE; // relation is open
    }

    int ba_rename_relation= ba_renamerel(Oldrelname,Newrelname);

    return ba_rename_relation;
}