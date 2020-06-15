//Todo :  how to check if conversion fails
//In join should u swap the relations depending on number of attributes



/*#include "schema.cpp"
#include <bits/stdc++.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

using namespace std;*/

int select(char srcrel[ATTR_SIZE],char targetrel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strval[ATTR_SIZE])
{

    // get the srcrel's open relation id(let it be srcrelid), using getRelId() method of cache layer
    // if srcrel is not open in open relation table, return E_RELNOTOPEN

    int src_relid=getRelId(srcrel);
    if(src_relid==FAILURE)
        return E_RELNOTOPEN;
    //cout<<"Relopen"<<endl;
    union Attribute attrcat_entry[6];

    // get the attribute catalog entry for attr, using getAttrcatEntry() method of cache layer(let it be attrcatentry).
    // if getAttrcatEntry() call fails return E_ATTRNOTEXIST
    int flag= getAttrCatEntry(src_relid,attr,attrcat_entry);
    if(flag!=SUCCESS)
        return flag;
    
    //cout<<"Getattrcat successful"<<endl;
    //convert strval into union Attribute (let it be val) as shown in the following code:
     //if convert fails, return E_ATTRTYPEMISMATCH
    int type=attrcat_entry[2].ival;
    union Attribute val;
    //cout<<type<<endl;
    if(type==INT) 
        //The input contains a string representation of the integer attribute value.
        val.ival=stoi(strval);
        //if conversion fails(i.e string can not be converted to integer) return E_ATTRTYPEMISMATCH. 
    else if(type==FLOAT)
    {
        //do accordingly to float
        val.fval=stof(strval);

    }
    else if(type==STRING)
    {
        //No type conversion needed here
        strcpy(val.sval,strval);
    }
    else
        return E_NATTRMISMATCH;
   //cout<<"Here!!"<<endl;

    //Next task is to create the destination relation.
    //Prepare arguments for createRel() in the following way:
    //get RelcatEntry of srcrel from cache using getRelCatEntry() method of cache layer.
    //get the no. of attributes present in src relation, from RelcatEntry.(let it be nAttrs)
    union Attribute relcat_entry[6];
    getRelCatEntry(src_relid,relcat_entry);
    int nAttrs=relcat_entry[1].ival;

    // let attr_name[nAttrs][ATTR_SIZE] be a 2D array of type char(attribute names of rel).
    // let attr_types[nAttrs] be an array of type int

    char attr_name[nAttrs][ATTR_SIZE];
    int attr_types[nAttrs];

    /*iterate through 0 to nAttrs-1 :
        get the i'th attribute's AttrCatEntry (using getAttrcatEntry() of cache layer )
        fill attr_name, attr_types of corresponding attributes using Attribute catalog found.
    */
    recId prev_recid={-1,-1};
    recId recid;
    for(int i=0;i<nAttrs;i++)
    {
        union Attribute record[6];
        union Attribute a;
        strcpy(a.sval,OpenRelTable[src_relid]);
        recid=linear_search(ATTRCAT_RELID,"RelName",a,EQ,&prev_recid);
        if(!((recid.block==-1)&&(recid.slot==-1)))
        {
            getRecord(record,recid.block,recid.slot);
            strcpy(attr_name[i],record[1].sval);
            attr_types[i]=record[2].ival;  
        }
        if((recid.block==-1)&&(recid.slot==-1))
        	return E_ATTRNOTEXIST;
        

    }

    /* let retval= createRel(targetrel,no_of_attrs_srcrel,attr_name,attr_type)
       where createrel is a function in schema layer
       if create fails return retval */
    int retval=createRel(targetrel,nAttrs,attr_name,attr_types);
    if(retval!=SUCCESS)
        return retval;

    //int targetrelid = openRel(targetrel) 
    //where openRel is a function in schema layer
    /* if open fails
        delete target relation by calling deleteRel(targetrel) of schema layer
        return retval
    */
    int target_relid=openRel(targetrel);
    if(target_relid==-1)
    {	
    	retval=ba_delete(targetrel);
        return -1;
    }

    
    //Note: Before calling the search function, reset the search to start from the first hit
    // by calling ba_search of block access layer with op=RST.
    /*
    while (1):
        var: union Attribute record[no_of_attrs_srcrel];
        if ba_search(srcrelid,record,attr,val,op) returns SUCCESS:
            retval = ba_insert(targetrelid,record);
            if(insert fails):
                close the targetrel(by calling closeRel(targetrel) method of schema layer)
                delete targetrel (by calling deleteRel(targetrel) of schema layer)
                return retval

        else: break
    */

    prev_recid={-1,-1};
    while(1)
    {

        union Attribute record[nAttrs];
        retval=ba_search(src_relid,record,attr,val,op,&prev_recid);
        if(retval==SUCCESS)
        {
            int ret= ba_insert(target_relid,record);
            //cout<<"yaaay"<<endl;
            if(ret!=SUCCESS)
            {
                closeRel(target_relid);
                int flag= ba_delete(targetrel);
                return ret;

            }
        }
        else
            break;
    }

    //Close the target relation using closeRel() method of schema layer
    // return SUCCESS;

    closeRel(target_relid);
    //cout<<"reached here"<<endl;
    return SUCCESS;
    
    }



int project(char srcrel[ATTR_SIZE],char targetrel[ATTR_SIZE],int tar_nAttrs, char tar_attrs[][ATTR_SIZE])
{
    //cout<<"reached project"<<endl;
    int srcrelid,targetrelid;
    int flag,iter;

    srcrelid=getRelId(srcrel);
    //cout<<"****************************************"<<endl;
    //cout<<"project relid is"<<srcrelid<<endl;
    if(srcrelid==E_NOTOPEN)
    {
        return E_NOTOPEN; // src relation not opened
    }

    union Attribute srcrelcat[6];
    flag=getRelCatEntry(srcrelid,srcrelcat);
    int nAttrs=srcrelcat[1].ival;
    
    int attr_offset[tar_nAttrs];
    int attr_type[tar_nAttrs];

    for(iter=0;iter<tar_nAttrs;iter++)
    {
        union Attribute attrcat[6];
        flag=getAttrCatEntry(srcrelid,tar_attrs[iter],attrcat);
        if(flag!=SUCCESS)
        {
            return flag; // attribute not in src rel.
        }
        attr_offset[iter]=attrcat[5].ival;
        attr_type[iter]=attrcat[2].ival;
    }

    flag=createRel(targetrel,tar_nAttrs,tar_attrs,attr_type);
    if(flag!=SUCCESS)
    {
        return flag; // unable to create target relation
    }

    targetrelid=openRel(targetrel);
    if(targetrelid==E_CACHEFULL)
    {
        flag=ba_delete(targetrel);
        return E_CACHEFULL;
    }
    //cout<<"projecttttt"<<endl;
    recId prev_recid={-1,-1};

    while(1)
    {
        union Attribute rec[nAttrs];
        union Attribute val;
        strcpy(val.sval,"PRJCT");
        char attr[ATTR_SIZE];
        strcpy(attr,"PRJCT");
        //cout<<"ba search of project"<<endl;
        flag=ba_search(srcrelid,rec,attr,val,PRJCT,&prev_recid);
        if(flag==SUCCESS)
        {
            //cout<<"hereeeee"<<endl;
            union Attribute proj_rec[tar_nAttrs];

            for(iter=0;iter<tar_nAttrs;iter++){
                proj_rec[iter]=rec[attr_offset[iter]];
            }
            flag=ba_insert(targetrelid,proj_rec);
            if(flag!=SUCCESS)
            {
                closeRel(targetrelid);
                ba_delete(targetrel);
                return flag; //unable to insert into target relation.
            }
        }
        else
            break;
        
    }
    closeRel(targetrelid);
    return SUCCESS;
}


int insert(char relname[ATTR_SIZE], int nAttrs, char record[][ATTR_SIZE])
{

    // get the relation's open relation id(let it be rel_id), using getRelId() method of Openreltable
    // if relation is not opened in Openreltable, return E_RELNOTOPEN
    int relid=getRelId(relname);
    if(relid==E_RELNOTOPEN)
        return relid;


    //get the no. of attributes present in relation, from RelcatEntry present in Openreltable(using getRelCatEntry() method).
    //if nAttrs!=no_of_attrs in relation, return E_NATTRMISMATCH
    union Attribute relcat_entry[6];
    getRelCatEntry(relid,relcat_entry);
    int noAttrs=relcat_entry[1].ival;
    if(noAttrs!=nAttrs)
        return E_NATTRMISMATCH;

    union  Attribute recval[nAttrs];

    /*iterate through 0 to nAttrs-1 :
        get the i'th attribute's AttrCatEntry (using getAttrcatEntry() of Openreltable )
    */
    // let type=attrcatentry.attr_type;
    union Attribute attrcat_entry[6];
    recId prev_recid={-1,-1};
    recId recid;
    char attrName[ATTR_SIZE];
    strcpy(attrName,"RelName");
    union Attribute a;
    strcpy(a.sval,OpenRelTable[relid]);
    for(int i=0;i<nAttrs;i++)
    {
        
        recid=linear_search(ATTRCAT_RELID,attrName,a,EQ,&prev_recid);
        getRecord(attrcat_entry,recid.block,recid.slot);
        int type=attrcat_entry[2].ival;
        if(type==INT)
            recval[i].ival=stoi(record[i]);
        else if(type==FLOAT)
            recval[i].fval=stof(record[i]);
        else if(type==STRING)
            strcpy(recval[i].sval,record[i]);


    }

    int retval= ba_insert(relid,recval);
    return retval;

    //return retval
}

int join(char srcrel1[ATTR_SIZE],char srcrel2[ATTR_SIZE],char targetrel[ATTR_SIZE], char attr1[ATTR_SIZE], char attr2[ATTR_SIZE])
{
    
    // get the srcrel1's open relation id(let it be srcrelid1), using getRelId() method of Openreltable in cache layer
    // if srcrel1 is not opened in open relation table, return E_RELNOTOPEN

    int srcrelid1=getRelId(srcrel1);
    if(srcrelid1==E_RELNOTOPEN)
        return srcrelid1;
    cout<<"!!!!!"<<endl;

    // get the srcrel2's open relation id(let it be srcrelid2), using getRelId() method of Openreltable in cache layer
    // if srcrel2 is not opened in open relation table, return E_RELNOTOPEN

    int srcrelid2=getRelId(srcrel2);
    if(srcrelid2==E_RELNOTOPEN)
        return srcrelid2;
    
    //get the AttrCatEntry of attr1 in rel1 and attr2 in rel2 using getAttrCatEntry() method of Openreltable in cache layer.
    union Attribute attrcat_entry1[6];
    int flag1=getAttrCatEntry(srcrelid1,attr1,attrcat_entry1);

    union Attribute attrcat_entry2[6];
    int flag2= getAttrCatEntry(srcrelid2,attr2,attrcat_entry2);
    
    // if attr1 is not present in rel1 or attr2 not present in rel2 (failure of call to Openreltable) return E_ATTRNOTEXIST.
    if(flag1!=SUCCESS ||flag2!=SUCCESS)
        return E_ATTRNOTEXIST;
    
    // if attr1 and attr2 are of different types return E_ATTRTYPEMISMATCH
    if(attrcat_entry1[2].ival!=attrcat_entry2[2].ival)
        return E_ATTRTYPEMISMATCH;
    
    // let nAttrs1 = no_of_attrs in rel1 ( can be found using getRelCatEntry() of Openreltable in cache layer).
    // let nAttrs2 = no_of_attrs in rel1 ( can be found using getRelCatEntry() of Openreltable in cache layer).
    union Attribute relcat_entry1[6];
    getRelCatEntry(srcrelid1,relcat_entry1);
    int nAttrs1=relcat_entry1[1].ival;

    union Attribute relcat_entry2[6];
    getRelCatEntry(srcrelid2,relcat_entry2); 
    int nAttrs2=relcat_entry2[1].ival;

    //skipped checking if index exists for at least one attribute
    //let tar_attrs[nAttrs1+nAttrs2-1][ATTR_SIZE] be an array of type char
    //let tar_attrtypes[nAttrs1+nAttrs2-1] be an array of type int
    /* iterate through all the attributes in both the source rels and update tar_attrs[],tar_attrtype[] arrays (except for attr2 in srcrel2),
    by getting attribute catalog of each attribute from Openreltable(using method getattrcat() in cache layer) */  
       

    char tar_attrs[nAttrs1+nAttrs2-1][ATTR_SIZE];
    int tar_attrtype[nAttrs1+nAttrs2-1];
    recId prev_recid={-1,-1};
    recId recid;
    char attrName[ATTR_SIZE];
    strcpy(attrName,"RelName");
    union Attribute a;
    strcpy(a.sval,OpenRelTable[srcrelid1]);
    for(int iter=0;iter<nAttrs1;iter++)
    {
        union Attribute att1[6];
        recid=linear_search(ATTRCAT_RELID,attrName,a,EQ,&prev_recid);
        getRecord(att1,recid.block,recid.slot);
        
        strncpy(tar_attrs[iter],att1[1].sval,ATTR_SIZE);
        tar_attrtype[iter]=att1[2].ival;
    }
    int attr_ind=nAttrs1;
    prev_recid={-1,-1};
    strcpy(a.sval,OpenRelTable[srcrelid2]);
    for(int iter=0;iter<nAttrs2;iter++)
    {
        union Attribute att2[6];
        recid=linear_search(ATTRCAT_RELID,attrName,a,EQ,&prev_recid);
        getRecord(att2,recid.block,recid.slot);

        if(strcmp(attr2,att2[1].sval)!=0)
        {
            tar_attrtype[attr_ind]=att2[2].ival;
            strncpy(tar_attrs[attr_ind++],att2[1].sval,ATTR_SIZE);
        }
    }


    // retval= createRel(targetrel,nAttrs1+nAttrs2-1,tar_attrs,tar_attrtypes);
    int flag=createRel(targetrel,nAttrs1+nAttrs2-1,tar_attrs,tar_attrtype);
    if(flag!=SUCCESS)
    {
        return flag; // target rel may already exist or attrs more than limit or ...
    }

    int targetrelid=openRel(targetrel);

    if(targetrelid==E_CACHEFULL)
    {
        flag=deleteRel(targetrel);
        return E_CACHEFULL;
    }

    //var: Union Attribute rec1[nAttrs1]
    /* while ba_search(srcrelid1,rec1, ,oper=PROJ, no_val) returns success:
            var: union Attribute Record[nAttrs2]
            while ba_search(srcrelid2,Record, attr2, oper=EQ, rel1 record's attr1 value) returns SUCCESS:
                 var: union Attribute tar_record[nAttrs1+nAttrs2-1]
                 copy the rel1's, rel2's record to tar_record[] (except for attr2 offset in rel2)
                 call ba_insert(targetrelid,tar_record);
                 if insert fails:
                    close the targetrel(by calling closeRel(targetrel) method of schema layer)
                    delete targetrel(by calling deleteRel(targetrel) of schema layer)
                    return E_DISKFULL
            
     */

    union Attribute rec1[nAttrs1];
    recId prev_recid1={-1,-1};
    union Attribute dummy;
    strcpy(dummy.sval," ");
    while(ba_search(srcrelid1,rec1,"PROJECT",dummy,PRJCT,&prev_recid1)==SUCCESS)
    {
        union Attribute rec2[nAttrs2];
        union Attribute tar_record[nAttrs1+nAttrs2-1];
        recId prev_recid2={-1,-1};
        while(ba_search(srcrelid2,rec2,attr2,rec1[attrcat_entry1[5].ival],EQ,&prev_recid2)==SUCCESS)
        {   //cout<<"beginning of hit"<<endl;
            for(int iter=0;iter<nAttrs1;iter++)
                tar_record[iter]=rec1[iter];
            
            int attr_ind=nAttrs1;
            for(int iter=0;iter<nAttrs2;iter++)
            {
                if(iter!=attrcat_entry2[2].ival)
                    tar_record[attr_ind++]=rec2[iter];
                
            }

            flag=ba_insert(targetrelid,tar_record);
            if(flag!=SUCCESS)
            {

                closeRel(targetrelid);
                deleteRel(targetrel);
                return flag;
            }
            
            //cout<<"end of hit"<<endl;
        }   
    }


    //close the target relation by calling closeRel() of schema layer.
     //return SUCCESS;
    closeRel(targetrelid);
    return SUCCESS;
}
    
    
    
   
    
    
    

    
    
    

    
