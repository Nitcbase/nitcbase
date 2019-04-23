/* algebra select call */
#include "../cache/cache.h"
#include "../define/constants.h"
#include <string.h>

// LAST ARGUMENT SHOULD BE char[].
int select(char srcrel[ATTR_SIZE],char targetrel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strval[ATTR_SIZE]){
    int srcrelid,targetrelid;

    srcrelid=OpenRelTable::getRelId(srcrel);
    if(srcrelid==E_NOTOPEN){
        return E_NOTOPEN; // src relation not opened
    }

    int flag;
    struct RelCatEntry srcrelcat;

    flag=OpenRelTable::getRelCatEntry(srcrelid,&srcrelcat);
    int nAttrs=srcrelcat.num_attr;

    union Attribute val;
    struct AttrCatEntry tar_attr;
    flag=OpenRelTable::getAttrCatEntry(srcrelid, attr, &tar_attr);
    if(flag!=SUCCESS){
        return flag; // target attribute name not found
    }
    if(tar_attr.attr_type==INT){  /* CHECK FOR TYPE MISMATCH HERE*/
        attrval[iter].ival=atoi(attr[iter]);
    }else if(tar_attr.attr_type==FLOAT){
        //take accordingly to float
    }else if(tar_attr.attr_type==STRING){
        strcpy(attrval[iter].sval,attr[iter]);
    }

    char attrs[nAttrs][ATTR_SIZE];
    struct AttrCatEntry srcattrcat;
    int attr_type[nAttrs];
    int iter;
    for(iter=0;iter<nAttrs;iter++){
        flag=OpenRelTable::getAttrCatEntry(srcrelid,iter,&srcattrcat);
        if(flag!=SUCCESS){
            return flag; // target attribute name not found
        }
        attr_type[iter]=srcattrcat.attr_type;
        strncpy(attrs[iter],srcattrcat.attr_name,ATTR_SIZE);
    }

    

    flag=createRel(targetrel,nAttrs,attrs,attr_type);
    if(flag!=SUCCESS){
        return flag; // unable to create target relation
    }
    targetrelid=openRel(targetrel);
    if(targetrelid==E_CACHEFULL){
        flag=deleterel(targetrel);
        return E_CACHEFULL;
    }

    while(1){
        union Attribute rec[nAttrs];
        flag=ba_search(srcrelid,rec,attr,val,op);
        if(flag=SUCCESS){
            ba_insert(targetrelid,rec);
            if(flag!=SUCCESS){
                closeRel(targetrel);
                deleterel(targetrel);
                return flag; //unable to insert into target relation.
            }
        }else{
            break;
        }
    }
    closeRel(targetrel);
    return SUCCESS;
}