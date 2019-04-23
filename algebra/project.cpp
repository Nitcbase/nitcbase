/* algebra select call */
#include "../cache/cache.h"
#include "../define/constants.h"
#include <string.h>

bool isEqualName(char* str1,char *str2){
    if(strcmp(str1,str2)==0){
        return TRUE;
    }else{
        return FALSE;
    }
}

int project(char srcrel[ATTR_SIZE],char targetrel[ATTR_SIZE],int tar_nAttrs, char tar_attrs[][ATTR_SIZE]){
    int srcrelid,targetrelid;
    int flag,iter;

    srcrelid=OpenRelTable::getRelId(srcrel);
    if(srcrelid==E_NOTOPEN){
        return E_NOTOPEN; // src relation not opened
    }

    struct RelCatEntry srcrelcat;
    flag=OpenRelTable::getRelCatEntry(srcrelid,&srcrelcat);
    int nAttrs=srcrelcat.num_attr;
    
    int attr_offset[tar_nAttrs];
    int attr_type[tar_nAttrs];

    for(iter=0;iter<tar_nAttrs;iter++){
        struct AttrCatEntry attrcat;
        flag=OpenRelTable::getAttrCatEntry(srcrelid,tar_attrs[iter],&attrcat);
        if(flag!=SUCCESS){
            return flag; // attribute not in src rel.
        }
        attr_offset[iter]=attrcat.offset;
        attr_type[iter]=attrcat.attr_type;
    }

    flag=createRel(targetrel,tar_nAttrs,tar_attrs,attr_type);
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
            union Attribute proj_rec[tar_nAttrs];

            for(iter=0;iter<tar_nAttrs;iter++){
                proj_rec[iter]=rec[attr_offset[iter]];
            }
            flag=ba_insert(targetrelid,proj_rec);
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