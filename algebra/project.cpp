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
    if(srcrelid==FAILURE){
        return FAILURE; // src relation not opened
    }

    struct RelCatEntry srcrelcat;
    flag=OpenRelTable::getRelCatEntry(srcrelid,&srcrelcat);
    int nAttrs=srcrelcat.num_attr;

    int iter,iter2;
    for(iter=0;iter<tar_nAttrs;iter++){
        for(iter2=iter+1;iter2<tar_nAttrs;iter++){
            if(isEqualName(tar_attrs[iter],tar_attrs[iter2])){
                return FAILURE; //two attrs. having same name in list /* NOT NEEDED -- CREATE TAKES CARE */
            }
        }
    }

    
    int attr_offset[tar_nAttrs];
    int attr_type[tar_nAttrs];

    for(iter=0;iter<tar_nAttrs;iter++){
        struct AttrCatEntry attrcat;
        flag=OpenRelTable::getAttrCatEntry(srcrelid,tar_attrs[iter],&attrcat);
        if(flag==FAILURE){
            return FAILURE; // attribute not in src rel.
        }
        attr_offset[iter]=attrcat.offset;
        attr_type[iter]=attrcat.attr_type;
    }

    flag=createrel(targetrel,tar_nAttrs,tar_attrs,attr_type);
    if(flag==FAILURE){
        return FAILURE; // unable to create target relation
    }
    targetrelid=OpenRelTable::OpenRel(targetrel);

    while(1){
        union Attribute rec[nAttrs];
        flag=ba_search(srcrelid,rec,attr,val,op);
        if(flag=SUCCESS){
            union Attribute proj_rec[tar_nAttrs];

            for(iter=0;iter<tar_nAttrs;iter++){
                proj_rec[iter]=rec[attr_offset[iter]];
            }
            ba_insert(targetrelid,proj_rec);
        }else{
            break;
        }
    }

    int flag;
}