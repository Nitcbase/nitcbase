// Code for XFS Interface (for Unix) 
// To Do:  Students cannot name their relations "temp"

#include<bits/stdc++.h>
#include "imp-exp.cpp"
using namespace std;

vector <string> strip_whitespace(string input_command)
{
	vector <string> words;
	string temp="";
	for(int i=0;i<input_command.length();i++)
	{
		if(input_command[i]=='('||input_command[i]==')')
		{
			if(temp!="")
			{
			words.push_back(temp);
			}
			temp="";
		}
		else if(input_command[i]==',')
		{
			if(temp!="")
			{
			words.push_back(temp);
			}
			temp="";
		}
		else if(input_command[i]==' '||input_command[i]==';')
		{
			if(temp!="")
			{
			words.push_back(temp);
			}
			temp="";
		}
		else
		{
			temp=temp+input_command[i];
		}

	}
	if(temp!="")
		words.push_back(temp);
	for(auto i= words.begin();i!=words.end();i++)
	{
		
		//cout<<*i<<endl;
	}
	return words;
}

bool check_filenamelength(string filepath)
{
	int pos1= filepath.rfind('.');
	int pos2=filepath.rfind('/');
	int diff=pos2-pos1+1;
	if(diff>15)
	{
		cout<<" File name should have at most 15 characters\n";
		return false;
	}
	else 
		return true;

}


int main()
{
	//import("sample.csv");
	while(1)
	{
		cout<<"# ";
		string input_command;
		//cin.ignore();
		getline(cin,input_command);
		vector <string> s= strip_whitespace(input_command);
		string first=s[0];
		//cout<<first<<endl;
		if(first=="EXIT"||first=="exit")
		{
			if(s.size()==1)
				exit(0);
			else
			{
				cout<<"Syntax Error"<<endl;
				continue;
			}
		}	
		else if(first=="ls"||first=="LS")
		{
			ls();
		}
		else if(first=="fdisk")
		{
			if(s.size()==1)
			{
				//cout<<"fdisk"<<endl;
				createdisk();
				formatdisk();
				meta();
			}
			else
			{
				cout<<"Syntax Error"<<endl;
				continue;
			}
			
		}
		else if(first=="import" )
		{
			if(s.size()!=2)
			{
				cout<<"Incorrect number of arguements"<<endl;
			}
			string filepath = s[1];
			char f[filepath.length()];
			int i=0;
			for(i=0;i<filepath.length();i++)
			{
				f[i]=filepath[i];
			}
			f[i]='\0';
			FILE * file=fopen(f,"r");
			if(!file)
			{
				cout<<"Invalid file path or file does not exist"<<endl;
				continue;
			}
			fclose(file);
			bool check= check_filenamelength(filepath);
			if(!check)
			{
				continue;
			}
			cout<<" Calling import"<<endl;
			cout<<f<<endl;
			import(f);

		}
		else if(first=="export")
		{
			// If file name is not given, export to a file with name same as that of relation
			if(s.size()==3)
			{
				char relname[16];
				int i;
				string second=s[1];
				for(i=0;i<15;i++)
					relname[i]=second[i];
				relname[i]='\0';
				string third=s[2];
				char filepath[third.length()];
				for(i=0;i<third.length();i++)
					filepath[i]=third[i];
				filepath[i]='\0';
				exp(relname,filepath);
			}
			else
			{
				cout<<"Incorrect number of arguments"<<endl;
				continue;
			}

		}
		else if(first=="dump")
		{
			if(s.size()!=2)
			{
				cout<<"Syntax Error"<<endl;
				continue;
			}
			string second=s[1];
			if(second=="bmap")
			{
				db();
			}
			else if(second=="relcat")
			{
				dump_relcat();
			}
			else if(second=="attrcat")
			{
				dump_attrcat();
			}
		}
		else if(first=="create"||first=="CREATE")
		{
			string second=s[1];
			if(second=="TABLE"||second=="table")
			{
				string third=s[2];
				char tablename[16];
				int i;
				for(i=0;i<15;i++)
					tablename[i]=third[i];
				int no_attrs=(s.size()-3)/2;
				char attribute[no_attrs][16];
				int type_attr[no_attrs];
				int k;
				for(k=0,i=3;i<s.size();i++,k++)
				{
					for(int j=0;j<15;j++)
						attribute[k][j]=s[i][j];
					i++;
					if(s[i]=="INT" || s[i]=="int")
						type_attr[k]=INT;
					else if(s[i]=="FLOAT" || s[i]=="float")
						type_attr[k]=FLOAT;
					else if(s[i]=="STRING" || s[i]=="string"|| s[i]=="str"|| s[i]=="STR")
						type_attr[k]=STRING;
					else
					{     
						cout<<"Syntax Error"<<endl;
						continue;
					}
					
				}
				createRel(tablename,no_attrs,attribute,type_attr);	
			}
			if(second=="INDEX"||second=="index")
			{
				string third=s[2];
				if(third!="ON")
				{	
					cout<<"Syntax Error"<<endl;
					continue;
				}
				else
				{
					string t=s[3];
					//cout<<t<<"\n";
					char tablename[16],attrname[16];
					int i;
					for(i=0;t[i]!='.';i++)
						tablename[i]=t[i];
					tablename[i++]='\0';
					int k;
					for(k=0;i<t.size();i++,k++)
					{
						attrname[k]=t[i];
					}
					attrname[k]='\0';
					int relid=openRel(tablename);
					//cout<<relid<<" "<<attrname<<"\n";
					bplus_create(relid,attrname);	
				  }
			}
		}
		else if(first=="drop"||first=="DROP")
		{
			string second=s[1];
			if(second=="TABLE"||second=="table")
			{
				string third=s[2];
				char tablename[16];
				int i;
				for(i=0;i<15;i++)
					tablename[i]=third[i];
				ba_delete(tablename);	
			}
			else if(second=="INDEX"||second=="index")
			{
				string third=s[2];
				if(third!="ON")
				{	
					cout<<"Syntax Error"<<endl;
					continue;
				}
				else
				{
					string t=s[3];
					//cout<<t<<"\n";
					char tablename[16],attrname[16];
					int i;
					for(i=0;t[i]!='.'&&i<t.size();i++)
						tablename[i]=t[i];
					if(i==t.size())
					{
						cout<<"Syntax Error!!!\n";
						continue;
					}
					tablename[i++]='\0';
					int k;
					for(k=0;i<t.size();i++,k++)
					{
						attrname[k]=t[i];
					}
					attrname[k]='\0';
					int relid=openRel(tablename);
					//cout<<relid<<" "<<attrname<<"\n";
					int ret=dropindex(relid,attrname);
					if(ret!=SUCCESS)
						cout<<"FAILED!!!\n";	
				  }
			}
			else
			{
				cout<<"Syntax Error\n";
			}
		}
		else if(first=="open"||first=="OPEN")
		{
			string second=s[1];
			if(second=="TABLE"||second=="table")
			{
				string third=s[2];
				char tablename[16];
				int i;
				for(i=0;i<15;i++)
					tablename[i]=third[i];
				tablename[i]='\0';
				int x=openRel(tablename);
				if(x==E_RELNOTEXIST)
				{
					cout<<"E_RELNOTEXIST - Relation does not exist"<<endl;
				}
				else if(x==E_CACHEFULL)
				{
					cout<<"E_CACHEFULL - Relation Cache is full"<<endl;
				}
				else
				{
					cout<<" Relation opened successfully"<<endl;
				}

			}
			else
			{
				cout<<"Syntax Error"<<endl;
				continue;
			}
		}
		else if(first=="close"||first=="CLOSE")
		{
			string second=s[1];
			if(second=="TABLE"||second=="table")
			{
				string third=s[2];
				char tablename[16];
				int i;
				for(i=0;i<15;i++)
					tablename[i]=third[i];
				tablename[i]='\0';
				int id=getRelId(tablename);
				if(id==E_RELNOTOPEN)
				{
					cout<<"E_RELNOTOPEN- Relation not open"<<endl;
					continue;
				}
				int x=closeRel(id);
				if(x==E_OUTOFBOUND)
				{
					cout<<"E_OUTOFBOUND - Relation ID is out of bound"<<endl;
				}
				else if(x==E_RELNOTOPEN)
				{
					cout<<"E_RELNOTOPEN- Relation not open"<<endl;
				}
				else
				{
					cout<<" Relation closed successfully"<<endl;
				}

			}
			else
			{
				cout<<"Syntax Error"<<endl;
				
			}
		}
		else if(first=="select"||first=="SELECT")
		{
			
			string second=s[1];
			if(second=="*")
			{
				if(s.size()==7) //SELECT * FROM TABLE
				{
					string x=s[4];
					char source_table[16];
					int i;
					for(i=0;i<15;i++)
						source_table[i]=x[i];
					source_table[i]='\0';
					x=s[6];
					char tar_table[16];
					for(i=0;i<15;i++)
						tar_table[i]=x[i];
					tar_table[i]='\0';
					
					//-----------------------------------------
					int relid=openRel(source_table);
					if(relid==E_CACHEFULL||relid==E_RELNOTEXIST)
					{
						cout<<"SELECT NOT POSSIBLE\n";
						continue;
					}
   					union Attribute relcatentry[6];
					if(getRelCatEntry(relid,relcatentry)!=SUCCESS)
					{
						cout<<"SELECT NOT POSSIBLE\n";
       						continue;
					}
					int nAttrs;
					nAttrs = relcatentry[1].ival;
					char tar_attrs[nAttrs][ATTR_SIZE];
					union Attribute attr[6];
					int attr_blk=5;
					struct HeadInfo h;
					while(attr_blk != -1)
					{
						h=getheader(attr_blk);
						unsigned char slotmap[h.numSlots];
						getSlotmap(slotmap,attr_blk);
						for(int i=0;i<20;i++)
      						{
        	    	   				  getRecord(attr,attr_blk,i);
		    				  if((char)slotmap[i]=='0')
		    		 	            {
							continue;
		    				 }
		     				 if(strcmp(attr[0].sval,source_table)==0) 
		    				 {
	              					strcpy(tar_attrs[attr[5].ival],attr[1].sval);
		     				 } 
					   }
					   attr_blk =h.rblock;
					}
					//---------------------------------------------------
					project(source_table,tar_table,nAttrs,tar_attrs);
				}
				else if(s.size()==11) //SELECT * FROM TABLE WHERE
				{
					string x=s[4];
					char source_table[16];
					int i;
					for(i=0;i<15;i++)
						source_table[i]=x[i];
					source_table[i]='\0';
					x=s[6];
					char tar_table[16];
					for(i=0;i<15;i++)
						tar_table[i]=x[i];
					tar_table[i]='\0';
					x=s[8];
					char attr_name[16];
					for(i=0;i<15;i++)
						attr_name[i]=x[i];
					attr_name[i]='\0';
					x=s[10];
					char attr_value[16];
					for(i=0;i<15;i++)
						attr_value[i]=x[i];
					attr_value[i]='\0';
					x=s[9];
					int op;
					if(x=="EQ"||x=="eq"||x=="="||x=="==")
					{
						op=EQ;
					}
					if(x=="LE"||x=="le"||x=="<=")
					{
						op=LE;
					}
					if(x=="GE"||x=="ge"||x==">=")
					{
						op=GE;
					}
					if(x=="LT"||x=="lt"||x=="<")
					{
						op=LT;
					}
					if(x=="GT"||x=="GT"||x==">")
					{
						op=GT;
					}
					if(x=="NE"||x=="ne"||x=="!=")
					{
						op=NE;
					}
					//------------------------------------------------------------------
					int relid=openRel(source_table);
					if(relid==E_CACHEFULL||relid==E_RELNOTEXIST)
					{
						cout<<"SELECT NOT POSSIBLE\n";
						continue;
					}
   					union Attribute relcatentry[6];
					if(getRelCatEntry(relid,relcatentry)!=SUCCESS)
					{
						cout<<"SELECT NOT POSSIBLE\n";
       						continue;
					}
					int nAttrs;
					nAttrs = relcatentry[1].ival;
					char tar_attrs[nAttrs][ATTR_SIZE];
					union Attribute attr[6];
					int attr_blk=5;
					struct HeadInfo h;
					while(attr_blk != -1)
					{
						h=getheader(attr_blk);
						unsigned char slotmap[h.numSlots];
						getSlotmap(slotmap,attr_blk);
						for(int i=0;i<20;i++)
      						{
        	    	   				  getRecord(attr,attr_blk,i);
		    				  if((char)slotmap[i]=='0')
		    		 	            {
							continue;
		    				 }
		     				 if(strcmp(attr[0].sval,source_table)==0) 
		    				 {
	              					strcpy(tar_attrs[attr[5].ival],attr[1].sval);
		     				 } 
					   }
					   attr_blk =h.rblock;
					}
					//---------------------------------------------------
					cout<<tar_table<<"\n"<<nAttrs<<"\n"<<op<<"\n"<<attr_value;
					select(source_table,tar_table,attr_name,op,attr_value);
					//project("temp",tar_table,nAttrs,tar_attrs);
					//ba_delete("temp");
				}
				else if(s.size()==12) //SELECT * FROM JOIN WHERE

				{
					string x=s[3];
					char source_table1[16];
					int i;
					for(i=0;i<15;i++)
						source_table1[i]=x[i];
					source_table1[i]='\0';
					x=s[5];
					char source_table2[16];
					for(i=0;i<15;i++)
						source_table2[i]=x[i];
					source_table2[i]='\0';
					x=s[7];
					char tar_table[16];
					for(i=0;i<15;i++)
						tar_table[i]=x[i];
					tar_table[i]='\0';
					x=s[9];
					char attr1[16];
					int j;
					for(i=0;x[i]!='.';i++)
						continue;
					for(i=i+1,j=0;i<x.size();i++,j++)
						attr1[j]=x[i];
					attr1[j]='\0';
					x=s[11];
					char attr2[16];
					for(i=0;x[i]!='.';i++)
						continue;
					for(i=i+1,j=0;i<x.size();i++,j++)
						attr2[j]=x[i];
					attr2[j]='\0';
					openRel(source_table1);
					openRel(source_table2);
					int ret=join(source_table1,source_table2,tar_table,attr1,attr2);
					if(ret==FAILURE)
					{
						cout<<"Join Failed\n";
					}
				}
				else
				{
					cout<<"INVALID SYNTAX\n";
					continue;
				}	
			}
			else
			{	
				int ctr=0;
				int i=1;
				for(;s[i]!="from"&&s[i]!="FROM";i++)
				{
					ctr++;
				}
				int from_pos=i;
				char attr_list[ctr][16];
				i=1;
				int k=0;
				for(;s[i]!="from"&&s[i]!="FROM"&&k<ctr;i++,k++)
				{
					
					int j;
					for( j=0;j<15;j++)
						attr_list[k][j]=s[i][j];
					attr_list[k][j]='\0';
					
				}
				int total_size=s.size();
				char src_rel[16];
				char tar_rel[16];
				if(total_size-from_pos==4)// Select Attrlist from table
				{
					string src=s[from_pos+1];
					//char src_rel[16];
					for(i=0;i<15;i++)
					{
						src_rel[i]=src[i];
					}
					src_rel[i]='\0';
					src=s[from_pos+3];
					//char tar_rel[16];
				
					for(i=0;i<15;i++)
					{
						tar_rel[i]=src[i];
					}
					tar_rel[i]='\0';
					//cout<<src_rel<<" "<<tar_rel<<endl;
					cout<<ctr<<endl;
					project(src_rel,tar_rel,ctr,attr_list);

				}
				else if(s[from_pos+4]=="WHERE"||s[from_pos+4]=="where")
				{
					char attrname[16];
					int i;
					for(i=0;i<15;i++)
					{
						attrname[i]=s[from_pos+5][i];
					}
					attrname[i]='\0';
					string x=s[from_pos+6];
					int op;
					if(x=="EQ"||x=="eq"||x=="="||x=="==")
					{
						op=EQ;
					}
					if(x=="LE"||x=="le"||x=="<=")
					{
						op=LE;
					}
					if(x=="GE"||x=="ge"||x==">=")
					{
						op=GE;
					}
					if(x=="LT"||x=="lt"||x=="<")
					{
						op=LT;
					}
					if(x=="GT"||x=="GT"||x==">")
					{
						op=GT;
					}
					if(x=="NE"||x=="ne"||x=="!=")
					{
						op=NE;
					}
					char attrval[16];
					x=s[from_pos+7];
					for(i=0;i<15;i++)
						attrval[i]=x[i];
					attrval[i]='\0';
					openRel(src_rel);
					select(src_rel,"temp",attrname,op,attrval);
					project("temp",tar_rel,ctr,attr_list);
					deleteRel("temp");

				}
				else if((s[from_pos+2]=="join")||(s[from_pos+2]=="JOIN"))
				{
					string src=s[from_pos+1];
					char src_rel1[16];
					for(i=0;i<15;i++)
					{
						src_rel1[i]=src[i];
					}
					src_rel1[i]='\0';
					src=s[from_pos+3];
					char src_rel2[16];
					for(i=0;i<15;i++)
					{
						src_rel2[i]=src[i];
					}
					src_rel2[i]='\0';
					if((s[from_pos+4]=="into"||s[from_pos+4]=="INTO")&&(s[from_pos+6]=="WHERE"||s[from_pos+6]=="where") &&s[from_pos+8]=="="&&(s.size()-from_pos==10))
					{
						string x=s[from_pos+5];
						char tar_rel[16];
						for(i=0;i<15;i++)
						{
							tar_rel[i]=x[i];
						}
						tar_rel[i]='\0';
						string attr1=s[from_pos+7];
						string attr2=s[from_pos+9];
						char attribute1[16];
						for(i=0;i<15;i++)
							attribute1[i]=attr1[i];
						attribute1[i]='\0';
						char attribute2[16];
						for(i=0;i<15;i++)
							attribute2[i]=attr2[i];
						attribute2[i]='\0';
						int relid1=openRel(src_rel1);
						if(relid1== E_RELNOTEXIST)
						{
							cout<<"Open Relation Failed : Relation does not exist"<<endl;
							continue;
						}
						else if(relid1==E_CACHEFULL)
						{
							cout<<"Open Relation Failed : Relation cache is full"<<endl;
							continue;
						}

						int relid2=openRel(src_rel2);
						if(relid2== E_RELNOTEXIST)
						{
							cout<<"Open Relation Failed : Relation does not exist"<<endl;
							continue;
						}
						else if(relid2==E_CACHEFULL)
						{
							cout<<"Open Relation Failed : Relation cache is full"<<endl;
							continue;
						}

						int ret=join(src_rel1,src_rel2,tar_rel,attribute1,attribute2);
						if(ret==E_RELNOTOPEN)
						{
							cout<<"Join Failed : At least one relation is not open"<<endl;
							continue;
						}
						else if(ret==E_ATTRNOTEXIST)
						{
							cout<<"Open Relation Failed : Relation cache is full"<<endl;
							continue;
						}
						else if(ret==E_ATTRTYPEMISMATCH)
						{
							cout<<"Open Relation Failed : Relation cache is full"<<endl;
							continue;
						}


					}
					else
					{
						cout<<"Syntax Error"<<endl;
						

					}

				}
				else
				{
					cout<<"Syntax Error"<<endl;
				}

				



			}	
		}
		else if(first=="insert"||first=="INSERT")
		{
			string third=s[3];
			char tablename[16];
			int i;
			for(i=0;i<15;i++)
				tablename[i]=third[i];
			tablename[i]='\0';
			string fifth=s[5];
			if(fifth=="FROM" || fifth=="from")
			{	
				string filepath = s[6];
				char f[filepath.length()];
				int i=0;
				for(i=0;i<filepath.length();i++)
				{
					f[i]=filepath[i];
				}
				f[i]='\0';
				FILE * file=fopen(f,"r");
				if(!file)
				{
					cout<<"Invalid file path or file does not exist"<<endl;
					continue;
				}
				fclose(file);
				insert(tablename,f);
			 }
			 else
			 {
				insert_val(s,tablename);
			 }
		}
		else if(first=="alter"||first=="ALTER")
		{
			if(s.size()==6)
			{
				if(s[1]=="TABLE"&& s[2]=="RENAME"&&s[4]=="TO")
				{
					string rel=s[3];
					string newrel=s[5];
					char oldrelname[16];
					char newrelname[16];
					int i;
					for(i=0;i<15;i++)
						oldrelname[i]=rel[i];
					oldrelname[i]='\0';
					for(i=0;i<15;i++)
						newrelname[i]=newrel[i];
					newrelname[i]='\0';
					int x=ba_renamerel(oldrelname,newrelname);
					if(x==E_RELEXIST)
					{
						cout<<"Relation with the name already exists"<<endl;
					}
					else if(x==E_RELNOTEXIST)
					{
						cout<<"Relation to be renamed does not exist"<<endl;
					}
					else if(x==SUCCESS)
					{
						cout<<"Renaming done successfully"<<endl;
					}
				}
				else
				{
					cout<<"Syntax Error"<<endl;
				}
			}
			else if(s.size()==8)
			{
				if(s[1]=="TABLE"&& s[2]=="RENAME"&&s[4]=="COLUMN"&&s[6]=="TO")
				{
					string rel=s[3];
					string col=s[5];
					string newcol=s[7];
					char relname[16];
					char oldcolname[16];
					char newcolname[16];
					int i;
					for(i=0;i<15;i++)
						relname[i]=rel[i];
					relname[i]='\0';
					for(i=0;i<15;i++)
						oldcolname[i]=col[i];
					oldcolname[i]='\0';
					for(i=0;i<15;i++)
						newcolname[i]=newcol[i];
					newcolname[i]='\0';
					int x=ba_renameattr(relname,oldcolname,newcolname);
					if(x==E_RELNOTEXIST)
					{
						cout<<"Relation does not exist"<<endl;
					}
					else if(x==E_ATTREXIST)
					{
						cout<<"Attribute with same name exists in the relation"<<endl;
					}
					else if(x==E_ATTRNOTEXIST)
					{
						cout<<"Attribute to be renamed does not exist"<<endl;
					}
					else if(x==SUCCESS)
					{
						cout<<"Renaming done successfully"<<endl;
					}
				}
				else
				{
					cout<<"Syntax Error"<<endl;
				}
			}
			else
			{
				cout<<"Syntax Error"<<endl;
			}
		}
		else
		{
			cout<<"Syntax Error. "<<endl;
		}



	}
	return 0;
}
