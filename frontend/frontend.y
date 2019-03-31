%{
	#include<iostream>
	#include<cstdlib>
	#include "frontend.h"
	#include "frontend.cpp"
	#define YYSTYPE fnode *
	int yyerror(char const *);
	int yylex(void);
%}

%token SELECT INTO FROM WHERE PROJECT INTTYPE FLOATTYPE STRINGTYPE CREATE DROP TABLE INDEX ALTER RENAME TO JOIN ID EXIT EQTK LTTK GTTK LETK GETK NETK COLUMN AND INSERT VALUES

%%

Input : Commands EXIT { }
 		| EXIT { }
 		;

Commands : Commands Command { }
 		 | Command { }
 		 ;

Command : ddlCommand { }
		| dmlCommand { }
		;

ddlCommand : CREATE TABLE ID '(' ColList ')' ';' { fe_createrel($3, $5); freeall($3); freeall($5); }
			| DROP TABLE ID ';' { fe_droprel($3); freeall($3); }
			| ALTER TABLE ID RENAME TO ID ';' { fe_renamerel($3, $6); freeall($3), freeall($6); }
			| ALTER TABLE ID RENAME COLUMN ID TO ID ';' { fe_renamecol($3, $6, $8); freeall($3); freeall($6); freeall($8); }
			| CREATE INDEX ON ID '.' ID ';' { fe_createind($4, $6); freeall($4); freeall($6); }
			| DROP INDEX ON ID '.' ID ';' { fe_dropind($4, $6); freeall($4); freeall($6); }
			;

ColList : ColList ',' Column { $$=setright($1, $3); }
		| Column { $$=$1; }
		;

Column : ID INTTYPE { $$=$1; $$->ival = INT; }
	   | ID FLOATTYPE { $$=$1; $$->ival = FLOAT; } 
	   | ID STRINGTYPE { $$=$1; $$->ival = STRING; }
	   ;

dmlCommand : SELECT '*' INTO ID FROM ID WHERE CondList ';' { fe_select(NULL, $4, $6, $8); freeall($4); freeall($6); freeall($8); }
		   | SELECT AttrList INTO ID FROM ID WHERE CondList ';' { fe_select($2, $4, $6, $8); freeall($2); freeall($4); freeall($6); freeall($8); }
		   | SELECT AttrList INTO ID FROM ID ';' { fe_select($2, $4, $6, NULL); freeall($4); freeall($6); freeall($2); }
		   | SELECT '*' INTO ID FROM ID ';' { cout<<"Currently not supported"<<endl; }
		   | SELECT '*' INTO ID FROM ID JOIN ID WHERE ID '=' ID ';' { }
		   | SELECT AttrList INTO ID FROM ID JOIN ID WHERE ID '=' ID ';' { }
		   | INSERT INTO ID VALUES RecList ';' { }
		   ;

AttrList : IdList { $$=$1; };

CondList : CondList AND Condition { $$=setnext($1, $3);  }
		 | Condition { $$=$1; }
		 ;

Condition : ID EQTK ID { $$=setright($1, $3);  $$->ival = EQ; }
		  | ID LETK ID { $$=setright($1, $3);  $$->ival = LE; }
		  | ID GETK ID { $$=setright($1, $3);  $$->ival = GE; }
		  | ID LTTK ID { $$=setright($1, $3);  $$->ival = LT; }
		  | ID GTTK ID { $$=setright($1, $3);  $$->ival = GT; }
		  | ID NETK ID { $$=setright($1, $3);  $$->ival = NE; }
		  ;

RecList : RecList ',' Record { setnext($1, $3); $$=$1; }
		| Record { $$=$1 }
		;

Record : '(' IdList ')' { $$=$2; };

IdList : IdList ',' ID { $$=setright($1, $3); }
	   | ID { $$=$1 }
	   ;


%%

int yyerror(char const *s){
	cout<<s<<endl;
	return 1;
}