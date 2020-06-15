#include<bits/stdc++.h>
#include "interface.h"
#include "imp-exp.cpp"
using namespace std;
void string_to_char(string x, char *a,int size)
{
	int i;
	if(size==15)
	{
		for(i=0;i<x.size()&&i<15;i++)
			a[i]=x[i];
		a[i]='\0';
	}
	else
	{
		for(i=0;i<size;i++)
		{
			a[i]=x[i];
		}
		a[i]='\0';
	}
	
}
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
bool check_filenamelength(string filename)
{
	int pos1= filename.rfind('.');
	if(filename.substr(pos1+1,filename.length())=="csv")
	{
		//cout<<";;;;";
		string file_name=filename.substr(0,pos1);
		if(file_name.length()>15)
		{	
			cout<<" File name should have at most 15 characters\n";
			return false;
		}
		else
			return true;
	}
	else
		return false;
}

int main()
{
	while(1)	
	{
		cout<<"# ";
		string input_command;
		smatch m;
		//cin.ignore();
		getline(cin,input_command);
		if(regex_match(input_command,ex))
		{
			exit(0);
		}
		else if(regex_match(input_command,imp))
		{
			//cout<<"Matched"<<endl;
			string Filepath;
			regex_search(input_command,m,filepath);
			Filepath=m[0];
			char f[Filepath.length()+1];
			string_to_char(Filepath,f,Filepath.length());
			FILE * file=fopen(f,"r");
			if(!file)
			{
				cout<<"Invalid file path or file does not exist"<<endl;
				continue;
			}
			fclose(file);
			regex_search(input_command,m,filename);
			string Filename=m[0];
			//cout<<Filename<<endl;
			bool r=check_filenamelength(Filename);
			if(!r)
			{
				continue;
			}
			//cout<<"here"<<endl;
			int ret=import(f);
			if(ret==SUCCESS)
				cout<<" Imported successfully"<<endl;
			continue;
		}
			
		else if(regex_match(input_command,expo))
		{
			////cout<<"Matched export"<<endl;
			regex_search(input_command,m,expo);
			
			string table_name=m[2];
			string Filepath;
			regex_search(input_command,m,filepath);
			Filepath=m[0];
			char relname[16];
			string_to_char(table_name,relname,15);
			char f[Filepath.length()+1];
			string_to_char(Filepath,f,Filepath.length());
			//cout<<relname<<endl;
			int ret= exp(relname,f);
			if(ret==SUCCESS)
				cout<<"Exported successfully"<<endl;
			continue;
	
		}
		else if(regex_match(input_command,fdisk))
		{
			createdisk();
			formatdisk();
			meta();
			cout<<"Disk formatted"<<endl;
			continue;
			//dump_relcat();
		}
		else if(regex_match(input_command,dump_rel))
		{
			dump_relcat();
			continue;
		}
		else if(regex_match(input_command,dump_attr))
		{
			dump_attrcat();
			continue;
		}
		else if(regex_match(input_command,dump_bmap))
		{
			db();
			continue;
		}
		else if(regex_match(input_command,list_all))
		{
			ls();
			continue;
		}
		else if(regex_match(input_command,create_table))
		{
			//cout<<"Matched create table"<<endl;
			regex_search(input_command,m,create_table);
			//for(auto x:m)
				//cout<<x<<endl;
			string table_name=m[3];
			char relname[16];
			string_to_char(table_name,relname,15);
			regex_search(input_command,m,temp);
			string attrs=m[0];
			vector<string> words=strip_whitespace(attrs);

			/*for(int i=0;i<words.size();i++)
				cout<<words[i]<<endl;*/
			int no_attrs=words.size()/2;
			char attribute[no_attrs][16];
			int type_attr[no_attrs];
			for(int i=0,k=0;i<no_attrs;i++,k+=2)
			{
				string_to_char(words[k],attribute[i],15);
				if(words[k+1]=="STR")
					type_attr[i]=STRING;
				else if(words[k+1]=="INT")
					type_attr[i]=INT;
				else if(words[k+1]=="FLOAT")
					type_attr[i]=FLOAT;

			}
			int ret=createRel(relname,no_attrs,attribute,type_attr);
			if(ret==SUCCESS)
				cout<<"Relation created successfully"<<endl;
			continue;
			/*int i = 1; 
    		while (regex_search(input_command, m, temp1)) 
    		{ 
        		cout << "\nMatched string is " << m.str(0) << endl 
             			<< "and it is found at position " 
             	<< m.position(0)<<endl;  
        		i++; 
  
        		// suffix to find the rest of the string. 
        		input_command= m.suffix().str(); 
    		} */

			
		}
		else if(regex_match(input_command,open_table))
		{
			regex_search(input_command,m,open_table);
			string tablename=m[3];
			char relname[16];
			string_to_char(tablename,relname,15);
			int ret=openRel(relname);
			if(ret>=0&&ret<=11)
			 	 cout<<"Relation opened successfully\n";
			continue;
		}
		else if(regex_match(input_command,close_table))
		{
			regex_search(input_command,m,close_table);
			string tablename=m[3];
			char relname[16];
			string_to_char(tablename,relname,15);
			int id=getRelId(relname);
			if(id==E_RELNOTOPEN)
			{
				cout<<"Failed"<<endl;
				continue;
			}
			int ret=closeRel(id);
			
			if(ret==SUCCESS)
				cout<<"Relation closed successfully"<<endl;
			continue;
		}
		else if(regex_match(input_command,drop_table))
		{
			regex_search(input_command,m,drop_table);
			string tablename=m[3];
			char relname[16];
			string_to_char(tablename,relname,15);
			int ret=deleteRel(relname);
			if(ret==SUCCESS)
				cout<<"Relation deleted successfully"<<endl;
			continue;
		}
		else if(regex_match(input_command,create_index))
		{
			////cout<<"Matched"<<endl;
			regex_search(input_command,m,create_index);
			string tablename=m[4];
			string attrname=m[5];
			char relname[16],attr_name[16];
			string_to_char(tablename,relname,15);
			string_to_char(attrname,attr_name,15);
			int relid=getRelId(relname);
			if(relid==E_RELNOTOPEN)
			{
				cout<<"E_RELNOTOPEN\n";
				continue;
			}
			int ret=bplus_create(relid,attr_name);
			if(ret==SUCCESS)
				cout<<"Index created successfully\n";	
			continue;
		}
		else if(regex_match(input_command,drop_index))
		{
			////cout<<"Matched"<<endl;
			regex_search(input_command,m,drop_index);
			string tablename=m[4];
			string attrname=m[5];
			char relname[16],attr_name[16];
			string_to_char(tablename,relname,15);
			string_to_char(attrname,attr_name,15);
			int relid=getRelId(relname);
			if(relid==E_RELNOTOPEN)
			{
				cout<<"E_RELNOTOPEN";
				continue;
			}
			int ret=dropindex(relid,attr_name);
			if(ret==SUCCESS)
				//cout<<"Index created successfully";	
			continue;
		}
		else if(regex_match(input_command,insert_single))
		{
			//cout<<"Matched"<<endl;
			regex_search(input_command,m,insert_single);
			string tablename=m[3];
			char relname[16];
			string_to_char(tablename,relname,15);
			regex_search(input_command,m,temp);
			string t=m[0];
			vector<string> words=strip_whitespace(t);
			//for(int i=0;i<words.size();i++)
			//	cout<<words[i]<<endl;
			//cout<<relname<<endl;
			int ret=insert_val(words,relname);
			//cout<<ret<<endl;
			if(ret==SUCCESS)
			{
				cout<<"Inserted successfully"<<endl;
			}
			
			continue;
		}
		else if(regex_match(input_command,insert_multiple))
		{
			//cout<<"Matched"<<endl;
			regex_search(input_command,m,insert_multiple);
			string tablename=m[3];
			char relname[16];
			string_to_char(tablename,relname,15);
			regex_search(input_command,m,filepath);
			string t=m[0];
			char Filepath[t.length()+1];
			string_to_char(t,Filepath,t.length());
			FILE * file=fopen(Filepath,"r");
			if(!file)
			{
				cout<<"Invalid file path or file does not exist"<<endl;
				continue;
			}
			fclose(file);
			int ret=insert(relname,Filepath);
			if(ret==SUCCESS)
			{
				cout<<"Inserted successfully"<<endl;
			}
			
			continue;
		}
		else if(regex_match(input_command,rename_table))
		{
			//cout<<"Matched"<<endl;
			regex_search(input_command,m,rename_table);
			string oldtablename=m[4];
			string newtablename=m[6];
			char old_rel[16];
			char new_rel[16];
			string_to_char(oldtablename,old_rel,15);
			string_to_char(newtablename,new_rel,15);
			int ret=ba_renamerel(old_rel,new_rel);
			if(ret==SUCCESS)
			{
				cout<<"Renamed Relation Successfully"<<endl;
			}
			else
			{
				cout<<"Failed\n";
			}
			continue;

			
		}
		else if(regex_match(input_command,rename_column))
		{
			//cout<<"Matched"<<endl;
			regex_search(input_command,m,rename_column);
			string tablename=m[4];
			string oldcolumnname=m[6];
			string newcolumnname=m[8];
			char relname[16];
			char old_col[16];
			char new_col[16];
			string_to_char(tablename,relname,15);
			string_to_char(oldcolumnname,old_col,15);
			string_to_char(newcolumnname,new_col,15);
			int ret=ba_renameattr(relname,old_col,new_col);
			if(ret==SUCCESS)
			{
				cout<<"Renamed Attribute Successfully"<<endl;
			}
			else
			{
				cout<<"Failed\n";
			}
			continue;
			
		}
		else if(regex_match(input_command,select_from))
		{
			//cout<<"Matched select"<<endl;
			regex_search(input_command,m,select_from);
			string source=m[4];
			string target=m[6];
			char source_rel[16];
			char target_rel[16];
			string_to_char(source,source_rel,15);
			string_to_char(target,target_rel,15);
			int src_relid=getRelId(source_rel);
			if(src_relid==E_RELNOTOPEN)
			{
				cout<<"Source relation not open"<<endl;
				continue;
			}
			union Attribute relcatentry[6];
			if(getRelCatEntry(src_relid,relcatentry)!=SUCCESS)
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
		     		if(strcmp(attr[0].sval,source_rel)==0) 
		    		{
	              		strcpy(tar_attrs[attr[5].ival],attr[1].sval);
		     		} 
				}
				 attr_blk =h.rblock;
			}
			//---------------------------------------------------
			int ret=project(source_rel,target_rel,nAttrs,tar_attrs);
			if(ret==SUCCESS)
				cout<<"Select successful"<<endl;
			else
			{
			cout<<"Failed\n";
			}
			continue;
		}
		else if(regex_match(input_command,select_from_where))
		{
			//cout<<"Matched select where"<<endl;
			regex_search(input_command,m,select_from_where);
			string src=m[4];
			string tar=m[6];
			string attr=m[8];
			string o=m[9];
			string value=m[10];
			//cout<<value<<endl;
			char src_rel[16];
			string_to_char(src,src_rel,15);
			char tar_rel[16];
			string_to_char(tar,tar_rel,15);
			char attribute[16];
			string_to_char(attr,attribute,15);
			int op;
			if(o=="=")
			{
				op=EQ;
			}
			else if(o=="<")
			{
				op=LT;
			}
			else if(o=="<=")
			{
				op=LE;
			}
			else if(o==">")
			{
				op=GT;
			}
			else if(o==">=")
			{
				op=GE;
			}
			else if(o=="!=")
			{
				op=NE;
			}
			char val[16]; 
			string_to_char(value,val,15);
			int ret=select(src_rel,tar_rel,attribute,op,val);
			if(ret==SUCCESS)
			{
				cout<<"Select executed successfully"<<endl;
			}
			else
			{
				cout<<"Failed\n";
			}
			continue;

		}
		else if(regex_match(input_command,select_attr_from))
		{
			regex_search(input_command,m,select_attr_from);
			vector <string> y;
			for(auto x:m)
				y.push_back(x);
			int i;
			for(i=0;i<y.size();i++)
			{
				if(y[i]=="from"||y[i]=="FROM")
					break;
			}
			char src_rel[16];
			string_to_char(y[i+1],src_rel,15);
			char tar_rel[16];
			string_to_char(y[i+3],tar_rel,15);
			 i = 1; 
			string attribute_list;
    		while (regex_search(input_command, m,attrlist)) 
    		{ 

        		//cout << "\nMatched string is " << m.str(0) << endl << "and it is found at position " << m.position(0)<<endl;  
             	if(i==2)
             		attribute_list=m.str(0);
        		i++; 
  
        		// suffix to find the rest of the string. 
        		input_command= m.suffix().str(); 
    		} 
    		vector <string> words=strip_whitespace(attribute_list);
    		int count=words.size();
    		char attrs[count][16];
    		for(int i=0;i<count;i++)
    		{
    			string_to_char(words[i],attrs[i],15);
    		}

    		int ret=project(src_rel,tar_rel,count,attrs);
    		if(ret==SUCCESS)
    		{
    			cout<<"Command executed successfully"<<endl;
    		}
		else
		{
			cout<<"Failed\n";
		}
    		continue;

		}
		else if(regex_match(input_command,select_attr_from_where))
		{
			
			regex_search(input_command,m,select_attr_from_where);
			vector <string> y;
			for(auto x:m)
				y.push_back(x);
			int i;
			//for( i=0;i<y.size();i++)
			//	cout<<y[i]<<endl;
			//cout<<"!!!!"<<endl;
			/*regex_search(input_command,m,condition);
			for(auto x:m )
				cout<<x<<endl;
			string attr=m[1];
			string o=m[2];
			string value=m[3];
			cout<<value<<":::"<<endl;*/
			
		
			for(i=0;i<y.size();i++)
			{
				if(y[i]=="where"||y[i]=="WHERE")
					break;
			}
			string attr=y[i+1];
			string o=y[i+2];
			string value=y[i+3];
			char attribute[16];
			string_to_char(attr,attribute,15);
			int op;
			if(o=="=")
			{
				op=EQ;
			}
			else if(o=="<")
			{
				op=LT;
			}
			else if(o=="<=")
			{
				op=LE;
			}
			else if(o==">")
			{
				op=GT;
			}
			else if(o==">=")
			{
				op=GE;
			}
			else if(o=="!=")
			{
				op=NE;
			}
			char val[16];
			string_to_char(value,val,15);
			for(i=0;i<y.size();i++)
			{
				if(y[i]=="from"||y[i]=="FROM")
					break;
			}
			char src_rel[16];
			string_to_char(y[i+1],src_rel,15);
			char tar_rel[16];
			string_to_char(y[i+3],tar_rel,15);
			 i = 1; 
			string attribute_list;
			//cout<<"I am here\n";
    		while (regex_search(input_command, m,attrlist)) 
    		{ 

        		//cout << "\nMatched string is " << m.str(0) << endl 
             	//		<< "and it is found at position " 
             	//<< m.position(0)<<endl;  
             	if(i==2)
             		attribute_list=m.str(0);
        		i++; 
  
        		// suffix to find the rest of the string. 
        		input_command= m.suffix().str(); 
    		} 
    		vector <string> words=strip_whitespace(attribute_list);
    		int count=words.size();
    		char attrs[count][16];
    		for(int i=0;i<count;i++)
    		{
    			string_to_char(words[i],attrs[i],15);
			//cout<<attrs[i]<<"\n";
    		}
    		//cout<<attribute<<op<<val;
    		int ret=select(src_rel,"temp",attribute,op,val);
		//cout<<ret;
    		//expo("temp","/home/athira/Desktop/Physical-Layer-master/disk/xfs_interface_new/temp.csv");
    		if(ret==SUCCESS)
    		{
			//cout<<"I am here\n";
    			int relid=openRel("temp");
    			if(relid!=E_RELNOTEXIST&& relid!=E_CACHEFULL)
    			{
				
    				int ret_project=project("temp",tar_rel,count,attrs);
				//cout<<ret_project;
    				if(ret_project==SUCCESS)
    				{
					//cout<<"I am here\n";
    					cout<<"Command executed successfully"<<endl;
    					closeRel(relid);
    					deleteRel("temp");
    					continue;
    				}
    				else
    				{
					closeRel(relid);
    					deleteRel("temp");
    					cout<<"Command failed to execute"<<endl;
    					continue;
    				}
    			}
    			else
    			{
    				cout<<"Command failed to execute"<<endl;
    				continue;
    			}
	

    		}
		else
		{
			cout<<"Failed\n";
		}
    		continue;

		}
		else if(regex_match(input_command,select_from_join))
		{
			regex_search(input_command,m,select_from_join);
			//check if rel names are same everywhere
			if(m[4]!=m[10]||m[6]!=m[12])
			{
				cout<<"Syntax Error"<<endl;
				continue;
			}
			char src_rel1[16];
			char src_rel2[16];
			char tar_rel[16];
			char attr1[16];
			char attr2[16];

			string_to_char(m[4],src_rel1,15);
			string_to_char(m[6],src_rel2,15);
			string_to_char(m[8],tar_rel,15);
			string_to_char(m[11],attr1,15);
			string_to_char(m[13],attr2,15);
		    int ret=join(src_rel1,src_rel2,tar_rel,attr1,attr2);
			if(ret==SUCCESS)
			{
				cout<<"Join successful"<<endl;
				continue;
			}
			
			else
			{
				cout<<"Join Failed"<<endl;
				continue;
			}
			//m[4]<<" "<<m[6]<<" "<<m[8]<<" "<<m[10]<<" "<<m[11]<<" "<<m[12]<<" "<<m[13]<<endl;
			
		}
		else if(regex_match(input_command,select_attr_from_join))
		{
			regex_search(input_command,m,select_attr_from_join);
			vector <string> y;
			for(auto x:m)
				y.push_back(x);

			int i;
			for(i=0;i<y.size();i++)
			{
				//cout<<y[i]<<endl;
			}
			for(i=0;i<y.size();i++)
			{
				if(y[i]=="from"||y[i]=="FROM")
					break;
			}
			
			char src_rel1[16];
			char src_rel2[16];
			char tar_rel[16];
			char attr1[16];
			char attr2[16];

			string_to_char(y[i+1],src_rel1,15);
			string_to_char(y[i+3],src_rel2,15);
			string_to_char(y[i+5],tar_rel,15);
			string_to_char(y[i+8],attr1,15);
			string_to_char(y[i+10],attr2,15);
			int ret=join(src_rel1,src_rel2,"temp",attr1,attr2);
			int relid;
			if(ret==SUCCESS)
			{	
				relid=openRel("temp");
				if(!(relid>=0&&relid<MAXOPEN))
				{
					cout<<"Join Failed"<<endl;
				}
				i = 1; 
				string attribute_list;
    			while (regex_search(input_command, m,attrlist)) 
    			{ 

        			//cout << "\nMatched string is " << m.str(0) << endl 
             	//			<< "and it is found at position " 
             	//	<< m.position(0)<<endl;  
             		if(i==2)
             			attribute_list=m.str(0);
        			i++; 
  
        		// suffix to find the rest of the string. 
        			input_command= m.suffix().str(); 
    			} 
    			vector <string> words=strip_whitespace(attribute_list);
    			int count=words.size();
    			char attrs[count][16];
    			for(int i=0;i<count;i++)
    			{
    				string_to_char(words[i],attrs[i],15);
    			}


				int ret_project=project("temp",tar_rel,count,attrs);
				if(ret_project==SUCCESS)
				{
					cout<<"Join successful"<<endl;
					closeRel(relid);
					deleteRel("temp");
					continue;
				}
				
			}
			
			else
			{
				cout<<"Join Failed"<<endl;
				continue;
			}
			//m[4]<<" "<<m[6]<<" "<<m[8]<<" "<<m[10]<<" "<<m[11]<<" "<<m[12]<<" "<<m[13]<<endl;
			
		}
		
		
		
		else
		{
			cout<<"Syntax Error"<<endl;
		}






	}


	
	
	



	/*smatch m;
	string s;
	//getline(cin,s);
	s=" export Students /home/athira/s.csv ";
	if(regex_match(s,expo))
	{
		cout<<"Join matched";
	}*/
	
	/*smatch m; 
	string s;
	regex token("([A-Za-z0-9]+)");
	getline(cin,s);
	if(regex_match(s,r))
	{
		regex_search(s, m,r);
		for (auto x : m) 
		{
       	 		cout << x<<endl; 
		}
		/*regex_search(s, m, r1);
		for (auto x : m) 
		{
			if(x!=" "&&x.length()!=s.length())
       	 		cout << x<<endl; 
		}
		cout<<"yes"<<endl;
	}*/
	return 0;
}
