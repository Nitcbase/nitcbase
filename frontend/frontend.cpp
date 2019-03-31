//#include"frontend.h"

fnode *getNode(char *str, int ival, fnode *r, fnode *n){
	fnode *newnode = (fnode *)malloc(sizeof(fnode));
	memset(newnode, 0, sizeof(fnode));
	if(str!=NULL){
		strncpy(newnode->str, str, 15);
	}
	newnode->ival = ival;
	newnode->right = r;
	newnode->next = n;
	return newnode;
}

fnode *setright(fnode *l, fnode *r){
	fnode *tmp;
	//Assumes l is not NULL
	tmp = l;
	while(tmp->next != NULL){
		tmp = tmp->next;
	}
	while(tmp->right != NULL){
		tmp = tmp->right;
	}
	tmp->right = r;
	return l;
}

fnode *setnext(fnode *f1, fnode *f2){
	fnode *tmp;
	//Assumes f1 is not NULL
	tmp = f1;
	while(tmp->next != NULL){
		tmp = tmp->next;
	}
	tmp->next = f2;
	return f1;
}

void freeall(fnode *f){ //recurcively free all malloced nodes
	if(f==NULL){
		return;
	}
	freeall(f->next);
	freeall(f->right);
	free(f);
}

int hsize(fnode *f){
	int size=0;
	fnode *t = f;
	while(t!=NULL){
		size++;
		t=t->right;
	}
	return size;
}

int vsize(fnode *f){
	int size=0;
	fnode *t = f;
	while(t!=NULL){
		size++;
		t=t->next;
	}
	return size;
}

/* DDL commands */

void fe_createrel(fnode *rel, fnode *clist){

}

void fe_droprel(fnode *rel){

}

void fe_renamerel(fnode *oldrel, fnode *newrel){

}

void fe_renamecol(fnode *rel, fnode *oldcol, fnode *newcol){

}

void fe_createind(fnode *rel, fnode *col){

}

void fe_dropind(fnode *rel, fnode *col){

}

/* DML Commands */
/* ATTEMPT 1
void fe_select(fnode *attrs, fnode *dstrel, fnode *srcrel, fnode *condlist){
	fnode *temp;
	int num_attr = hsize(attrs);
	char attrbs[num_attr][ATTR_SIZE];
	temp=attrs;
	for(int i=0; i<num_attr; i++){
		strcpy(attrbs[i], temp->str);
		temp = temp->right;
	}
	int num_cond = vsize(condlist);
	Condition conds[num_cond];
	temp = condlist;
	for(int i=0; i<num_cond; i++){
		strcpy(conds[i].attr, temp->str);
		strcpy(conds[i].value, temp->right->str);
		conds[i].op = temp->ival;
		temp = temp->next;
	}
	int retval = sql_select( num_attr, attrbs, dstrel->str, srcrel->str, num_cond, conds );
}
*/
void fe_select(fnode *attrs, fnode *dstrel, fnode *srcrel, fnode *condlist){
	
}

void fe_join(fnode *attrs, fnode *dstrel, fnode *srcrel1, fnode *srcrel2, fnode *attr1, fnode *attr2){

}