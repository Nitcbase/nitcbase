/* closing an already open relation */
#include "../cache/cache.h"



int closeRel(int rel_id){ /* CHANGE ARGUMENT TO RELNAME */
    int close_rel=OpenRelTable::CloseRel(rel_id);
    return close_rel;
}