/* closing an already open relation */
#include "../cache/cache.h"



int closeRel(char rel[ATTR_SIZE]){ /* CHANGE ARGUMENT TO RELNAME */
    int rel_id=OpenRelTable::getRelId(rel);
    if(rel_id==E_NOTOPEN) return rel_id;
    
    int close_rel=OpenRelTable::CloseRel(rel_id);
    return close_rel;
}