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
void showtable();

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
	char types[7];//int(i) or varchar(v)
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
		showtable();
	}
	while(c = getchar() != EOF);
	
	return 0;
}

void showtable(){
	Table *nowtable;
	Data *nowdata;
	nowtable = root->next;
	for(int k=0;k<numofTable;k++){
		nowdata = nowtable->dataRoot;
		printf("----------Table %d : %s----------\n", k+1, nowtable->tableName);
		for(int j=0;j<nowtable->attrNum;j++){
		//	printf("attribute name : %s\n", nowtable->attrName[j]);
		//	printf("type name : %s\n", nowtable->types[j]);	
			if(nowtable->primarykey[j] == 1)
				printf("#%-14s", nowtable->attrName[j]);
			else 			
				printf("%-15s", nowtable->attrName[j]);									
		}
		printf("\n");
		for(int j=0;j<nowtable->dataCount;j++){
			for(int l=0;l<nowtable->attrNum;l++){
				printf("%-15s", nowdata->attr[l]);
			}
			printf("\n");
			nowdata = nowdata->next;
		}
		
		nowtable = nowtable->next;
	}	
}

void Analysis(char* str, char** keyword){
	int cnt = 0;
	char* pch = NULL;
	// word only
	pch = strtok (str," \n,'(){}");
	while (pch != NULL){
		//printf ("%s\n",pch);
		keyword[cnt++] = pch;
		pch = strtok (NULL, " \n,'(){}");
	}
}

void GetInstruction(){
	char stack[300];
	char* keyword[300];
	int stackcnt = 0;
	for(int i=0;i<300;i++){
		stack[i] = 0;
		keyword[i] = 0;
	}
	char *str = new char[300];
	char c;
	int cnt = 0;
	bool isquote = 0;
	while(1){
		c = getchar();
		//printf("%c", c);
		if(c == ';'){
			str[cnt] = c;
			break;
		}
		//if there is '' , case sensitive
		else if(c == '\'')
			isquote = !isquote;
		else if(c == '('){
			stack[stackcnt++] = c;
		}
		else if(c == ')'){
			stackcnt--;	
		}
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
	if(stackcnt!=0){
		printf("() is not match\n"); 
		return;
	}
	Analysis(str, keyword);
	int len = 0;
	for(int i=0;i<300;i++){
		//printf("keyword [%d] = %s\n", i, keyword[i]);
		//printf("%s\n",keyword[i]);
		if(keyword[i] != 0){
			len++;
			//printf("len = %d\n", len);
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
			
			if(sameTable){
				printf("same Table exixt!!\n");
				break;
			}
			i += 2;
			Table *newTable = new Table;
			
			//get table name
			strcpy(newTable->tableName, keyword[i]);
			printf("get table : %s\n", newTable->tableName);
			//get attribute
			int attrcnt = 0;
			i += 1;
			//now keyword[i] is attribute name
			int somethingWrong = 0;
			while(i < len-1){
				
				//printf("i = %d, len = %d\n", i, len);
				//printf("now keyword is = %s\n", keyword[i]);
				if(!strcmp(keyword[i], ";"))
					break;
				//get the attribute and type
				newTable->attrName[attrcnt] = keyword[i];
				//printf("attrName[%d] get %s\n", attrcnt, newTable->attrName[attrcnt]);
				i += 1;
				if(!strcmp(keyword[i], "varchar"))
					newTable->types[attrcnt++] = 'v';
				else if(!strcmp(keyword[i], "int"))
					newTable->types[attrcnt++] = 'i';
				else{
					printf("Doesn't have type : %s\n", keyword[i]);
					somethingWrong = 1;
					break;
				}
				//printf("type[%d] get %s\n", attrcnt-1, newTable->types[attrcnt-1]);
				
				//printf("i = %d, len = %d\n", i, len);
				
				
				//get primary key
				if(!strcmp(keyword[i+1], "primary")){
					newTable->primarykey[attrcnt-1] = 1;
				//	printf("primarykey[%d] = 1\n", attrcnt-1);
				//	printf("got primary key = %s\n", keyword[i-1]);
					i += 2;
				}
				//for varchar number
				else if(isdigit(*keyword[i+1])){
					//printf("FIND varchar number!!\n");
					//string to interger
					stringstream s_str( keyword[i+1] );
				    int num;
				    s_str >> num;
					newTable->varcharLength[attrcnt-1] = num;
					//printf("varcharLength[%d] get %d\n", attrcnt-1, num);
					i += 1;
				}
				
				i += 1;
				newTable->attrNum += 1;
				//printf("this table's attrnum = %d\n", attrcnt);
				//printf("%d attributes !!\n\n", newTable->attrNum);
			}
			if(somethingWrong == 0){
				numofTable += 1;
				printf("Now have %d Table\n", numofTable);
				tail->next = newTable;
				tail = newTable;
			}
		}
		
		else if(! strcmp(keyword[i], "insert")){
			i += 2;
			Table *nowtable;
			Data *nowdata;
			nowtable = root->next;
			
			//start to find table
			//now keyword[i] is table name
			int somethingWrong = 0;
			for(int j=0;j<numofTable;j++){
				if(!strcmp(nowtable->tableName, keyword[i])){
					//got that table
					Data *data = new Data;
					
					//insert word in data
					if(!strcmp(keyword[i+1],"values")){
						//same order as in table
						i += 2;
						//now keyword[i] is first word
						//start to insert
						//printf("start to insert : %s\n", keyword[i]);
						for(int k=0;k<nowtable->attrNum;k++){
							int conflict = 0;
							//check if it is primary key
							if(nowtable->primarykey[k] == 1){
								//is primary key
								Data *check = new Data;
								check = root->next->dataRoot;
								//printf("got primary key!!\n\n");
								for(int l=0;l<nowtable->dataCount;l++){
								//	printf("I am in loop %d\n", l+1);
								//	printf("new data is %s\n", keyword[i]);
								//	printf("data in table is %s\n", check->attr[k]);
									if(!strcmp(check->attr[k], keyword[i])){
										somethingWrong = 1;
										conflict = 1;
										break;
									}
									check = check->next;
								}
							}
							if(conflict == 0){
								//printf("Not primary key!!\n");
								if(nowtable->types[k] == 'v'){//is varchar
									if(strlen(keyword[i]) < nowtable->varcharLength[k]){
										data->attr[k] = keyword[i++];
										printf("insert : %s\n", keyword[i-1]);
									}
									else{
										somethingWrong = 1;
										printf("out of varchar RANGE!!\n");
										break;
									}
								}
								else{//is interger
									if(isdigit(*keyword[i])){ 
										data->attr[k] = keyword[i++];
										printf("insert : %s\n", keyword[i-1]);	
									}
									else{
										somethingWrong = 1;
										printf("Not a Number!!\n");
										break;
									}
								}
							}
							
							else{
								printf("There is already exist same primary key\n");
								break;
							}
							
							//origin
							//data->attr[k] = keyword[i++];
							//printf("insert : %s\n", keyword[i-1]);
						}
						
						if(somethingWrong == 0){
							printf("-----INDERT SUCCESSFULLY-----\n");
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
						/*printf("¶¶§Ç:");
						for(int g=0;g<nowtable->attrNum;g++){
							printf("%d ", a[g]);
						}
						printf("\n");*/
						i += (nowtable->attrNum + 2);
						//now keyword[i] is first data of insert
						for(int k=0;k<nowtable->attrNum;k++){
							int conflict = 0;
							//check if it is primary key
							if(nowtable->primarykey[a[k]] == 1){
								//is primary key
								Data *check = new Data;
								check =	nowtable->dataRoot;
								//printf("got primary key = %s!!\n", nowtable->attrName[a[k]]);
								for(int l=0;l<nowtable->dataCount;l++){
								//	printf("I am in loop %d\n", l+1);
								//	printf("new data is %s\n", keyword[i]);
								//	printf("data in table is %s\n", check->attr[a[k]]);
									if(!strcmp(check->attr[a[k]], keyword[i])){
										printf("primary key conflict\n");
										somethingWrong = 1;
										conflict = 1;
										break;
									}
									check = check->next;
								}
							}
							if(conflict == 0){
								//printf("Not primary key!!\n");
								if(nowtable->types[a[k]] == 'v'){
									if(strlen(keyword[i]) < nowtable->varcharLength[a[k]]){
										data->attr[a[k]] = keyword[i++];
										printf("insert : %s\n", keyword[i-1]);
									}
									else{
										somethingWrong = 1;
										printf("out of varchar RANGE!!\n");
										break;
									}
								}
								else{//is interger
									if(isdigit(*keyword[i])){ 
										data->attr[a[k]] = keyword[i++];
										printf("insert : %s\n", keyword[i-1]);	
									}
									else{
										somethingWrong = 1;
										printf("Not a Number!!\n");
										break;
									}
								}
							}
							
							else{
								printf("There is already exist same primary key\n");
								break;
							}
							
							//origin
							//data->attr[k] = keyword[i++];
							//printf("insert : %s\n", keyword[i-1]);
						}
						
						if(somethingWrong == 0){
							printf("-----INDERT SUCCESSFULLY-----\n");
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
			showtable();
		}
	}
}

