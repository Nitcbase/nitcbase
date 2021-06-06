#include<stdio.h>
#include<stdlib.h>
//#include<bits/stdc++.h>
using namespace std;

#include "schema.cpp"
//#include "algebra.cpp"
//#include "algebra.cpp"

//TO DO
//make sure relation catalog and attribute catalog cannot be changed by delete,rename etc
//check in delete blk, delete rel on the type entered in blk allocation map
//Tell Aparnas team that ECACHEFULL and ERELNOTEXISTS cannot take values from 0 to 11


int check_type(char *data) {
	int count_int = 0, count_dot = 0, count_string = 0, i;
	for (i = 0; data[i] != '\0'; i++) {

		if (data[i] >= '0' && data[i] <= '9')
			count_int++;
		if (data[i] == '.')
			count_dot++;
		else
			count_string++;
	}

	if (count_dot == 1 && count_int == (strlen(data) - 1))
		return FLOAT;
	if (count_int == strlen(data)) {
		return INT;
	} else
		return STRING;
}

int insert_val(vector<string> s, char tablename[16]) {
	char ch;
	int relid = getRelId(tablename);
	if (relid == E_CACHEFULL || relid == E_RELNOTEXIST || relid == E_RELNOTOPEN) {
		//cout<<"Relation not open\n";
		return relid;
	}
	union Attribute relcatentry[6];
	int y;
	y = getRelCatEntry(relid, relcatentry);
	if (y != SUCCESS) {
		//cout<<"Insert Failed\n";
		return y;
	}
	int count;
	count = relcatentry[1].ival;
	int type[count];
	//getting the types of all attributes
	union Attribute attr[6];
	int attr_blk = 5;
	char Attr_name[6][16];
	int j = 0;
	struct HeadInfo h;
	while (attr_blk != -1) {
		h = getheader(attr_blk);
		unsigned char slotmap[h.numSlots];
		getSlotmap(slotmap, attr_blk);
		for (int i = 0; i < 20; i++) {
			getRecord(attr, attr_blk, i);
			if ((char) slotmap[i] == '0') {
				continue;
			}
			if (strcmp(attr[0].sval, tablename) == 0) {
				type[attr[5].ival] = attr[2].ival;
			}
		}
		attr_blk = h.rblock;
	}
	//-----------------------------------------------------

	char record_array[count][16];
	if (s.size() != count) {
		//cout<<"Mismatch in no of arguments\n";
		return E_NATTRMISMATCH;
	}
	//cout<<"hi\n";
	//cout<<count<<"\n";
	for (int i = 0; i < count; i++) {
		string x = s[i];
		char p[16];
		int j;
		for (j = 0; j < 15 && j < x.size(); j++) {
			p[j] = x[j];
		}
		p[j] = '\0';
		strcpy(record_array[i], p);
	}

	union Attribute rec[count];
	for (int l = 0; l < count; l++) {
		if (type[l] == FLOAT) {
			// cout<<record_array[l]<<"\n";
			if (check_type(record_array[l]) == FLOAT || check_type(record_array[l]) == INT)
				rec[l].fval = atof(record_array[l]);
			else
				return E_ATTRTYPEMISMATCH;


			//cout<<rec[l].fval<<"\n";
		}
		if (type[l] == INT) {
			//cout<<record_array[l]<<"\n";
			if (check_type(record_array[l]) != INT)
				return E_ATTRTYPEMISMATCH;
			rec[l].ival = strtoull(record_array[l], NULL, 10);
			//cout<<rec[l].ival<<"\n";
		}
		if (type[l] == STRING) {
			//cout<<record_array[l]<<"\n";

			strcpy(rec[l].sval, record_array[l]);
			//cout<<rec[l].sval<<"\n";
		}
	}
	int r;
	//cout<<"Calling insert"<<endl;
	r = ba_insert(relid, rec);
	if (r == SUCCESS) {
		return SUCCESS;
	} else {
		//cout<<"FAILURE\n";
		return FAILURE;
	}


}

int insert(char tablename[16], char *filename) {
	FILE *file = fopen(filename, "r");
	char ch;
	int relid = getRelId(tablename);
	if (relid == E_CACHEFULL || relid == E_RELNOTEXIST || relid == E_RELNOTOPEN) {
		//cout<<"INSERT NOT POSSIBLE\n";
		return relid;
	}
	union Attribute relcatentry[6];
	int y = getRelCatEntry(relid, relcatentry);
	if (y != SUCCESS) {
		//cout<<"INSERT NOT POSSIBLE\n";
		return y;
	}
	int count;
	count = relcatentry[1].ival;
	int type[count];

	//getting the types of all attributes
	union Attribute attr[6];
	int attr_blk = 5;
	char Attr_name[6][16];
	int j = 0;
	struct HeadInfo h;
	char s[16];
	while (attr_blk != -1) {
		h = getheader(attr_blk);
		unsigned char slotmap[h.numSlots];
		getSlotmap(slotmap, attr_blk);
		for (int i = 0; i < 20; i++) {
			getRecord(attr, attr_blk, i);
			if ((char) slotmap[i] == '0') {
				continue;
			}
			if (strcmp(attr[0].sval, tablename) == 0) {
				type[attr[5].ival] = attr[2].ival;
			}
		}
		attr_blk = h.rblock;
	}
	//-----------------------------------------------------
	char *record = (char *) malloc(sizeof(char));
	int len = 1;
	while (1) {
		ch = fgetc(file);
		if (ch == EOF)
			break;
		while (ch == ' ' || ch == '\t' || ch == '\n') {
			ch = fgetc(file);
		}
		if (ch == EOF)
			break;
		len = 1;
		int c_c = 0;
		char oldch = ',';
		while ((ch != '\n') && (ch != EOF)) {
			if (ch == ',')
				c_c++;
			if (oldch == ch && ch == ',') {
				cout << "Null values not allowed\n";
				return FAILURE;
			}
			record[len - 1] = ch;
			len++;
			record = (char *) realloc(record, (len) * sizeof(char));
			oldch = ch;
			ch = fgetc(file);

		}
		if (oldch == ',' && ch != '\n') {
			cout << "Null values not allowed in attribute values\n";
			return FAILURE;
		}
		if (count != c_c + 1 && ch != '\n') {
			//closeRel(relid);
			cout << "Mismatch in number of attributes\n";
			return FAILURE;
		}
		record[len - 1] = '\0';
		int i = 0;
		//record contains each record in the file (seperated by commas)
		char record_array[count][16];
		j = 0;
		while (j < count) {
			int k = 0;

			while (((record[i] != ',') && (record[i] != '\0')) && (k < 15)) {
				record_array[j][k++] = record[i++];
			}
			if (k == 15) {
				while (record[i] != ',')
					i++;
			}
			i++;
			record_array[j][k] = '\0';
			j++;
		}
		union Attribute rec[count];
		for (int l = 0; l < count; l++) {
			if (type[l] == FLOAT) {
				if (check_type(record_array[l]) == FLOAT || check_type(record_array[l]) == INT)
					rec[l].fval = atof(record_array[l]);
				else
					return E_ATTRTYPEMISMATCH;
			}
			if (type[l] == INT) {
				if (check_type(record_array[l]) != INT)
					return E_ATTRTYPEMISMATCH;
				rec[l].ival = strtoull(record_array[l], NULL, 10);
			}
			if (type[l] == STRING) {
				strcpy(rec[l].sval, record_array[l]);
			}
		}
		int r;
		r = ba_insert(relid, rec);
		if (r != SUCCESS)
			return FAILURE;

		//Assuming insert function returns 1 for successsful insert and 0 for unsuccessful insert

		if (ch == EOF)
			break;
	}
	//closeRel(relid);
	fclose(file);
	return SUCCESS;
}


int import(char *filename) {
	FILE *file = fopen(filename, "r");
	//attr : first line in the file
	//count : no of attributes in the file
	char *attr = (char *) malloc(sizeof(char));
	int len = 1;
	char ch, oldch;
	int count = 1;
	oldch = ',';
	while ((ch = fgetc(file)) != '\n') {

		if (ch == EOF)
			break;
		while (ch == ' ' || ch == '\t' || ch == '\n') {
			ch = fgetc(file);
		}
		if (ch == EOF)
			break;
		if (ch == ',') {
			count++;
			if (oldch == ch) {
				cout << "Null values are not allowed in attribute names\n";
				return FAILURE;
			}
		}
		attr[len - 1] = ch;
		len++;
		attr = (char *) realloc(attr, (len) * sizeof(char));
		oldch = ch;
		//  cout<<oldch<<"\n";
	}
	if (oldch == ',') {
		cout << "Null values are not allowed in attribute names\n";
		return FAILURE;
	}
	attr[len - 1] = '\0';
	int i = 0, j, k;
	char attribute[count][16];
	j = 0;
	while (j < count) {
		k = 0;
		while (((attr[i] != ',') && (attr[i] != '\0')) && (k < 15)) {
			attribute[j][k++] = attr[i++];
		}
		if (k == 15) {
			while (attr[i] != ',')
				i++;
		}
		attribute[j][k] = '\0';
		j++;
		i++;
	}
	i = 0;
	//attribute array contains the names of all attributes


	char *attr_type = (char *) malloc(sizeof(char));
	len = 1;
	while ((ch = fgetc(file)) != '\n') {
		attr_type[len - 1] = ch;
		len++;
		attr_type = (char *) realloc(attr_type, (len) * sizeof(char));
	}
	attr_type[len - 1] = '\0';
	i = 0;
	//attr_type contains second line in the file (to know the type)
	char attribute_type[count][16];
	int type[count];
	j = 0;
	while (j < count) {
		k = 0;
		while (((attr_type[i] != ',') && (attr_type[i] != '\0')) && (k < 15)) {
			attribute_type[j][k++] = attr_type[i++];
		}
		attribute_type[j][k] = '\0';
		int t = check_type(attribute_type[j]);
		if (t == FLOAT)
			type[j] = FLOAT;
		if (t == INT)
			type[j] = INT;
		if (t == STRING)
			type[j] = STRING;
		j++;
		i++;
	}

	i = 0;
	//type array contains the types of all attributes
	char newfilename[16];
	int loopv = strlen(filename) - 1;
	while (filename[loopv] != '.') {
		//newfilename[loopv]=filename[loopv];
		loopv--;
	}
	loopv--;
	int end = loopv;
	while (filename[loopv] != '/') {
		loopv--;
	}
	int start = loopv + 1;
	int f = 0;
	for (; start <= end; start++) {
		newfilename[f] = filename[start];
		f++;
	}
	newfilename[f] = '\0';
	int ret;
	ret = createRel(newfilename, count, attribute, type);
	if (ret != SUCCESS) {
		cout << "Import not possible as createRel failed\n";
		return FAILURE;
	}
	int relid = openRel(newfilename);
	if (relid == E_CACHEFULL || relid == E_RELNOTEXIST) {
		cout << "Import not possible as openRel failed\n";
		return FAILURE;
	}
	file = fopen(filename, "r");
	while ((ch = fgetc(file)) != '\n')
		continue;

	char *record = (char *) malloc(sizeof(char));
	len = 1;

	while (1) {
		ch = fgetc(file);
		if (ch == EOF)
			break;
		while (ch == ' ' || ch == '\t' || ch == '\n') {
			ch = fgetc(file);
		}
		if (ch == EOF)
			break;
		len = 1;
		int c_c = 0;
		oldch = ',';
		while ((ch != '\n') && (ch != EOF)) {
			if (ch == ',')
				c_c++;
			if (ch == oldch && ch == ',') {

				closeRel(relid);
				ba_delete(newfilename);
				//cout<<ch;
				cout << "Null values are not allowed in attribute names\n";
				return FAILURE;
			}
			record[len - 1] = ch;
			len++;
			record = (char *) realloc(record, (len) * sizeof(char));
			oldch = ch;

			ch = fgetc(file);

		}

		if (oldch == ',' && ch != '\n') {
			closeRel(relid);
			ba_delete(newfilename);

			cout << "Null values are not allowed in attribute names\n";
			return FAILURE;
		}
		if (count != c_c + 1 && ch != '\n') {
			closeRel(relid);
			ba_delete(newfilename);
			cout << "Mismatch in number of attributes\n";
			return FAILURE;
		}
		// cout<<record<<"\n";
		record[len - 1] = '\0';
		i = 0;
		//record contains each record in the file (seperated by commas)
		char record_array[count][16];
		j = 0;
		while (j < count) {
			k = 0;

			while (((record[i] != ',') && (record[i] != '\0')) && (k < 15)) {
				record_array[j][k++] = record[i++];
			}
			if (k == 15) {
				while (record[i] != ',')
					i++;
			}
			i++;
			record_array[j][k] = '\0';

			j++;
		}

		union Attribute rec[count];
		for (int l = 0; l < count; l++) {
			if (type[l] == FLOAT) {
				if (check_type(record_array[l]) == FLOAT || check_type(record_array[l]) == INT)
					rec[l].fval = atof(record_array[l]);
				else
					return E_ATTRTYPEMISMATCH;
			}
			if (type[l] == INT) {
				if (check_type(record_array[l]) != INT)
					return E_ATTRTYPEMISMATCH;
				rec[l].ival = strtoull(record_array[l], NULL, 10);
			}
			if (type[l] == STRING) {
				strcpy(rec[l].sval, record_array[l]);
			}
		}
		int r;
		r = ba_insert(relid, rec);
		union Attribute attr5[6];
		getRecord(attr5, 4, 3);

		//Assuming insert function returns 1 for successsful insert and 0 for unsuccessful insert
		if (r != SUCCESS) {
			closeRel(relid);
			ba_delete(newfilename);
			cout << "Insert failed" << endl;
			return FAILURE;
		}
		if (ch == EOF)
			break;

	}
	closeRel(relid);
	fclose(file);
	return SUCCESS;
}

int exp(char *rel_name, char *exportname) {
	//cout<<"here\n";
	FILE *fp_export = fopen(exportname, "w");
	if (!fp_export) {
		cout << " Invalid file path" << endl;
		return FAILURE;
	}
	struct HeadInfo header;
	int i, first_block;
	union Attribute rec[6]; //for relation catalog entry
	int num_slots, next_block_num, num_attrs, no_attrs;
	for (i = 0; i < 20; i++) {            //cout<<"entered\n";
		getRecord(rec, 4, i);
		if (strcmp(rec[0].sval, rel_name) == 0) {
			first_block = rec[3].ival;
			no_attrs = rec[1].ival;
			break;
		}
	}
	//cout<<"exited\n";
	if (i == 20) {
		cout << "The relation does not exist\n";
		return FAILURE;
	}
	if (first_block == -1) {
		cout << "No records exist\n";
		return FAILURE;
	}

	union Attribute attr[6];
	int attr_blk = 5;
	char Attr_name[no_attrs][16];
	int Attr_type[no_attrs];
	int j = 0;
	while (attr_blk != -1) {
		header = getheader(attr_blk);
		next_block_num = header.rblock;
		for (i = 0; i < 20; i++) {
			getRecord(attr, attr_blk, i);
			if (strcmp(attr[0].sval, rel_name) == 0) {
				strcpy(Attr_name[j], attr[1].sval);
				Attr_type[j++] = attr[2].ival;
			}
		}
		attr_blk = next_block_num;
	}
	for (j = 0; j < no_attrs; j++) {
		fputs(Attr_name[j], fp_export);
		if (j != no_attrs - 1)
			fputs(",", fp_export);
	}
	fputs("\n", fp_export);
	int block_num = first_block;
	while (block_num != -1) {
		header = getheader(block_num);
		num_slots = header.numSlots;
		next_block_num = header.rblock;
		num_attrs = header.numAttrs;
		unsigned char slotmap[num_slots];
		getSlotmap(slotmap, block_num);

		union Attribute A[num_attrs];
		int iter;
		for (iter = 0; iter < num_slots; iter++) {
			if (slotmap[iter] == '1') {
				getRecord(A, block_num, iter);
				char s[16];
				for (int l = 0; l < no_attrs; l++) {
					if (Attr_type[l] == FLOAT) {
						sprintf(s, "%f", A[l].fval);
						//cout<<s<<" "<<strlen(s)<<"\n";
						fputs(s, fp_export);
					}
					if (Attr_type[l] == INT) {
						sprintf(s, "%lld", A[l].ival);
						//cout<<s<<" "<<strlen(s)<<"\n";
						fputs(s, fp_export);
					}
					if (Attr_type[l] == STRING) {
						fputs(A[l].sval, fp_export);
					}
					if (l != no_attrs - 1)
						fputs(",", fp_export);
				}
				fputs("\n", fp_export);
			}
		}
		block_num = next_block_num;
	}
	fclose(fp_export);
	return SUCCESS;
}

void dump_attrcat() {
	FILE *fp_export = fopen("attribute_catalog", "w");
	union Attribute attr[6];
	int attr_blk = 5;
	char Attr_name[6][16];
	int j = 0;
	struct HeadInfo h;
	char s[16];
	while (attr_blk != -1) {
		h = getheader(attr_blk);
		sprintf(s, "%d", h.blockType);
		fputs(s, fp_export);
		fputs(",", fp_export);
		sprintf(s, "%d", h.pblock);
		fputs(s, fp_export);
		fputs(",", fp_export);
		sprintf(s, "%d", h.lblock);
		fputs(s, fp_export);
		fputs(",", fp_export);
		sprintf(s, "%d", h.rblock);
		fputs(s, fp_export);
		fputs(",", fp_export);
		sprintf(s, "%d", h.numEntries);
		fputs(s, fp_export);
		fputs(",", fp_export);
		sprintf(s, "%d", h.numAttrs);
		fputs(s, fp_export);
		fputs(",", fp_export);
		sprintf(s, "%d", h.numSlots);
		fputs(s, fp_export);
		fputs(",", fp_export);


		unsigned char slotmap[h.numSlots];
		getSlotmap(slotmap, attr_blk);
		for (int k = 0; k < 20; k++) {
			unsigned char ch = slotmap[k];
			//ch[0]=slotmap[k];
			fputc(ch, fp_export);
		}
		fputs("\n", fp_export);

		for (int i = 0; i < 20; i++) {

			getRecord(attr, attr_blk, i);
			if ((char) slotmap[i] == '0') {
				strcpy(attr[0].sval, "NULL");
				strcpy(attr[1].sval, "NULL");
			}
			fputs(attr[0].sval, fp_export);
			fputs(",", fp_export);
			fputs(attr[1].sval, fp_export);
			fputs(",", fp_export);
			sprintf(s, "%d", attr[2].ival);
			fputs(s, fp_export);
			fputs(",", fp_export);
			sprintf(s, "%d", attr[3].ival);
			fputs(s, fp_export);
			fputs(",", fp_export);
			sprintf(s, "%d", attr[4].ival);
			fputs(s, fp_export);
			fputs(",", fp_export);
			sprintf(s, "%d", attr[5].ival);
			fputs(s, fp_export);
			fputs("\n", fp_export);
		}
		attr_blk = h.rblock;
	}
	fclose(fp_export);
}

void dump_relcat() {
	FILE *fp_export = fopen("relation_catalog", "w");
	union Attribute attr[6];
	int attr_blk = 4;
	char Attr_name[6][16];
	int j = 0;
	struct HeadInfo h;
	char s[16];

	h = getheader(attr_blk);
	sprintf(s, "%d", h.blockType);
	fputs(s, fp_export);
	fputs(",", fp_export);
	sprintf(s, "%d", h.pblock);
	fputs(s, fp_export);
	fputs(",", fp_export);
	sprintf(s, "%d", h.lblock);
	fputs(s, fp_export);
	fputs(",", fp_export);
	sprintf(s, "%d", h.rblock);
	fputs(s, fp_export);
	fputs(",", fp_export);
	sprintf(s, "%d", h.numEntries);
	fputs(s, fp_export);
	fputs(",", fp_export);
	sprintf(s, "%d", h.numAttrs);
	fputs(s, fp_export);
	fputs(",", fp_export);
	sprintf(s, "%d", h.numSlots);
	fputs(s, fp_export);
	fputs(",", fp_export);


	unsigned char slotmap[h.numSlots];
	getSlotmap(slotmap, attr_blk);
	for (int k = 0; k < 20; k++) {
		unsigned char ch = slotmap[k];
		//ch[0]=slotmap[k];
		fputc(ch, fp_export);
	}
	fputs("\n", fp_export);

	for (int i = 0; i < 20; i++) {
		getRecord(attr, attr_blk, i);
		if ((char) slotmap[i] == '0')
			strcpy(attr[0].sval, "NULL");
		fputs(attr[0].sval, fp_export);
		fputs(",", fp_export);
		sprintf(s, "%d", attr[1].ival);
		fputs(s, fp_export);
		fputs(",", fp_export);
		sprintf(s, "%d", attr[2].ival);
		fputs(s, fp_export);
		fputs(",", fp_export);
		sprintf(s, "%d", attr[3].ival);
		fputs(s, fp_export);
		fputs(",", fp_export);
		sprintf(s, "%d", attr[4].ival);
		fputs(s, fp_export);
		fputs(",", fp_export);
		sprintf(s, "%d", attr[5].ival);
		fputs(s, fp_export);
		fputs("\n", fp_export);
	}
	fclose(fp_export);
}

void ls() {
	union Attribute attr[6];
	int attr_blk = 4;
	struct HeadInfo h;
	h = getheader(attr_blk);
	unsigned char slotmap[h.numSlots];
	getSlotmap(slotmap, attr_blk);
	for (int i = 0; i < 20; i++) {
		getRecord(attr, attr_blk, i);
		if ((char) slotmap[i] == '1')
			cout << attr[0].sval << "\n";
	}
	cout << "\n";
}

void db(int bno, char *filename) {
	FILE *fp_export = fopen(filename, "w");
	struct HeadInfo h;
	h = getheader(bno);
	if (h.blockType == REC) {
		char s[16];
		sprintf(s, "%d", h.blockType);
		fputs(s, fp_export);
		fputs(",", fp_export);
		sprintf(s, "%d", h.pblock);
		fputs(s, fp_export);
		fputs(",", fp_export);
		sprintf(s, "%d", h.lblock);
		fputs(s, fp_export);
		fputs(",", fp_export);
		sprintf(s, "%d", h.rblock);
		fputs(s, fp_export);
		fputs(",", fp_export);
		sprintf(s, "%d", h.numEntries);
		fputs(s, fp_export);
		fputs(",", fp_export);
		sprintf(s, "%d", h.numAttrs);
		fputs(s, fp_export);
		fputs(",", fp_export);
		sprintf(s, "%d", h.numSlots);
		fputs(s, fp_export);
		fputs(",", fp_export);

		unsigned char slotmap[h.numSlots];
		getSlotmap(slotmap, bno);
		for (int k = 0; k < h.numSlots; k++) {
			char ch[2];
			ch[0] = slotmap[k];
			fputs(ch, fp_export);
		}
		union Attribute attr[h.numAttrs];
		for (int i = 0; i < h.numEntries; i++) {
			getRecord(attr, bno, i);
			/*     if((char)slotmap[i]=='0')
				strcpy(attr[0].sval,"NULL");
				 fputs(attr[0].sval,fp_export);
				 fputs(",",fp_export);
					   sprintf(s, "%d",attr[1].ival);
					   fputs(s,fp_export);
				 fputs(",",fp_export);
							sprintf(s, "%d",attr[2].ival);
					   fputs(s,fp_export);
				 fputs(",",fp_export);
				 sprintf(s, "%d",attr[3].ival);
					   fputs(s,fp_export);
				 fputs(",",fp_export);
				 sprintf(s, "%d",attr[4].ival);
					   fputs(s,fp_export);
				 fputs(",",fp_export);
				 sprintf(s, "%d",attr[5].ival);
					   fputs(s,fp_export);
				 fputs("\n",fp_export);*/
		}
	}
}

void db() {
	FILE *disk = fopen("disk", "rb+");
	fseek(disk, 0, SEEK_SET);
	unsigned char blockAllocationMap[4 * BLOCK_SIZE];
	fread(blockAllocationMap, 4 * BLOCK_SIZE, 1, disk);
	int iter;
	fclose(disk);
	char s[16];
	FILE *fp_export = fopen("block_allocation_map", "w");
	for (iter = 0; iter < 4; iter++) {
		fputs("Block ", fp_export);
		sprintf(s, "%d", iter);
		fputs(s, fp_export);
		fputs(": Block Allocation Map\n", fp_export);
	}
	for (iter = 4; iter < 8192; iter++) {
		fputs("Block ", fp_export);
		sprintf(s, "%d", iter);
		fputs(s, fp_export);
		if ((int32_t) (blockAllocationMap[iter]) == UNUSED_BLK) {
			fputs(": Unused Block\n", fp_export);
		}
		if ((int32_t) (blockAllocationMap[iter]) == REC) {
			fputs(": Record Block\n", fp_export);
		}
		if ((int32_t) (blockAllocationMap[iter]) == IND_INTERNAL) {
			fputs(": Internal Index Block\n", fp_export);
		}
		if ((int32_t) (blockAllocationMap[iter]) == IND_LEAF) {
			fputs(": Leaf Index Block\n", fp_export);
		}
	}

	fclose(fp_export);
}

/*int main()
{


openRel("rel1");
select("rel1","rel11","CGPA",LE,"9.6");
exp("rel11","rel_11");
ls();
char attrs[3][ATTR_SIZE];
strcpy(attrs[0],"NO");
strcpy(attrs[1],"PHNO");
strcpy(attrs[2],"PLACE");
openRel("rel11");
project("rel11","rel12",3,attrs);
ls();
exp("rel12","rel_12");
int id=getRelId("rel11");
union Attribute b[6];
getRelCatEntry(id,b);
cout<<"first_blk is "<<b[3].ival<<endl;
unsigned char slotmap[b[5].ival];
getSlotmap(slotmap,8);
for(int i=0;i<b[5].ival;i++)
	cout<<slotmap[i];
cout<<id<<endl;
import("join1.csv");
import("join2.csv");
ls();
openRel("join1");
openRel("join2");
int flag=join("join1","join2","join3","ID","ID");
cout<<flag<<endl;
ls();
exp("join3","rel_join3");

*/
/*import("sample.csv");

int relid=openRel("sample");
bplus_create(relid,"MARK");
dropindex(relid,"MARK");
}*/


