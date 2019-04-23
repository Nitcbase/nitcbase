/* To create a new relation */
#include "../cache/cache.h"
#include "../define/id.h"
#include "../define/constants.h"
#include "../define/errors.h"
#include<string.h>

int createRel(char relname[],int nAttrs, char attrs[][ATTR_SIZE],int attrtype[]){
    int relcat_entry;
    struct RelCatEntry relcat;
    OpenRelTable::getRelCatEntry(0,&relcat);
    recId targetrelid;
    union Attribute attrval;
    strcpy(attrval.sval,relname);

    union Attribute relcatrec[6];//relname,#attrs,#records,firstblk,slotsperblk
    int flag=ba_search(0,relcatrec,"RelName", attrval, EQ);

    if(flag==SUCCESS){
        return E_RELEXIST; //Relation name already exists.
    }
    int iter;
    for(iter=0;iter<nAttrs;iter++){
        int iter2;
        // check if attrtype[iter] lies in 0-2 or not
        for(iter2=iter+1;iter2<nAttrs;iter2++){
            if(strcmp(attrs[iter],attrs[iter2])==0){
                return E_DUPLICATEATTR;  //distinct attributes having same name.
            }
        }
    }

    strcpy(relcatrec[0].sval,relname);
    relcatrec[1].ival=nAttrs;
    relcatrec[2].ival=0;
    relcatrec[3].ival=-1;
    relcatrec[4].ival=(2016/(16*nAttrs+1));

    int flag=ba_insert(0,relcatrec);

    if(flag!=SUCCESS){
        ba_delete(relname);
        return flag;
    }

    for(iter=0;iter<nAttrs;iter++){
        union Attribute attrcatrec[6];//relname,attrname,attrtype,primaryflag,rootblk,offset

        strcpy(attrcatrec[0].sval,relname);
        strcpy(attrcatrec[1].sval,attrs[iter]);
        attrcatrec[2].ival=attrtype[iter];
        attrcatrec[3].ival=-1;
        attrcatrec[4].ival=-1;
        attrcatrec[5].ival=iter;

        flag=ba_insert(1,attrcatrec);

        if(flag!=SUCCESS){
            ba_delete(relname);
            return flag;
        }

    }

    return SUCCESS;
	
}