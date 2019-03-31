// delete a relation (specified by relname)
#include "../define/constants.h"
#include "../cache/cache.h"

int deleterel(char relname[ATTR_SIZE]){
    int relid=OpenRelTable::getRelId(relname);

    if(relid!=FAILURE){
        return FAILURE; // target relation is opened
    }

    int ba_delete_rel=ba_delete(relname);
    
    return ba_delete_rel;
}