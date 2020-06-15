
typedef int relId; //for any opened relation

/* A record is identified by its block number and slot number */
typedef struct recId{
	int block;
	int slot;
} recId;

typedef struct SearchIndexId{
	int sblock;
	int sindex;
} SearchIndexId;
