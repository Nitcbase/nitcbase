/* algebra select call */
#include "../cache/cache.h"
#include "../define/constants.h"
#include <string.h>

int select(char srcrel[ATTR_SIZE],char targetrel[ATTR_SIZE], char attr[ATTR_SIZE], int op, union Attribute val){
    int srcrelid,targetrelid;

    srcrelid=OpenRelTable::getRelId(srcrel);
    if(srcrelid==FAILURE){
        return FAILURE; // src relation not opened
    }

    int flag;
    struct RelCatEntry srcrelcat;

    flag=OpenRelTable::getRelCatEntry(srcrelid,&srcrelcat);

    int nAttrs=srcrelcat.num_attr;

    char attr[nAttrs][ATTR_SIZE];
    struct AttrCatEntry srcattrcat;

    int attr_type[nAttrs];
    int iter;
    for(iter=0;iter<nAttrs;iter++){
        flag=OpenRelTable::getAttrCatEntry(srcrelid,iter,&srcattrcat);
        if(flag==FAILURE){
            return FAILURE; // attr with iter offset not found
        }
        attr_type[iter]=srcattrcat.attr_type;
        strncpy(attr[iter],srcattrcat.attr_name,ATTR_SIZE);
    }

    flag=OpenRelTable::getAttrCatEntry(srcrelid, attr, &srcattrcat);
    if(flag==FAILURE){
        return FAILURE; // target attribute name not found
    }

    flag=createrel(targetrel,nAttrs,attr,attr_type);

    if(flag==FAILURE){
        return FAILURE; // unable to create target relation
    }

    while(1){
        union Attribute rec[nAttrs];
        flag=ba_search(srcrelid,rec,attr,val,op);
        if(flag=SUCCESS){
            ba_insert(targetrelid,rec);
        }else{
            break;
        }
    }

    return SUCCESS;
}