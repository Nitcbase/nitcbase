/* closing an already open relation */
#include "../cache/cache.h"



int closeRel(int rel_id){
    int close_rel=OpenRelTable::CloseRel(rel_id);
    return close_rel;
}