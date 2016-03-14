#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <cctype>
#include <string>
#include <sstream> 

using namespace std;

void GetInstruction();
void Analysis(char* str);

struct Data{
	char* attr[7];
	Data* next;
	Data(){
		next = NULL;
	}
};

struct Table{
	char tableName[50];
	char* attrName[7];
	char* types[7];//int or varchar
	int varcharLength[7];
	int attrNum;//how many attribute are there in this table
	int primarykey[7];
	int dataCount;
	
	Data* dataRoot;
	Data* datatail;
	
	Table *next;
	
	
	Table(){
		attrNum = 0;
		dataCount = 0;
		dataRoot = NULL;
		datatail = NULL;
		memset(primarykey, 0, 7*sizeof(int));	
	}
	
	
};

Table *root;
Table *tail;

int numofTable = 0;

int main(){
	char c;
	root = new Table;
	tail = root; 
	do{
		GetInstruction();
	}
	while(c = getchar() != EOF);
	
	return 0;
}


void Analysis(char* str, char** keyword){
	int cnt = 0;
	char* pch;
	// word only
	pch = strtok (str," \n,(){}");
	while (pch != NULL){
	//	printf ("%s\n",pch);
		keyword[cnt++] = pch;
		pch = strtok (NULL, " \n,(){}");
	}
}

void GetInstruction(){
	char* keyword[300];
	for(int i=0;i<300;i++){
		keyword[i] = 0;
	}
	char *str = new char[300];
	char c;
	int cnt = 0;
	bool isquote = 0;
	while( (c = getchar()) != ';'){
		//if there is '', case sensitive
		if(c == '\'')
			isquote = !isquote;
		if(isquote == 0){
			if('A' <= c && c <= 'Z')
				c = tolower(c);
		}
		str[cnt++] = c;
	}
	printf("%s\n", str);
	Analysis(str, keyword);
	int len = 0;
	for(int i=0;i<300;i++){
	//	printf("keyword [%d] = %d\n", i, keyword[i]);
	//	printf("%s\n",keyword[i]);
		if(keyword[i] != 0){
			len++;
	//		printf("len = %d\n", len);
		}
		else break;
	}
	
	//check all word collected 
	/* 
	printf("len = %d\n", len);
	for(int i=0;i<len;i++){
		printf("%s\n",keyword[i]);	
	}
	*/ 
	
	for(int i=0;i<len;i++){
		if(! strcmp(keyword[i], "create")){
			numofTable += 1;
			i += 2;
			Table *newTable = new Table;
			tail->next = newTable;
			tail = newTable;
			//get table name
			strcpy(newTable->tableName, keyword[i]);
			printf("get table : %s\n", newTable->tableName);
			//get attribute
			int attrcnt = 0;
			i += 1;
			//now keyword[i] is attribute name
			while(i < len){
				newTable->attrName[attrcnt] = keyword[i];
				printf("attrName[%d] get %s\n", attrcnt, newTable->attrName[attrcnt]);
				i += 1;
				newTable->types[attrcnt++] = keyword[i];
				printf("type[%d] get %s\n", attrcnt-1, newTable->types[attrcnt-1]);
				i += 1;
				
				if(!strcmp(keyword[i], "primary")){
					newTable->primarykey[attrcnt-1] = 1;
					printf("got primary key = %s\n", keyword[i-2]);
					i += 2;
				}
				//for varchar number
				else if(isdigit(*keyword[i])){
					stringstream s_str( keyword[i] );
				    int num;
				    s_str >> num;
					newTable->varcharLength[attrcnt-1] = num;
					printf("varcharLength[%d] get %d\n", attrcnt-1, num);
					i += 1;
				}
				newTable->attrNum = attrcnt;
				printf("%d attributes !!\n\n", newTable->attrNum);
			}
			printf("Now have %d Table\n", numofTable);
		}
		
		else if(! strcmp(keyword[i], "insert")){
			i += 2;
			Table *now;
			Data *nowdata;
			now = root->next;
			
			//start to find table
			//now keyword[i] is table name
			for(int j=0;j<numofTable;j++){
				if(!strcmp(now->tableName, keyword[i])){
					//got that table
					Data *data = new Data;
					if(now->dataCount == 0){
						now->dataRoot = data;
						now->datatail = data;
						now->dataCount += 1;	
					}
					else{
						now->datatail->next = data;
						now->datatail = data;
						now->dataCount += 1;
					}
					if(!strcmp(keyword[i+1],"values")){
						//same order as in table
						i += 2;
						//start to insert
						for(int k=0;k<now->attrNum;k++){
							data->attr[k] = keyword[i++];
							printf("insert : %s\n", keyword[i-1]);
						}
					}
					else{
						//as given order
						//use array a to record the order
						int *a = (int*)malloc(now->attrNum * sizeof(int));
						for(int k=0;k<now->attrNum;k++){
							for(int l=0;l<now->attrNum;l++){
								if(!strcmp(keyword[i+k+1], now->attrName[l])){
									a[k] = l;
								}
							}
						}
						i += (now->attrNum + 2);
						//now keyword[i] is first data of insert
						for(int k=0;k<now->attrNum;k++){
							data->attr[a[k]] = keyword[i++];
							printf("insert : %s\n", keyword[i-1]); 
						}
					}
					break;
				}
				now = now->next;
				if(j==numofTable-1)
					printf("404 table not found\n");
			}
		}
		else if(! strcmp(keyword[i], "showtable")){
			Table *now;
			Data *nowdata;
			now = root->next;
			for(int k=0;k<numofTable;k++){
				nowdata = now->dataRoot;
				printf("Table %d : %s\n", k, now->tableName);
				for(int j=0;j<now->attrNum;j++){
				//	printf("attribute name : %s\n", now->attrName[j]);
				//	printf("type name : %s\n", now->types[j]);
					printf("%15s", now->attrName[j]);
				}
				printf("\n");
				for(int j=0;j<now->dataCount;j++){
					for(int l=0;l<now->attrNum;l++){
						printf("%15s", nowdata->attr[l]);
					}
					printf("\n");
					nowdata = nowdata->next;
				}
				
				now = now->next;
			}
		}
	}
}
