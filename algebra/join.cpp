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
    if(srcrelid1==E_NOTOPEN){
        return E_NOTOPEN; // src relation not opened
    }
    srcrelid2=OpenRelTable::getRelId(srcrel2);
    if(srcrelid2==E_NOTOPEN){
        return E_NOTOPEN; // src relation not opened
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
    if(flag!=SUCCESS){
        return flag; // srcrel1 attribute not found
    }
    flag=OpenRelTable::getAttrCatEntry(srcrelid2, attr2, &attrcat2);
    if(flag!=SUCCESS){
        return flag; // srcrel1 attribute not found
    }

    if(attrcat1.attr_type!=attrcat2.attr_type){
        return E_ATTRTYPEMISMATCH;// type mismatch of attributes.
    }


    //swap srcrels if needed.

    
    int iter;

    char tar_attrs[nAttrs1+nAttrs2-1][ATTR_SIZE];
    int tar_attrtype[nAttrs1+nAttrs2-1];
    for(iter=0;iter<nAttrs1;iter++){
        AttrCatEntry att1;
        flag=OpenRelTable::getAttrCatEntry(srcrelid1, iter, &att1);
        strncpy(tar_attrs[iter],att1.attr_name,ATTR_SIZE);
        tar_attrtype[iter]=att1.attr_type;
    }
    int attr_ind=nAttrs1;
    
    for(iter=0;iter<nAttrs2;iter++){
        AttrCatEntry att2;
        flag=OpenRelTable::getAttrCatEntry(srcrelid2, iter, &att2);

        if(!isEqualName(attr2,att2.attr_name)){
            tar_attrtype[attr_ind]=att2.attr_type;
            strncpy(tar_attrs[attr_ind++],att2.attr_name,ATTR_SIZE);
        }
    }

    flag=create(targetrel,nAttrs1+nAttrs2-1,tar_attrs,tar_attrtype);
    if(flag!=SUCCESS){
        return flag; // target rel may already exist or attrs more than limit or ...
    }

    targetrelid=openRel(targetrel);

    if(targetrelid==E_CACHEFULL){
        flag=deleterel(targetrel);
        return E_CACHEFULL;
    }

    union Attribute rec1[nAttrs1];

    while(ba_search(srcrelid1,rec1,"" ,,PROJ)==SUCCESS){
        union Attribute rec2[nAttrs2];
        union Attribute tar_record[nAttrs1+nAttrs2-1];
        while(ba_search(srcrelid2,rec2,attr2,rec1[attrcat1.offset],EQ)==SUCCESS){
            for(iter=0;iter<nAttrs1;iter++){
                tar_record[iter]=rec1[iter];
            }
            int attr_ind=nAttrs1;
            for(iter=0;iter<nAttrs2;iter++){
                if(iter!=attrcat2.offset){
                    tar_record[attr_ind++]=rec2[iter];
                }
            }

            flag=ba_insert(targetrelid,tar_record);
            if(flag!=SUCCUESS){
                closeRel(targetrel);
                deleterel(targetrel);
            }
            return flag;
        }   
    }
    closeRel(targetrel);
    return SUCCESS;
}