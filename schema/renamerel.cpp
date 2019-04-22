// renaming an already present relation 
#include "../cache/cache.h"
#include "../define/constants.h"


int renamerel(char Oldrelname[ATTR_SIZE],char Newrelname[ATTR_SIZE]){
    int relid=OpenRelTable::getRelId(Oldrelname);
    if(relid!=E_NOTOPEN){
        return E_OPEN; // relation is open
    }

    int flag= ba_renamerel(Oldrelname,Newrelname);

    return flag;
}