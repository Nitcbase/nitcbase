#define BLOCK_SIZE 2048 //used
#define ATTR_SIZE 16 //used
#define DISK_BLOCKS 8192 
#define MAXOPEN 12

#define SUCCESS 0 //used
#define FAILURE -1 //used

#define USED 1 //used 
#define UNUSED -1  //used
#define ALLOTED 2 //used? in block allocn map

#define REC 0 //used
#define IND_INTERNAL 1 //used
#define IND_LEAF 2 //used

#define EQ 101
#define LE 102
#define LT 103
#define GE 104
#define GT 105
#define NE 106 //if considered
#define RST 100 //reset op.

#define INT 0
#define STRING 1
#define FLOAT 2


#define RELCAT_RELID 0
#define ATTRCAT_RELID 1
