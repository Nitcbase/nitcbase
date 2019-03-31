typedef struct fnode{
	char str[ATTR_SIZE];
	int ival;
	struct fnode *right;
	struct fnode *next;
} fnode;

typedef struct Condition{
	char attr[ATTR_SIZE];
	int op;
	char value[ATTR_SIZE];
} Condition;