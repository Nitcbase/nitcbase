#include<bits/stdc++.h>
#define REC 1
#define IND 2
using namespace std;
struct recBlock {
	int32_t blockType;
	int32_t pblock;
	int32_t lblock;
	int32_t rblock;
	int32_t numEntries;
	int32_t numAttrs; 
	int32_t numSlots;
	unsigned char reserved[4];
	unsigned char slotMap_Records[2048-104];
    unsigned char unused[72];	
};

 int main()
 {
 	 //cout<<sizeof(struct recBlock);
 	 unsigned char ch=(unsigned char)1;
 	 if(ch=='1')
 	 	cout<<"yesss";
 	 //cout<<(unsigned char)1;
 	 cout<<(int)ch<<endl;
 	 unsigned char ch1[5];
 	 ch1[1]='1';
 	 if((int32_t*)(ch1+1)==1)
 	 	cout<<"yes";
 }