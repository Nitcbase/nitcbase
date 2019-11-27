#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int check_type(char *data)
{	
	int count_int=0,count_dot=0,count_string=0,i;
	for(i=0;data[i]!='\0';i++)
	{
		if(data[i]>='0'&&data[i]<='9')
			count_int++;
		if(data[i]=='.')
			count_dot++;
		else
			count_string++;
	}
	if(count_dot==1&&count_int==(strlen(data)-1))
		return 0;
	if(count_int==strlen(data))
		return 1;
	else
		return 2;
}
void parse()
{
     FILE *file=fopen("sample.csv","rb");
     char *attr=malloc(sizeof(char));
     int len=1;
     char ch;
     int count=1;
     while((ch = fgetc(file)) != '\n') 
     {
 	if(ch==',')
		count++;
        attr[len-1]=ch;
        len++;
        attr=realloc(attr,(len)*sizeof(char));
    }
     attr[len-1]='\0';
     int i=0,j,k;
     printf("%s\n",attr);
     char attribute[count][16];
       j=0;
	while(j<count)
	{ 	k=0;
       		while(((attr[i]!=',')&&(attr[i]!='\0'))&&(k<16))
		{
	    		attribute[j][k++]=attr[i++];
		}
		attribute[j][k]='\0';
		j++;i++;
	}
	i=0;
	while(i<count)
		printf("%s\n",attribute[i++]);
 }

int main()
{
	parse();
	printf("%d",check_type("11111"));
	printf("%d",check_type("1.111"));
	printf("%d",check_type("1.11a"));
	printf("%d",check_type("1..1.1.1"));
	printf("%d",check_type("abcd_"));
	printf("%d",check_type("abcd12"));
}

