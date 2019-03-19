%{
	#include<iostream>
	#include<cstdlib>
%}

%token SELECT INTO FROM WHERE PROJECT INTTYPE FLOATTYPE STRINGTYPE CREATE DROP TABLE INDEX ALTER RENAME TO JOIN ID EXIT EQTK LTTK GTTK LETK GETK NETK COLUMN

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

ddlCommand : CREATE TABLE ID '(' ColList ')' ';' { }
			| DROP TABLE ID ';' { }
			| ALTER TABLE ID RENAME TO ID ';' { }
			| ALTER TABLE ID RENAME COLUMN ID TO ID ';' { }
			;

ColList : ColList ',' Column { }
		| Column { }
		;

Column : ID ID { };

dmlCommand : SELECT '*' INTO ID FROM ID WHERE CondList ';' { }
		   | SELECT AttrList INTO ID FROM ID WHERE CondList ';' { }
		   | SELECT AttrList INTO ID FROM ID ';' { }
		   | INSERT INTO ID VALUES RecList ';' { }
		   ;

CondList : CondList AND Condition { }
		 | Condition { }
		 ;

Condition : ID EQTK ID { }
		  | ID LETK ID { }
		  | ID GETK ID { }
		  | ID LTTK ID { }
		  | ID GTTK ID { }
		  | ID NETK ID { }
		  ;

RecList : RecList ',' Record { }
		| Record { }
		;

Record : '(' IdList ')' { };

IdList | IdList ',' ID { }
	   | ID { }
	   ;

