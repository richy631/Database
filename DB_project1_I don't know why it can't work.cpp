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
	Data* previous; 
	Data(){
		next = NULL;
		previous = NULL;
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
	pch = strtok (str," \n,'(){}");
	while (pch != NULL){
	//	printf ("%s\n",pch);
		keyword[cnt++] = pch;
		pch = strtok (NULL, " \n,'(){}");
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
			//if there is no quote
			if('A' <= c && c <= 'Z')
				c = tolower(c);
		}
		else if(c == ' ')
			c = '_';
		str[cnt++] = c;
	}
	//here will print all word 
	//printf("%s\n", str);
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
			Table *check = new Table;
			check = root;
			int sameTable = 0;
			for(int j=0;j<numofTable;j++){
				if(!strcmp(check->next->tableName, keyword[i+2]))
					sameTable = 1;
				check = check->next;
			}
			
			if(sameTable) break;
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
			while(i < len-1){
				printf("i = %d, len = %d\n\n", i, len);
				printf("keyword[14] = %s\n", keyword[14]);
				//get the attribute and type
				newTable->attrName[attrcnt] = keyword[i];
				printf("attrName[%d] get %s\n", attrcnt, newTable->attrName[attrcnt]);
				i += 1;
				newTable->types[attrcnt++] = keyword[i];
				printf("type[%d] get %s\n", attrcnt-1, newTable->types[attrcnt-1]);
				i += 1;
				//get primary key
				if(!strcmp(keyword[i], "primary")){
					newTable->primarykey[attrcnt-1] = 1;
					printf("primarykey[%d] = 1\n", attrcnt-1);
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
			Table *nowtable;
			Data *nowdata;
			nowtable = root->next;
			
			//start to find table
			//now keyword[i] is table name
			for(int j=0;j<numofTable;j++){
				if(!strcmp(nowtable->tableName, keyword[i])){
					//got that table
					Data *data = new Data;
					//no data exist , then create new data
					if(nowtable->dataCount == 0){
						nowtable->dataRoot = data;
						nowtable->datatail = data;
						nowtable->dataCount += 1;	
					}
					else{
						nowtable->datatail->next = data;
						data->previous = nowtable->datatail;
						nowtable->datatail = data;
						nowtable->dataCount += 1;
					}
					//insert word in data
					if(!strcmp(keyword[i+1],"values")){
						//same order as in table
						i += 2;
						//now keyword[i] is first word
						//start to insert
						for(int k=0;k<nowtable->attrNum;k++){
							int conflict = 0;
							//check if it is primary key
							//printf("nowtable's data number is %d\n", nowtable->dataCount);
							printf("primarykey[%d] = %d\n", k, nowtable->primarykey[k]);
							if(nowtable->primarykey[k] == 1){
								//is primary key
								Data *check = new Data;
								check = root->next->dataRoot;
								printf("got primary key!!\n\n");
								for(int l=0;l<nowtable->dataCount-1;l++){
									printf("I am in loop %d\n", l+1);
									printf("new data is %s\n", keyword[i]);
									printf("data in table is %s\n", check->attr[k]);
									if(!strcmp(check->attr[k], keyword[i])){
										
										conflict = 1;
									}
									check = check->next;
								}
							}
							if(conflict == 0){
								data->attr[k] = keyword[i++];
								printf("insert : %s\n", keyword[i-1]);
							}
							else{
								printf("There is already exist same primary key\n");
								nowtable->datatail = data->previous;
								nowtable->datatail->next = NULL;
								nowtable->dataCount -= 1;
								delete data;
								break;
							}
							
							//origin
							//data->attr[k] = keyword[i++];
							//printf("insert : %s\n", keyword[i-1]);
						}
					}
					else{
						//as given order
						//use array a to record the order
						//get the order
						int *a = (int*)malloc(nowtable->attrNum * sizeof(int));
						for(int k=0;k<nowtable->attrNum;k++){
							for(int l=0;l<nowtable->attrNum;l++){
								if(!strcmp(keyword[i+k+1], nowtable->attrName[l])){
									a[k] = l;
								}
							}
						}
						i += (nowtable->attrNum + 2);
						//now keyword[i] is first data of insert
						for(int k=0;k<nowtable->attrNum;k++){
							int conflict = 0;
							//check if it is primary key
							//printf("nowtable's data number is %d\n", nowtable->dataCount);
							printf("primarykey[%d] = %d\n", k, nowtable->primarykey[k]);
							if(nowtable->primarykey[k] == 1){
								//is primary key
								Data *check = new Data;
								check = root->next->dataRoot;
								printf("got primary key!!\n\n");
								for(int l=0;l<nowtable->dataCount-1;l++){
									printf("I am in loop %d\n", l+1);
									printf("new data is %s\n", keyword[i]);
									printf("data in table is %s\n", check->attr[k]);
									if(!strcmp(check->attr[k], keyword[i])){
										
										conflict = 1;
									}
									check = check->next;
								}
							}
							if(conflict == 0){
								data->attr[a[k]] = keyword[i++];
								printf("insert : %s\n", keyword[i-1]); 
							}
							else{
								printf("There is already exist same primary key\n");
								nowtable->datatail = data->previous;
								nowtable->datatail->next = NULL;
								nowtable->dataCount -= 1;
								delete data;
								break;
							}
							//origin
							//data->attr[a[k]] = keyword[i++];
							//printf("insert : %s\n", keyword[i-1]); 
						}
					}
					break;
				}
				nowtable = nowtable->next;
				if(j==numofTable-1)
					printf("404 table not found\n");
			}
		}
		else if(! strcmp(keyword[i], "showtable")){
			Table *nowtable;
			Data *nowdata;
			nowtable = root->next;
			for(int k=0;k<numofTable;k++){
				nowdata = nowtable->dataRoot;
				printf("Table %d : %s\n", k+1, nowtable->tableName);
				for(int j=0;j<nowtable->attrNum;j++){
				//	printf("attribute name : %s\n", nowtable->attrName[j]);
				//	printf("type name : %s\n", nowtable->types[j]);
					printf("%25s", nowtable->attrName[j]);
				}
				printf("\n");
				for(int j=0;j<nowtable->dataCount;j++){
					for(int l=0;l<nowtable->attrNum;l++){
						printf("%25s", nowdata->attr[l]);
					}
					printf("\n");
					nowdata = nowdata->next;
				}
				
				nowtable = nowtable->next;
			}
		}
	}
}
