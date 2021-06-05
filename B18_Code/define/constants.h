#define BLOCK_SIZE 2048
#define ATTR_SIZE 16
#define DISK_BLOCKS 8192
#define MAXOPEN 12

#define SUCCESS 0
#define FAILURE -1

#define USED 1 
#define UNUSED 0 
#define ALLOTED 2

#define REC 0
#define IND_INTERNAL 1
#define IND_LEAF 2
#define UNUSED_BLK 3

#define EQ 101
#define LE 102
#define LT 103
#define GE 104
#define GT 105
#define NE 106 //if considered
#define RST 100 //reset op.
#define PRJCT 107

#define INT 0
#define STRING 1
#define FLOAT 2


#define RELCAT_RELID 0
#define ATTRCAT_RELID 1
