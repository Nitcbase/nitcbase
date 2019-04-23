/* insert an index into b-tree */
#include "../buffer/blockBuffer.h"
#include "../buffer/buffer.h"
#include "../define/id.h"
#include "../cache/cache.h"
#include <cstring>
int compareAttr(union Attribute val1,union Attribute val2,int type ){
    if(type==INT){
        if(val1.ival==val2.ival){
            return 0;
        }else if(val1.ival<val2.ival){
            return 1;
        }else{
            return 2;
        }
    }else if(type==STRING){
        if(strcmp(val1.strval,val2.strval)==0){
            return 0;
        }else if(strcmp(val1.strval,val2.strval)<0){
            return 1;
        }else{
            return 2;
        }
    }
}

void setparent(int childblock,int parblock){
    class IndBuffer *tempchildblk=Buffer::getIndBlock(childblock);
    struct HeadInfo tempheader=tempchildblk->getheader();
    tempheader.pblock=parblock;
    delete tempchildblk;
    return;
}

int bplus_insert(int relid,char attrname[ATTR_SIZE],union Attribute val,recId rec_id){
    AttrCatEntry tar_attr;
    OpenRelTable::getAttrCatEntry(relid,attrname,&tar_attr);
    int root_block=tar_attr.root_block, type_attr=tar_attr.attr_type;
    struct Index indexval;
    indexval.attrval=val;
    indexval.block=rec_id.block;
    indexval.slot=rec_id.slot;
    if(root_block==-1){
        return E_NOINDEX;
    }

    class IndBuffer *head=Buffer::getIndBlock(root_block);
    struct HeadInfo header=head->getheader();

    while(header.block_type==IND_INTERNAL){
        int nEntries=header.num_entries;
        int child_head;

        struct InternalEntry elem;
        int getter;
        getter=head->getEntry((void *)(&elem),0);
        
        
        int iter;
        for(iter=0;iter<nEntries;iter++){
            getter=head->getEntry((void *)(&elem),iter);
            int compare_val1=compareAttr(indexval.attrval,elem.attrval,type_attr);
            if(compare_val1<=1){
                child_head=elem.lchild;
                break;
            }

        }
        if(iter==nEntries){
            child_head=elem.rchild;
        }
        
        delete head;
        root_block=child_head;
        class IndBuffer *head=Buffer::getIndBlock(root_block);
        header=head->getheader();
    }

    int nEntries=header.num_entries;
    bool success = false;
    int getter,setter;
    if(nEntries!=63){
        struct Index elem;
        int iter;
        for(iter=0;iter<nEntries;iter++){
            getter=head->getEntry((void *)(&elem),iter);
            int compare_val=compareAttr(indexval.attrval,elem.attrval,type_attr);
            if(compare_val<=1){
                break;
            }
        }
        int ind=iter;
        for(iter=nEntries;iter>ind;iter--){
            getter=head->getEntry((void *)(&elem),iter-1);
            setter=head->setEntry((void *)(&elem),iter);
        }

        setter=head->setEntry((void *)(&indexval),ind);
        header.num_entries=nEntries+1;
        head->setheader(header);
        delete head; //can be kept at end......
        success=true;
    }else{
        int par_block=header.pblock;
        struct Index attrs[nEntries+1];
        int iter;
        int ind;

        struct Index elem;
        for(iter=0;iter<nEntries;iter++){
            getter=head->getEntry((void *)(&elem),iter);
            int compare_val=compareAttr(indexval.attrval,elem.attrval,type_attr);
            if(compare_val==0||compare_val==2){
                ind=iter;
            }
            attrs[iter]=elem;
        }
        ind++;
        for(iter=nEntries;iter>ind;iter--){
            attrs[iter]=attrs[iter-1];
        }
        attrs[ind]=indexval;

        class IndBuffer* new_blk=Buffer::getFreeIndLeaf(); //if fails..delete indexing
        struct HeadInfo newblkheader=new_blk->getheader();

        for(iter=0;iter<32;iter++){
            setter=head->setEntry((void *)(&attrs[iter]),iter);
            setter=new_blk->setEntry((void *)(&attrs[iter]),32+iter);
        }
        union Attribute new_attrval;
        new_attrval=attrs[31].attrval;
        header.num_entries=32;
        newblkheader.num_entries=32;
        newblkheader.pblock=par_block;
        newblkheader.rblock=header.rblock; 
        newblkheader.lblock=root_block;
        header.rblock=new_blk->getblocknum();
        class IndBuffer *right_blk=Buffer::getIndBlock(newblkheader.rblock);
        struct HeadInfo rblk_header=right_blk->getheader();
        rblk_header.lblock=new_blk->getblocknum();
        right_blk->setheader(rblk_header);
        delete right_blk;

        
        head->setheader(header);
        new_blk->setheader(newblkheader);
        int new_blocknum=new_blk->getblocknum();
        int child_block=root_block;

        delete new_blk;
        delete head;    //can be kept at end......

        while(success==false){
            if(par_block!=-1){
                class IndBuffer *parblk=Buffer::getIndBlock(par_block);
                struct HeadInfo parheader=parblk->getheader();
                
                struct InternalEntry internal_entries[parheader.num_entries+1];
                int iter;
                int tar_ind=parheader.num_entries;

                for(iter=0;iter<parheader.num_entries;iter++){
                    getter=head->getEntry((void *)(&internal_entries[iter]),iter);
                    if(internal_entries[iter].lchild==child_block){
                        tar_ind=iter;
                    }
                }
                for(iter=parheader.num_entries;iter>tar_ind;iter++){
                    internal_entries[iter]=internal_entries[iter-1];
                }
                internal_entries[tar_ind].attrval=new_attrval;
                internal_entries[tar_ind].lchild=child_block;
                internal_entries[tar_ind].rchild=new_blocknum;

                if(tar_ind!=parheader.num_entries){
                    internal_entries[tar_ind+1].lchild=internal_entries[tar_ind].rchild;
                }

                if(parheader.num_entries!=100){
                    for(iter=0;iter<=parheader.num_entries;iter++){
                        setter=parblk->setEntry((void *)(&internal_entries[iter]),iter);
                    }
                    parheader.num_entries=parheader.num_entries+1;
                    parblk->setheader(parheader);
                    newblkheader.pblock=par_block;  //might have to change this line to load new_blocknum or even might not need this.
                    new_blk->setheader(newblkheader);
                    success=true;
                    
                    delete parblk;
                }else{
                    class IndBuffer* new_blk=Buffer::getFreeIndInternal(); //if fails..delete indexing
                    newblkheader=new_blk->getheader();

                    for(iter=0;iter<50;iter++){
                        setter=parblk->setEntry((void *)(&internal_entries[iter]),iter);
                        setter=new_blk->setEntry((void *)(&internal_entries[51+iter]),iter);
                    }
                    parheader.num_entries=50;
                    newblkheader.num_entries=50;
                    //newblkheader.pblock=parheader.pblock;
                    parblk->setheader(parheader);
                    new_blk->setheader(newblkheader);
                    new_blocknum=new_blk->getblocknum();
                    //set the pblock of all childs appropriately
                    for(iter=0;iter<newblkheader.num_entries;iter++){
                        setparent(internal_entries[iter].lchild,new_blocknum);
                    }
                    setparent(internal_entries[newblkheader.num_entries-1].rchild,new_blocknum);

                    delete parblk;
                    delete new_blk;
                    
                    child_block=par_block;
                    par_block=parheader.pblock;
                    struct InternalEntry internalval;
                    new_attrval=internal_entries[50].attrval;
                    
                }


            }else{
                //get new internal block 
                class IndBuffer* new_head=Buffer::getFreeIndInternal(); //if fails..delete indexing
                struct HeadInfo newhead_header=new_blk->getheader();

                struct InternalEntry internalval;
                internalval.attrval=new_attrval;
                internalval.lchild=child_block;
                internalval.rchild=new_blocknum;
                new_head->setEntry((void *)&internalval,0);

                newhead_header.pblock=-1;
                newhead_header.num_entries=1;
                new_head->setheader(newhead_header);

                //update head of respective cache element
                AttrCatEntry attrib_cat;
                int flag= OpenRelTable::getAttrCatEntry(relid,attr_offset,&attrib_cat);
                attrib_cat.root_block=new_head->getblocknum();
                int flag= OpenRelTable::setAttrCatEntry(relid,attr_offset,&attrib_cat);
                
                //update par of both rblock and lblock
                setparent(child_block,new_head->getblocknum());
                setparent(new_blocknum,new_head->getblocknum());

                delete new_head;
                success=true;
            }
        }

    }

   // delete head;
    return SUCCESS;
}