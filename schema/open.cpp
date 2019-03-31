/* Opening a relation */
#include "../cache/cache.h"

int openRel(char relname[16]){
    int open_rel=OpenRelTable::OpenRel(relname);
    return open_rel;
}