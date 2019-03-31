/* algebra natural join call */
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

int join(char srcrel1[ATTR_SIZE],char srcrel2[ATTR_SIZE],char targetrel[ATTR_SIZE], char attr1[ATTR_SIZE], char attr2[ATTR_SIZE]){
    int srcrelid1,srcrelid2,targetrelid;

    srcrelid1=OpenRelTable::getRelId(srcrel1);
    if(srcrelid1==FAILURE){
        return FAILURE; // src relation not opened
    }
    srcrelid2=OpenRelTable::getRelId(srcrel2);
    if(srcrelid2==FAILURE){
        return FAILURE; // src relation not opened
    }

    int flag;

    struct RelCatEntry srcrelcat1;
    flag=OpenRelTable::getRelCatEntry(srcrelid1,&srcrelcat1);
    int nAttrs1=srcrelcat1.num_attr;

    struct RelCatEntry srcrelcat2;
    flag=OpenRelTable::getRelCatEntry(srcrelid2,&srcrelcat2);
    int nAttrs2=srcrelcat2.num_attr;

    AttrCatEntry attrcat1,attrcat2;

    flag=OpenRelTable::getAttrCatEntry(srcrelid1, attr1, &attrcat1);
    if(flag==FAILURE){
        return FAILURE; // srcrel1 attribute not found
    }
    flag=OpenRelTable::getAttrCatEntry(srcrelid2, attr2, &attrcat2);
    if(flag==FAILURE){
        return FAILURE; // srcrel2 attribute not found
    }

    if(attrcat1.attr_type!=attrcat2.attr_type){
        return FAILURE;// type mismatch of attributes.
    }

    int iter,iter2;
    for(iter=0;iter<nAttrs1;iter++){
        for(iter2=0;iter2<nAttrs2;iter++){
            AttrCatEntry att1,att2;
            flag=OpenRelTable::getAttrCatEntry(srcrelid1, iter, &att1);
            flag=OpenRelTable::getAttrCatEntry(srcrelid2, iter2, &att2);
            if(isEqualName(att1.attr_name,att2.attr_name)){
                return FAILURE; //both rels having one attribute as same name
            }
        }
    }

    char tar_attrs[nAttrs1+nAttrs2-1][ATTR_SIZE];
    int tar_attrtype[nAttrs1+nAttrs2-1];
    for(iter=0;iter<nAttrs1;iter++){
        AttrCatEntry att1;
        flag=OpenRelTable::getAttrCatEntry(srcrelid1, iter, &att1);
        strncpy(tar_attrs[iter],att1.attr_name,ATTR_SIZE);
    }
    int attr_ind=nAttrs1;
    for(iter=0;iter<nAttrs2;iter++){
        AttrCatEntry att2;
        flag=OpenRelTable::getAttrCatEntry(srcrelid2, iter, &att2);

        if(!isEqualName(attr2,att2.attr_name))
        strncpy(tar_attrs[attr_ind++],att2.attr_name,ATTR_SIZE);
    }

    flag=create(targetrel,nAttrs1+nAttrs2-1,tar_attrs,tar_attrtype);
    if(flag==FAILURE){
        return FAILURE; // target rel may already exist or attrs more than limit or ...
    }

    targetrelid=OpenRelTable::OpenRel(targetrel);

    for(iter=0;iter<nAttrs1;iter++){
        //insert elems in to target rel accordingly..
    }
}