// inserting a record into relation
#include "../cache/cache.h"
#include "../define/constants.h"
#include <string.h>

int insert(char relname[16], int nAttrs, char attr[][16]){
    int relid=OpenRelTable::getRelId(relname);

    if(relid==E_NOTOPEN){
        return E_NOTOPEN; //relation not open
    }

    struct RelCatEntry relcatentry;
    OpenRelTable::getRelCatEntry(relid,&relcatentry);

    if(relcatentry.num_attr!=nAttrs){
        return FAILURE; // no.of attributes did not match
    }

    union Attribute attrval[nAttrs];

    int iter;
    for(iter=0;iter<nAttrs;iter++){
        struct AttrCatEntry attrcatentry;
        OpenRelTable::getAttrCatEntry(relid,iter,&attrcatentry);
        int type=attrcatentry.attr_type;
        if(type==INT){  /* CHECK FOR TYPE MISMATCH HERE*/
            attrval[iter].ival=atoi(attr[iter]);
        }else if(type==FLOAT){
            //take accordingly to float
        }else if(type==STRING){
            strcpy(attrval[iter].sval,attr[iter]);
        }
    }

    int flag=ba_insert(relid,attrval);
    return flag;

}