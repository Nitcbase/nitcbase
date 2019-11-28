#include<bits/stdc++.h>
#include<string>
using namespace std;
int curindex=0;
int relid=0;
int atid=0;
int intnl=-1;
int leaf=-1;
class Relcat
{
    public:
    string relname;
    int n;
    int r,f,l;
        
};

class Attrcat
{
    public: 
    string relname;
    string attrname;
    int flag;
    int rootblocknum[2];
    int offset;
    int previndx[2];
    Attrcat()
    {
      previndx[0]=-1;
      previndx[1]=-1;
    }
    
};

Relcat rel[5];
Attrcat at[20];

class Record
{
public:
  int nentries;
  int nattributes;
  int slot[24];
  int nextblk;
  int prevblk;

};
Record recblk[40];


class leafindx
{
public:
   int attrval[3];
   int blocknum[3][2]; //store blocknum and rownum
   int nentries;
   int pblock;
   int lblock;
   int rblock;
    leafindx(){
    
      pblock=-1;
      nentries=0;
      lblock=-1;
      rblock=-1;
    }
};

struct intindx
{

    int attrval;
    int lchild[2];
    int rchild[2];
};
struct lfblk
{
  int attrval;
  int blocknum[2];
};

class intblk
{
public:
  
  int pblock;
  int nentries;
  struct intindx s[4];
  
  intblk(){
   
      pblock=-1;
      nentries=0;
  }
 
};
intblk internalblk[50];
leafindx leafblk[100];

void createtable()
{
    string name;
    int n,r,val,t,row, blkno,l,temp,i;
    cout<<"Enter name of relation : "<<endl;
    cin>>name;
    string attrname;
    cout<<"Enter no. of attributes and rows : "<<endl;
    cin>>n>>r;
    
    
    
    rel[relid].relname=name;
    cout<<"Relation id:"<<relid<<endl;
    rel[relid].n=n;
    rel[relid].r=r;
    rel[relid].f=curindex;


    float v=24/n;
    float rblock=floor(v);
    if (r>rblock)
    {
      float nblk=r/rblock;
      rel[relid].l=curindex+ceil(nblk)-1;
    }
    else
      rel[relid].l=curindex;
    
    t=0;
    cout<<"Enter names of attributes : "<<endl;
    while(t<n)
    {
      cin>>attrname;
      at[atid].relname=name;
      at[atid].attrname=attrname;
      at[atid].flag=-1;
      at[atid].offset=t;
      atid++;
      ++t;
    }
    
    blkno=curindex;
    l=rel[relid].l;
    int prev=-1;

    for(i=curindex;i<=l;i++)
      {

        recblk[i].prevblk=prev;
        prev=i;
        recblk[i].nextblk=i+1;

        recblk[i].nattributes=n;
        if(r>=rblock)
          recblk[i].nentries=rblock;
        else
          recblk[i].nentries=r;
        r=r-rblock;
      }
      recblk[l].nextblk=-1;


    row=0;
    while(blkno<=l && row<recblk[blkno].nentries)
    
    {
      int k=0;
      cout<<"Enter row entries :"<<endl;
      while(k<n)
      {
        cout<<"attribute "<<k+1<<endl;
        cin>>val;
        cout<<"block :"<<blkno<<endl;
        cout<<"slot:"<<row*n+k<<endl;
        recblk[blkno].slot[row*n+k]=val;
        k++;
      }      
      
      if(row==recblk[blkno].nentries-1)
      {
        blkno++;
        row=0;
      }
      else
        row++;
    }

    curindex=l+1;
    relid++;    
}


int get_attrcatentry(int rel_id, string attrname)
{
  int i;
  for(i=0;i<atid;i++)
  {
    if(rel[rel_id].relname==at[i].relname)
      if(attrname==at[i].attrname)
        return i;
  }
  return -1;
}



int bplustreeinsert(int rel_id,string attrname,int val,int recid[])
{

  
  int i,newchild,j;
  int entry= get_attrcatentry(rel_id, attrname);
  if(entry == -1)
   { 
      cout<<"no attribute";
      return 0;
   }

  if(at[entry].flag==-1)
      return 0; //rootblknum not success
  int head=at[entry].rootblocknum[0];
  int isleaf=at[entry].rootblocknum[1];
  
  while(isleaf==0) //as long as the entry is not a leaf
  {

    for(i=0;i<internalblk[head].nentries;++i)
    {
      cout<<internalblk[head].s[i].attrval<<endl;
      if(internalblk[head].s[i].attrval>val)
        break;
    }

    if(i==internalblk[head].nentries)
    {
      
      isleaf=internalblk[head].s[i-1].rchild[1];
      head=internalblk[head].s[i-1].rchild[0];
      
    }
    else// an offset is found
    {
      isleaf=internalblk[head].s[i].lchild[1];
      head=internalblk[head].s[i].lchild[0];
     

    }
    
  }

 
  int nentries=leafblk[head].nentries;
  

  struct lfblk array[nentries+1];
  int c=0, flag=0;

  for(i=0;i<nentries;++i)
  {

    if(val<leafblk[head].attrval[i] && flag==0)
    {
      array[c].attrval=val;
      array[c].blocknum[0]=recid[0];
      array[c].blocknum[1]=recid[1];
      flag=1;
      c++;

    }

    array[c].attrval=leafblk[head].attrval[i];
    array[c].blocknum[0]=leafblk[head].blocknum[i][0];
    array[c].blocknum[1]= leafblk[head].blocknum[i][1];
    c++;
  }
  if(flag==0)
  {
    array[c].attrval=val;
    array[c].blocknum[0]=recid[0];
    array[c].blocknum[1]=recid[1];
    c++;
    flag=1;
  }



  if(nentries!=3)
  {

    for(i=0;i<c;++i)
    {
      leafblk[head].attrval[i]=array[i].attrval;
      leafblk[head].blocknum[i][0]=array[i].blocknum[0];
      leafblk[head].blocknum[i][1]=array[i].blocknum[1];
    }
    
    leafblk[head].nentries=leafblk[head].nentries+1;

  }

  else
  {
    
    for(i=0;i<2;i++)
    {
      leafblk[head].attrval[i]=array[i].attrval;
      leafblk[head].blocknum[i][0]=array[i].blocknum[0];
      leafblk[head].blocknum[i][1]=array[i].blocknum[1];

    }
    leafblk[head].nentries=2;
    leaf++;
    for(j=0;j<2;j++)
    {
      leafblk[leaf].attrval[j]=array[i].attrval;
      leafblk[leaf].blocknum[j][0]=array[i].blocknum[0];
      leafblk[leaf].blocknum[j][1]=array[i].blocknum[1];
      i++;

    }
    

    leafblk[leaf].nentries=2;

    int headparblock= leafblk[head].pblock;
    newchild=leaf;
    
    leafblk[newchild].rblock=leafblk[head].rblock;
    leafblk[newchild].lblock=head;
    leafblk[head].rblock=newchild;

    leafblk[newchild].pblock=headparblock;
    int newattrval = leafblk[head].attrval[1];
    int childblock = head;
    isleaf=1;

    bool done = false;

    while(!done)
    {
      

      if (headparblock!=-1)
      {
        
        struct intindx intarray[5];
        flag=0;
        c=0;
        for (i = 0; i < internalblk[headparblock].nentries; ++i)
        {
          if(internalblk[headparblock].s[i].attrval>newattrval && flag==0)
          {
            intarray[c].attrval=newattrval;
            intarray[c].lchild[0]=internalblk[headparblock].s[i].lchild[0];
            intarray[c].lchild[1]=internalblk[headparblock].s[i].lchild[1];
            intarray[c].rchild[0]=leaf;
            intarray[c].rchild[1]=1;
            flag=1;
            c++;
          }

          if(c-1>=0)
          {
          intarray[c].lchild[0]=intarray[c-1].rchild[0];
          intarray[c].lchild[1]=intarray[c-1].rchild[1];
          }
          else
          {
          intarray[c].lchild[0]=internalblk[headparblock].s[i].lchild[0];
          intarray[c].lchild[1]=internalblk[headparblock].s[i].lchild[1];
          }
          intarray[c].attrval=internalblk[headparblock].s[i].attrval;
          intarray[c].rchild[0]=internalblk[headparblock].s[i].rchild[0];
          intarray[c].rchild[1]=internalblk[headparblock].s[i].rchild[1];
          c++;
            

        }
        if(flag==0)//when newattrval is greater than all parentblock enries
        {
          intarray[c].lchild[0]=intarray[c-1].rchild[0];
          intarray[c].lchild[1]=intarray[c-1].rchild[1];
          intarray[c].attrval=newattrval;
          intarray[c].rchild[0]=newchild;
          intarray[c].rchild[1]=isleaf;
          flag=1;
        }


        if(internalblk[headparblock].nentries!=4)
        {
          for (i= 0; i < (internalblk[headparblock].nentries+1); ++i)
          {
            internalblk[headparblock].s[i].attrval=intarray[i].attrval;
            internalblk[headparblock].s[i].lchild[0]=intarray[i].lchild[0];
            internalblk[headparblock].s[i].lchild[1]=intarray[i].lchild[1];
            internalblk[headparblock].s[i].rchild[0]=intarray[i].rchild[0];
            internalblk[headparblock].s[i].rchild[1]=intarray[i].rchild[1];
          
          }
          internalblk[headparblock].nentries=internalblk[headparblock].nentries +1;
          done=true;

        }

        else //if parentblock is full
        {
          intnl++; // to get a new internalblock
          //copy first 2 entries(0 to 1) to old internal block
          for (i = 0; i < 2; ++i)
          {
            internalblk[headparblock].s[i].attrval=intarray[i].attrval;
            internalblk[headparblock].s[i].lchild[0]=intarray[i].lchild[0];
            internalblk[headparblock].s[i].lchild[1]=intarray[i].lchild[1];
            internalblk[headparblock].s[i].rchild[0]=intarray[i].rchild[0];
            internalblk[headparblock].s[i].rchild[1]=intarray[i].rchild[1];
          
          }
          internalblk[headparblock].nentries=2;
          i=3;
          //copy last 2 entries (3 to 4) into new internal block
          for (j = 0; j< 2; ++j)
          {
            internalblk[intnl].s[j].attrval=intarray[i].attrval;
            internalblk[intnl].s[j].lchild[0]=intarray[i].lchild[0];
            internalblk[intnl].s[j].lchild[1]=intarray[i].lchild[1];
            if(intarray[i].lchild[1]==1)
              leafblk[intarray[i].lchild[0]].pblock=intnl;
            else
              internalblk[intarray[i].lchild[0]].pblock=intnl;

            internalblk[intnl].s[j].rchild[0]=intarray[i].rchild[0];
            internalblk[intnl].s[j].rchild[1]=intarray[i].rchild[1];
          
            i++;
          }
          if(intarray[i-1].rchild[1]==1)
            leafblk[intarray[i-1].rchild[0]].pblock=intnl;
          else
            internalblk[intarray[i-1].rchild[0]].pblock=intnl;

          internalblk[intnl].nentries=2;
          head=headparblock;
          headparblock=internalblk[head].pblock;
          internalblk[intnl].pblock=internalblk[head].pblock;
          newchild=intnl;
          isleaf=0;
          newattrval=intarray[2].attrval;

        }

      }
      else //if headparblock==-1 i.e root is split now
      {
        
        intnl++;
        internalblk[intnl].s[0].attrval=newattrval;
        internalblk[intnl].s[0].lchild[0]=head;
        internalblk[intnl].s[0].lchild[1]=isleaf;
        internalblk[intnl].s[0].rchild[0]=newchild;
        internalblk[intnl].s[0].rchild[1]=isleaf;
        internalblk[intnl].nentries=1;
        leafblk[head].pblock=intnl;
        leafblk[newchild].pblock=intnl;
        at[entry].rootblocknum[0]=intnl;
        at[entry].rootblocknum[1]=0;
       
        int newrootblock=at[entry].rootblocknum[0];
       

        done=true;

      }


    }

  }

return 1;
}

int get_relid(string name)
{
    int i;
    for(i=0;i<relid;++i)
    {
      if(rel[i].relname==name)
        return i;
        
    }
    return -1;
}


void createbplustree()
{
       string nam;
       string attrname;
       int i,j,relation_id,offset,attrval,n,x,y;
       cout<<"relation name:"<<endl;
       cin>> nam;
       cout<<"attribute name:"<<endl;
       cin>> attrname;
       relation_id=get_relid(nam);

       if(relation_id==-1)
       {
           cout<<"no such relation";
           return;
       }

       for(i=0;i<atid;++i)
       {
           if(nam==at[i].relname)
           {
               if(attrname==at[i].attrname)
               {
                   if(at[i].flag==-1)

		               {
                       
                       leaf++;
                       at[i].rootblocknum[0]= leaf;
                       at[i].rootblocknum[1]= 1;
                       at[i].flag=1 ;//flag is set

                       leafblk[leaf].nentries=0;
                       offset=at[i].offset;

                      
                       break;

                       
		               }  
                }
            }
        }
        
        for(j=rel[relation_id].f;j!=-1;)
        {
            i=0;
            n=recblk[j].nattributes;
            while(i<recblk[j].nentries)
            {


              attrval=recblk[j].slot[i*n+offset];
              int recid[2]={j,i*n};
              int res=bplustreeinsert(relation_id,attrname,attrval,recid);
              if(res==0)
                return;
              cout<<"\nINSERT "<<attrval<<"(block "<<recid[0]<<", slot "<<recid[1]<<")"<<endl;
              for(x=0;x<=leaf;x++)
              {
                cout<<"L"<<x<<" ( ";
                for (y = 0; y < (leafblk[x].nentries); ++y)
                {
                  cout<<leafblk[x].attrval[y]<<"[block "<<leafblk[x].blocknum[y][0]<<", slot "<<leafblk[x].blocknum[y][1]<<"] ";
                }
                cout<<")"<<endl;
              }
              for(x=0;x<=intnl;x++)
              {
                cout<<"I"<<x<<" (";
                for ( y = 0; y < (internalblk[x].nentries); ++y)
                {
                  
                  if(internalblk[x].s[y].lchild[1]==1)
                    cout<<" L"<<internalblk[x].s[y].lchild[0]<<", ";
                  else
                    cout<<" I"<<internalblk[x].s[y].lchild[0]<<", ";
                  cout<<internalblk[x].s[y].attrval<<", ";
                  
                }

                if(internalblk[x].s[y-1].rchild[1]==1)
                    cout<<"L"<<internalblk[x].s[y-1].rchild[0]<<" )"<<endl;
                else
                    cout<<"I"<<internalblk[x].s[y-1].rchild[0]<<" )"<<endl;
              }

              
              i++;
            }
            j=recblk[j].nextblk;
         }
               
       return ;
}

int main()
{
    while(1)
    {
    int a,i,j;
    cout<<"1. Create table"<<endl;
    cout<<"2. B+ Tree" <<endl;
    cout<<"3. Display"<<endl;
    cout<<"4. Exit"<<endl;
    cin>>a;
    switch (a)
    {
        case 1:createtable();
               break;
        case 2:createbplustree();
               break;
        case 3: 
        for(i=0;i<=leaf;i++)
        {
          cout<<"L"<<i<<" lblock:"<<leafblk[i].lblock<<" rblock:"<<leafblk[i].rblock<<",  ( ";
          for (j = 0; j < (leafblk[i].nentries); ++j)
          {
            cout<<leafblk[i].attrval[j]<<" ";
          }

          cout<<")"<<endl;
        }
        for(i=0;i<=intnl;i++)
        {
          cout<<"I"<<i<<" (";
          for ( j = 0; j < (internalblk[i].nentries); ++j)
          {
            
            if(internalblk[i].s[j].lchild[1]==1)
              cout<<" L"<<internalblk[i].s[j].lchild[0]<<", ";
            else
              cout<<" I"<<internalblk[i].s[j].lchild[0]<<", ";
            cout<<internalblk[i].s[j].attrval<<", ";
            
          }

          if(internalblk[i].s[j-1].rchild[1]==1)
              cout<<"L"<<internalblk[i].s[j-1].rchild[0]<<" )"<<endl;
          else
              cout<<"I"<<internalblk[i].s[j-1].rchild[0]<<" )"<<endl;
        }

        break;
        case 4:exit(0);
        break;
        default:
        break;
       
  
    }
    
    }
    return 0;  
}
