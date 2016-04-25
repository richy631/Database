#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <cctype>
#include <string>
#include <sstream> 
#include <vector>

using namespace std;

void GetInstruction();
int Analysis(char* str, char** keyword);
void showtable();

struct Data{
	char* attr[7];//the real data
	char* fromWhichTable;
	Data* next;
	Data* previous; 
	Data(){
		fromWhichTable = NULL;
		next = NULL;
		previous = NULL;
	}
};

struct Table{
	char tableName[50];
	char alias;
	char* attrName[7];
	char types[7];//int(i) or varchar(v)
	int varcharLength[7];
	int attrNum;//how many attribute are there in this table
	int primarykey[7];//which attribute primary key is
	int dataCount;
	int isSelect[7];
	
	Data* dataRoot;
	Data* datatail;
	
	Table *next;
	Table *tabletail;
	
	
	Table(){
		attrNum = 0;
		alias = NULL;
		dataCount = 0;
		dataRoot = NULL;
		datatail = NULL;
		memset(primarykey, 0, 7*sizeof(int));
		memset(isSelect, 0, 7*sizeof(int));	
	}
	
	void copy(Table* srcTable){
		strcpy(srcTable->tableName, this->tableName);
		strcpy(srcTable->types, this->types);
		srcTable->alias = this->alias;
		srcTable->attrNum = this->attrNum;
		srcTable->dataCount = this->dataCount;
		for(int i=0;i<7;i++){
			strcpy(srcTable->attrName[i], this->attrName[i]);
			srcTable->varcharLength[i] = this->varcharLength[i];
			srcTable->primarykey[i] = this->primarykey[i];
			//srcTable->isSelect[i] = this->isSelect[i];
			
		}
		srcTable->dataRoot = this->dataRoot;
		srcTable->datatail = this->datatail;
		srcTable->next = NULL;
		srcTable->tabletail = NULL;
	}
	

	
};

Table *root;
Table *tail;

int numofTable = 0;

int main(int argc, char ** argv){
	char c;
	root = new Table;
	tail = root; 
	//FILE  *fopen("textcase.txt","i");
	
	do{
		GetInstruction();
		//showtable();
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
		printf("----------Table %d : %s (%c)----------\n", k+1, nowtable->tableName, nowtable->alias);
		
		for(int j=0;j<nowtable->attrNum;j++){
		//	printf("attribute name : %s\n", nowtable->attrName[j]);
		//	printf("type name : %s\n", nowtable->types[j]);	
			if(nowtable->primarykey[j] == 1)
				printf("#%-15s", nowtable->attrName[j]);
			else 			
				printf("%-16s", nowtable->attrName[j]);									
		}
		printf("\n");
		for(int j=0;j<nowtable->dataCount;j++){
			for(int l=0;l<nowtable->attrNum;l++){
				if(nowdata->attr[l] != NULL)
					printf("%-16s", nowdata->attr[l]);
				else
					printf("      NULL      \n");
			}
			printf("\n");
			nowdata = nowdata->next;
		}
		
		nowtable = nowtable->next;
	}	
}

bool compare(char* word1, char* word2, char* op){
	if(!strcmp(op, "=")){
		return (!strcmp(word1, word2));
	}
	else if(!strcmp(op, "<>")){
		return (strcmp(word1, word2));
	}
	else if(!strcmp(op, ">")){
		if(isdigit(*word2)){//word2 is number
			stringstream w1_str( word1 );
		    int num1;
		    w1_str >> num1;
		    //printf("num1 = %d\n", num1);
		    
		    stringstream w2_str( word2 );
		    int num2;
		    w2_str >> num2;
		    //printf("num2 = %d\n", num2);
		    
		    return (num1 > num2);
		}
		else return false;
	}
	else if(!strcmp(op, "<")){
		if(isdigit(*word2)){//word2 is number
			stringstream w1_str( word1 );
		    int num1;
		    w1_str >> num1;
		    //printf("num1 = %d\n", num1);
		    
		    
		    stringstream w2_str( word2 );
		    int num2;
		    w2_str >> num2;
		    //printf("num2 = %d\n", num2);
		    
		    return (num1 < num2);
		}
		else return false;
	}
}

bool compareData(Data* data1, Data* data2){
	for(int i=0;i<3;i++){
		if(strcmp(data1->attr[i], data2->attr[i])){
			return false;
		}
	}
	return true;
}

int Analysis(char* str, char** keyword){
	int cnt = 0;
	char* pch = NULL;
	// word only
	pch = strtok (str," \n,'(){}.");
	while (pch != NULL){
		//printf ("%s\n",pch);
		keyword[cnt++] = pch;
		pch = strtok (NULL, " \n,'(){}.");
	}
	return cnt;
}

void GetInstruction(){
	char stack[300];
	char* keyword[300];
	int stackcnt = 0;
	char *str = new char[300];
	for(int i=0;i<300;i++){
		stack[i] = 0;
		keyword[i] = 0;
		str[i] = 0;
	}
	char c;
	int cnt = 0;
	bool isquote = 0;
	while(1){
		c = getchar();
		//if (!~c) break;
		//printf("%c", c);
		if(c == ';'){
			str[cnt] = 'X';
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
	int keywordNum = Analysis(str, keyword);
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
				if(keyword[i+1] != NULL && !strcmp(keyword[i+1], "primary")){
					newTable->primarykey[attrcnt-1] = 1;
				//	printf("primarykey[%d] = 1\n", attrcnt-1);
				//	printf("got primary key = %s\n", keyword[i-1]);
					i += 2;
				}
				//for varchar number
				else if(keyword[i+1] != NULL && isdigit(*keyword[i+1])){
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
			//printf("start to insert %s\n", keyword[i]);
			int somethingWrong = 0;
			for(int j=0;j<numofTable;j++){
				//printf("[LINE 355]start to insert %s\n", keyword[i]);
				if(!strcmp(nowtable->tableName, keyword[i])){
					//got that table
					Data *data = new Data;
					
					//insert word in data
					if(!strcmp(keyword[i+1],"values")){
						//same order as in table
						i += 2;
						//now keyword[i] is first word
						//start to insert
						
						for(int k=0;k<nowtable->attrNum;k++){
							//printf("start to insert in VALUE PART : %s\n", keyword[i]);
							int conflict = 0;
							//check if it is primary key
							if(nowtable->primarykey[k] == 1){
								//is primary key
								Data *check = new Data;
								check = nowtable->dataRoot;
								printf("got primary key!!\n\n");
								for(int l=0;l<nowtable->dataCount;l++){
									//printf("I am in loop %d\n", l+1);
									//printf("new data is %s\n", keyword[i]);
									//printf("data in table is %s\n", check->attr[k]);
									//printf("HAHAHAHAH\n");
									if(!strcmp(check->attr[k], keyword[i])){
										somethingWrong = 1;
										conflict = 1;
										continue;
									}
									check = check->next;
								}
							}
							if(conflict == 0){
								//printf("Not primary key!!\n");
								if(nowtable->types[k] == 'v'){//is varchar
									//printf("it's varchar!!\n");
									//printf("start to insert in VARCHAR: %s\n", keyword[i]);
									if(strlen(keyword[i]) <= nowtable->varcharLength[k]){
										//printf("length is correct!!\n");
										//printf("len = %d, i = %d\n", len, i);
										if( i+1 < len && keyword[i+1] == NULL ){
											data->attr[k] = NULL;
										}
										else{
											data->attr[k] = keyword[i];
											//printf("[LINE 400]insert : %s\n", keyword[i]);
											i++;
										}
									}
									else{
										somethingWrong = 1;
										printf("the length %s is %d\n", keyword[i], strlen(keyword[i]));
										printf("but this attr length limit is %d\n", nowtable->varcharLength[k]);
										printf("out of varchar RANGE!!\n");
										break;
									}
								}
								else{//is interger
									if(isdigit(*keyword[i])){ 
										data->attr[k] = keyword[i];
										//printf("[LINE 415]insert : %s\n", keyword[i]);
										//printf("insert : %s\n", keyword[i]);	
										i++;
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
										//printf("insert : %s\n", keyword[i-1]);
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
										//printf("insert : %s\n", keyword[i-1]);	
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
		else if(! strcmp(keyword[i], "select")){
			//start to find FROM & WHERE
			int from = 0;
			int where = 0;
			int whereCondition = 0;
			//printf("number of word is %d\n", keywordNum);
			for(int j=0;j<keywordNum;j++) {
				if(!strcmp(keyword[i+j], "from")){
					from = i+j;
				}
				else if(!strcmp(keyword[i+j], "where")){
					where = i+j;
				}
				else if(!strcmp(keyword[i+j], "or") || !strcmp(keyword[i+j], "and")){
					whereCondition ++;
				}
				
			}
			//if(where == 0) where = keywordNum;
			//printf("from = %d, where = %d, whereCondition = %d\n", from, where, whereCondition);
			
			//the FROM PART 
			//there has "where"
			Table* fromTableRoot;
			int fromTableNum = 0;
			if(where!=0){
				//find which table in from
				fromTableRoot = new Table;
				for(int j=from+1;j<where;j++){
					//start with j = tableName 
					//find that table
					//printf("start to find the table \" %s \"\n", keyword[j]);
					Table* tmptable = root->next;
					int tableFound = 0;
					for(int k=0;k<numofTable;k++){
						if( !strcmp(keyword[j], tmptable->tableName) ){
							tableFound = 1;
							// find that table
							//printf("find the table is \" %s \"\n", tmptable->tableName);
							Table* fromTable = new Table;
							*fromTable = *tmptable;
							fromTable->next = NULL;
							if(fromTableNum == 0){
								fromTableRoot->next = fromTable;
								fromTableRoot->tabletail = fromTable;
								fromTableNum ++;
							}
							else{
								fromTableRoot->tabletail->next = fromTable;
								fromTableRoot->tabletail = fromTable;
								fromTableNum ++;
							}
							
							//check if the table has alias
							//printf("[] check if the table has alias?\n");
							if( keyword[j+1] != NULL && !strcmp(keyword[j+1], "as")){
								//printf("it has alias!!!\n");
								j = j+2;//now j is alias
								fromTable->alias = keyword[j][0];
								printf("now %s has alias %c\n", fromTable->tableName, fromTable->alias);
							}
							//printf("() seems it don't has alias...\n");
						}
						tmptable = tmptable->next;
					}
					if(tableFound == 0){
						printf("404 table not found!!\n");
						break;
					}
				}
			}
			
			//there has NO "where"
			else{//if where == 0
				//find which table in from
				fromTableRoot = new Table;
				for(int j=from+1;j<keywordNum;j++){
					//start with j = tableName 
					//find that table
					//printf("start to find the table \" %s \"\n", keyword[j]);
					Table* tmptable = root->next;
					int tableFound = 0;
					for(int k=0;k<numofTable;k++){
						if( !strcmp(keyword[j], tmptable->tableName) ){
							tableFound = 1;
							// find that table
							//printf("find the table is \" %s \"\n", tmptable->tableName);
							Table* fromTable = new Table;
							*fromTable = *tmptable;
							fromTable->next = NULL;
							if(fromTableNum == 0){
								fromTableRoot->next = fromTable;
								fromTableRoot->tabletail = fromTable;
								fromTableNum ++;
							}
							else{
								fromTableRoot->tabletail->next = fromTable;
								fromTableRoot->tabletail = fromTable;
								fromTableNum ++;
							}
							
							//check if the table has alias
							//printf("[] check if the table has alias?\n");
							if( keyword[j+1] != NULL && !strcmp(keyword[j+1], "as")){
								//printf("it has alias!!!\n");
								j = j+2;//now j is alias
								fromTable->alias = keyword[j][0];
								printf("now %s has alias %c\n", fromTable->tableName, fromTable->alias);
							}
							//printf("() seems it don't has alias...\n");
						}
						tmptable = tmptable->next;
					}
					if(tableFound == 0){
						printf("404 table not found!!\n");
						break;
					}
				}
				
			}
			Table* tmptable;
			tmptable = fromTableRoot->next;
			printf("FROM TABLE :\n");
			for(int k=0;k<fromTableNum;k++){		
				printf("%s, ", tmptable->tableName);
				tmptable = tmptable->next;
			}
			printf("\n");
			
			int count = 0;
			char* countStr;
			int sum = 0;
			char* sumStr;
			//the SELECT PART
			printf("start to parse SELECT part!!\n");
			Table* selectTableRoot = new Table;
			int selectTableNum = 0;
			if(!strcmp(keyword[i+1], "*")){
				printf("select *\n");
				Table* fromtable = fromTableRoot->next;
				for(int j=0;j<fromTableNum;j++){
					for(int k=0;k<fromtable->attrNum;k++){
						Table* selectTable = new Table;
						*selectTable = *fromtable;
						selectTable->next = NULL;
						selectTable->isSelect[k] = 1;
						if(k == 0){
							selectTableRoot->next = selectTable;
							selectTableRoot->tabletail = selectTable;
							selectTableNum ++;
						}
						else{
							selectTableRoot->tabletail->next = selectTable;
							selectTableRoot->tabletail = selectTable;	
							selectTableNum ++;
						}
					}
					fromtable = fromtable->next;
				}
			}	
			else if(!strcmp(keyword[i+1], "count")){
				count = 1;
				countStr = keyword[i+2];
				if(!strcmp(keyword[i+2], "*")){
					Table* fromtable = fromTableRoot->next;
					for(int j=0;j<fromTableNum;j++){
						for(int k=0;k<fromtable->attrNum;k++){
							Table* selectTable = new Table;
							*selectTable = *fromtable;
							selectTable->next = NULL;
							selectTable->isSelect[k] = 1;
							if(k == 0){
								selectTableRoot->next = selectTable;
								selectTableRoot->tabletail = selectTable;
								selectTableNum ++;
							}
							else{
								selectTableRoot->tabletail->next = selectTable;
								selectTableRoot->tabletail = selectTable;	
								selectTableNum ++;
							}
						}
						fromtable = fromtable->next;
					}
				}
				else{
					Table* fromtable = fromTableRoot->next;
					for(int j=0;j<fromTableNum;j++){
						for(int k=0;k<fromtable->attrNum;k++){
							if(!strcmp(keyword[i+2], fromtable->attrName[k])){
								//find attribute name in count()!!
								Table* selectTable = new Table;
								*selectTable = *fromtable;
								selectTable->next = NULL;
								selectTableRoot->next = selectTable;
								selectTableRoot->tabletail = selectTable;
								selectTable->isSelect[k] = 1;
								selectTableNum ++;
							}
						}
						fromtable = fromtable->next;
					}
					
				}
			}
			else if(!strcmp(keyword[i+1], "sum")){
				sum = 1;
				sumStr = keyword[i+2];
				Table* fromtable = fromTableRoot->next;
				for(int j=0;j<fromTableNum;j++){
					for(int k=0;k<fromtable->attrNum;k++){
						if(!strcmp(keyword[i+2], fromtable->attrName[k])){
							//find attribute name in count()!!
							printf("find the SUM(XXX) = %s\n", sumStr);
							Table* selectTable = new Table;
							*selectTable = *fromtable;
							selectTable->next = NULL;
							selectTable->isSelect[k] = 1;
							selectTableRoot->next = selectTable;
							selectTableRoot->tabletail = selectTable;
							selectTableNum ++;
						}
					}
					fromtable = fromtable->next;
				}
				printf("select table name = %s\n", selectTableRoot->next->tableName);
			}
			else{//SELECT XXX
				tmptable = fromTableRoot->next;
				printf("FROM TABLE :\n");
				for(int k=0;k<fromTableNum;k++){		
					printf("%s, ", tmptable->tableName);
					tmptable = tmptable->next;
				}
				printf("\n");
			
			
				for(int j=1;j<from;j++){
					Table* tmptable = fromTableRoot->next;
					//printf("[CHANGE_TABLE_TO J->%d] %p : %p\n", j,tmptable, tmptable->next);
					
					//if the attribute has prefix table name
					int find = 0;
					//printf("start to find the word \"%s\" to SELECT...\n", keyword[j]);
					int attributeFound = 0;
					for(int k=0;k<fromTableNum;k++){
						//full name or alias
						if(!strcmp(keyword[j], tmptable->tableName) || (keyword[j][0] == tmptable->alias)){
							//printf("[%s] Table : \"%s\"\n", __LINE__, tmptable->tableName);
							//find which table
							//then start to find attribute
							attributeFound = 0;
							for(int l=0;l<tmptable->attrNum;l++){
								if(!strcmp(keyword[j+1], tmptable->attrName[l])){
									//printf("in \"SELECT\" part\n");
									//printf("find the attribute!!\n");
									//printf("attribute : \"%s\"\n", tmptable->attrName[l]);
									//printf("it is from table : %s\n\n", tmptable->tableName);
									
									find = 1;
									attributeFound = 1;
									//find that attribute
									
									j = j + 1;
									
									Table* selectTable = new Table;
									*selectTable = *tmptable;
									selectTable->next = NULL; 
									selectTable->isSelect[l] = 1;
									if(selectTableNum == 0){
										selectTableRoot->next = selectTable;
										selectTableRoot->tabletail = selectTable;
										selectTableNum ++;	
									}
									else{
										selectTableRoot->tabletail->next = selectTable;
										selectTableRoot->tabletail = selectTable;
										selectTableNum ++;
									}
								}
								if(attributeFound == 1)
									break;
								if(!strcmp(keyword[j+1], "*")){
									//printf("find SELECT XXX.*\n");
									find = 1;
									attributeFound = 1;
									j = j + 1;
									Table* selectTable = new Table;
									*selectTable = *tmptable;
									selectTable->next = NULL;
									if(selectTableNum == 0){
										selectTableRoot->next = selectTable;
										selectTableRoot->tabletail = selectTable;
										selectTableNum ++;	
									}
									else{
										selectTableRoot->tabletail->next = selectTable;
										selectTableRoot->tabletail = selectTable;
										selectTableNum ++;
									}
									//printf("all \"isSelect\" should be 1\n");
									for(int m=0;m<selectTable->attrNum;m++){
										selectTable->isSelect[m] = 1;
									}
								}
							}
							
							
							
							
							if(attributeFound == 1)	break;
							else{
								printf("404 attribute not found in table \"%s!!\"\n", tmptable->tableName);
								break;
							}
						}
						if(find) break;
						
						//printf("start to find next table\n");
						tmptable = tmptable->next;
					}
					//if(attributeFound == 0)	break;
					if(find) continue;
					
					//printf("there is no table name!!, Just attribute\n");
					
					//if nothing happend then the first word is attribute
					//find where table this attribute from
					//printf("[CHANGE_TABLE_TO J->%d] %p : %p\n", j,tmptable, tmptable->next);
					tmptable = fromTableRoot->next;
					//printf("[CHANGE_TABLE_TO MIDDLE->%d] %p : %p\n", j,tmptable, tmptable->next);
					for(int k=0;k<fromTableNum;k++){
						//printf("[CHANGE_TABLE_TO K->%d] %p : %p\n", k,tmptable, tmptable->next);
						//printf("now keyword[%d] is %s\n", j, keyword[j]);
						//printf("this table is %s\n", tmptable->tableName);
						
						for(int l=0;l<tmptable->attrNum;l++){
							//printf("[CHANGE_TABLE_TO L] %p : %p\n", tmptable, tmptable->next);
							if(!strcmp(tmptable->attrName[l], keyword[j])){
								//found that attribute
								//printf("in \"SELECT\" part\n");
								//printf("find the attribute!!\n");
								//printf("[758] Table : \"%s\"\n", tmptable->tableName);
								//printf("[758] attribute : \"%s\"\n", tmptable->attrName[l]);
								attributeFound = 1;
								
								//j = j + 1;
								find = 1;
								
								Table* selectTable = new Table;
								*selectTable = *tmptable;
								selectTable->next = NULL; 
								selectTable->isSelect[l] = 1;
								if(selectTableNum == 0){
									selectTableRoot->next = selectTable;
									selectTableRoot->tabletail = selectTable;
									selectTableNum ++;
								}
								else{
									//printf("[CHANGE_TABLE_TO] BRFORE : %p : %p\n", tmptable, tmptable->next);
									selectTableRoot->tabletail->next = selectTable;
									//printf("[CHANGE_TABLE_TO] TAIL->NRXT: %p : %p\n", tmptable, tmptable->next);
									selectTableRoot->tabletail = selectTable;
									//printf("[CHANGE_TABLE_TO] TAIL: %p : %p\n", tmptable, tmptable->next);
									selectTableNum ++;
									//printf("[CHANGE_TABLE_TO] AFTER : %p : %p\n", tmptable, tmptable->next);
								}
							}
							if(attributeFound) break;
						}
						//if(find){
						//	break;
						//}
						//printf("[CHANGE_TABLE_TO] %p : %p\n", tmptable, tmptable->next);
						tmptable = tmptable->next;
					}
					
					if(attributeFound == 0){
						printf("404 attribute not found!!\n");
						break;	
					}
					//printf("j = %d, from = %d\n", j, from);
				}
				//printf("from = %d; where = %d; keywordNum = %d\n", from, where, keywordNum);
				//printf("now go to the WHERE part\n\n");
			}
			Table* nowtable;
			nowtable = fromTableRoot->next;
			printf("FROM TABLE :\n");
			for(int k=0;k<fromTableNum;k++){		
				printf("%s, ", nowtable->tableName);
				nowtable = nowtable->next;
			}
			printf("\n");
			nowtable = selectTableRoot->next;
			printf("SELECT TABLE :\n");
			for(int k=0;k<selectTableNum;k++){		
				printf("%s, ", nowtable->tableName);
				nowtable = nowtable->next;
			}
			printf("\n");
			
			//the WHERE PART
			//there is no WHERE
			if(!where){
				if(count == 1){
					printf("COUNT(%s)\n", countStr);
					printf("%d\n", selectTableRoot->next->dataCount);
				}
				else if(sum == 1){
					
				}
				else{
					Table *nowtable;
					Data *nowdata;
					nowtable = selectTableRoot->next;
					printf("fromTableRoot's name = %s\n", fromTableRoot->next->tableName);
					printf("selectTableRoot's name = %s\n", selectTableRoot->next->tableName);
					
					for(int j=0;j<selectTableNum;j++){
						for(int k=0;k<nowtable->attrNum;k++){
							if(nowtable->isSelect[k] == 1){
								printf("%7s.%-8s ", nowtable->tableName, nowtable->attrName[k]);	
							}
						}
						nowtable = nowtable->next; 
					}
					printf("\n");
					nowtable = selectTableRoot->next;
					nowdata = nowtable->dataRoot;
					for(int j=0;j<nowtable->dataCount;j++){
						//printf("j = %d\n", j);
						Table* tmptable = nowtable; 
						for(int k=0;k<selectTableNum;k++){
							for(int l=0;l<tmptable->attrNum;l++){
								if(tmptable->isSelect[l] == 1){
									printf("%15s ", nowdata->attr[l]);
								}
							}
							tmptable = tmptable->next;
						}
						printf("\n");
						
						nowdata = nowdata->next;
						//printf("next data!!\n");
					}
				}
					
			}
			else{//has "WHERE" condition
				printf("start to parse WHERE condition!!\n");
				Data* data[10];  //just tell us where this attribute from
				Table* table[10];//just tell us where this attribute from
				memset(data, NULL, 10*sizeof(Data*));
				memset(table, NULL, 10*sizeof(Table*));
				char* andOr[5];
				int attr[10];
				//printf("there has \"WHERE\"\n");
				int op[5] = {};
				
				int condition = 0;

				int andOrCnt = 0;
				int eqStart = 0;
				for(int j=where+1;j<keywordNum;j++){
					int find = 0;
					int tableFound = 0;
					printf("this word is %s\n", keyword[j]);
					//if the attribute has prefix table name
					Table* tmptable;
					tmptable = fromTableRoot->next;
					
					for(int k=0;k<fromTableNum;k++){
						printf("this table is %s\n", tmptable->tableName);
						//full name or alias
						if(!strcmp(keyword[j], tmptable->tableName) || (keyword[j][0] == tmptable->alias)){
							//printf("this table is %s\n", tmptable->tableName);
							tableFound = 1;
							//printf("start to find attribute : %s\n", keyword[j+1]);
							//find the table
							//then start to find attribute
							int attributeFound = 0;
							for(int l=0;l<tmptable->attrNum;l++){
								if(!strcmp(keyword[j+1], tmptable->attrName[l])){
									//find that attribute
									printf("Table : %s, Attribute : %s\n", tmptable->tableName, tmptable->attrName[l]);
									find = 1;
									attributeFound = 1;
									table[condition] = new Table;
									table[condition] = tmptable;
									//printf("table[%d] is %s\n", condition, table[condition]->tableName);
									data[condition] = new Data;
									data[condition] = table[condition]->dataRoot;
									
									attr[condition] = l;
									//printf("first data is %s\n", data->attr[attr[condition]]);
									//printf("there are %d conditions\n", condition);
								
									//printf("eqStart = %d\n", eqStart);
									
									if(eqStart == 0){
										//printf("equation start, eqStart = %d\n", eqStart);
										//the operator will appear
										//(next keyword will be "=" or something else)
										eqStart = !eqStart;
										if(condition < 2*whereCondition + 1 || whereCondition==0){
											op[condition] = j+2;
											//printf("op[%d] = \"%d\"\n", condition, op[condition]);
											//printf("%d in keyword[%d] is %s\n", op[condition], op[condition], keyword[op[condition]]);
											
										}	
										j = j + 2;//now j is operator
										//printf("next word will be %s\n", keyword[j+1]);
									}
									else{//next keyword will be "AND" or "OR"
										//printf("equation is over, eqStart = %d\n", eqStart);
										eqStart = !eqStart;
										j = j + 2;//now j is "AND" or "OR"
										if(keyword[j] != NULL){
											andOr[andOrCnt] = keyword[j];
											//printf("andOr[%d] get %s\n", andOrCnt, keyword[j]); 
											andOrCnt ++;
										}
									}
									condition ++;
								}
								if(attributeFound){
									break;	
								}
							}
							/*if(attributeFound == 0){
								printf("404 attribute not found in table \"%s\"\n", tmptable->tableName);
								break;	
							}*/
						}
						if(find){
							break;
						}
						tmptable = tmptable->next;
					}
					/*if(tableFound == 0){
						printf("404 table not found!!\n");
						break;
					}*/
					if(find) continue;
					//if nothing happend then the first word is attribute
					//find where table this attribute from
					int attributeFound = 0;
					tmptable = fromTableRoot->next;
					for(int k=0;k<fromTableNum;k++){
						//printf("this table is %s\n", tmptable->tableName);
						for(int l=0;l<tmptable->attrNum;l++){
							if(!strcmp(tmptable->attrName[l], keyword[j])){
								find = 1;
								attributeFound = 1; 
								table[condition] = new Table;
								table[condition] = tmptable;
								//printf("table[%d] is %s\n", condition, table[condition]->tableName);
								data[condition] = new Data;
								data[condition] = table[condition]->dataRoot;
								//printf("attribute name is %s\n", table[condition]->attrName[l]);
								
								attr[condition] = l;
								attributeFound = 1;
								
								//printf("first data is %s\n", data->attr[l]);
								
								//printf("there are %d conditions\n", condition);
								//printf("eqStart = %d\n", eqStart);
								
								if(eqStart == 0){
									//printf("equation start, eqStart = %d\n", eqStart);
									//the operator will appear
									//(next keyword will be "=" or something else)
									eqStart = !eqStart;
									if(condition < 2*whereCondition + 1 || whereCondition==0){
										op[condition] = j+1;
										//printf("op[%d] = \"%d\"\n", condition, op[condition]);
										//printf("%d in keyword[%d]\n", op[condition], keyword[op[condition]]);
										
									}
									j = j + 1;//now j is operator
									
									//printf("next word will be %s\n", keyword[j+1]);
								}
								else{//next keyword will be "AND" or "OR"
									//printf("equation is over, eqStart = %d\n", eqStart);
									eqStart = !eqStart;
									j = j + 1;//now j is "AND" or "OR" 
									if(keyword[j] != NULL){
										andOr[andOrCnt] = keyword[j];
										//printf("andOr[%d] get %s\n", andOrCnt, keyword[j]); 
										andOrCnt ++;
									}
								}
								condition ++;
							}
						}
						tmptable = tmptable->next;
					}
					if(find) continue;
					//just a number or string
					//printf("condition = %d\n", condition);
					if(!strcmp(keyword[j], "and") || !strcmp(keyword[j], "or")){
						//printf("this keyword is \"%s\"\n", keyword[j]);
						//printf("is not attribute\n");
						andOr[andOrCnt] = keyword[j];
						//printf("andOr[%d] get %s\n", andOrCnt, keyword[j]); 
						andOrCnt ++;
						eqStart = !eqStart;
						continue;
					}
					
					condition ++;
				}
				//printf("condition = %d\n", condition);
				/*for(int j=0;j<10;j++){
					if(table[j] == NULL)
						printf("table[%d] is NULL\n", j);
					else{
						printf("table[%d] : HAHA I AM NOT NULL\n", j);
						printf("my table name is %s\n", table[j]->tableName);
					}
				}*/
				
				/*for(int j=0;j<condition;j+=2){
					if(table[j] != NULL){
						//printf("these two table will compare :\n");
						//printf("j = %d\n", j);
						//printf("table : %s\n", table[j]->tableName);
						//printf("attribute : %s\n", table[j]->attrName[attr[j]]);
						//printf("all posibible tuple :\n");
						//data[j] = table[j]->dataRoot; 
							
						//for(int x=0;x<table[j]->dataCount;x++){
							//printf("%-15s ", data[j]->attr[attr[j]]);
							/*if(table[j+1] == NULL){
								printf("%-15s ", data[j]->attr[attr[j]]);
								printf("%-15s \n", keyword[op[j]+1]); 
							}*/ 
								
							/*else{
								data[j+1] = table[j+1]->dataRoot;
								for(int x=0;x<table[j+1]->dataCount;x++){
									printf("%-15s ", data[j]->attr[attr[j]]);
									printf("%-15s\n", data[j+1]->attr[attr[j+1]]);
									data[j+1] = data[j+1]->next;
								}
							} 
							//data[j] = data[j]->next;	
						//}
						//printf("\n");
					}  
				}*/
				
				//printf("\"AND\" or \"OR\" are %d is %s\n", whereCondition, andOr[0]);
				//for(int j=0;j<whereCondition;j++){
				//	printf("%s\n", andOr[j]);
				//start to check the condition
				printf("-----start to print the answer-----\n\n");
				//showtable();
				printf("\n");
				
				
				
				//printf("compare all the column form\n");
				//printf("table : %s ; attrbute : %s\n", table[0], table[0]->attrName[attr[0]]);
				//printf("table : %s ; attrbute : %s\n", table[1], table[0]->attrName[attr[1]]);
				//Data* tmpdata[10];
				bool statment[10];
				int statCnt = 0;
				//Data* tmpdata[10];
				memset(statment, 0, 10*sizeof(bool));
				vector<Data*> candidate1;
				vector<Data*> candidate2;
				int typeError = 0;
				//Data* candidate[10000][2];
				
				for(int j=0;j<condition;j+=2){//all table ready to be compare
					//printf("round %d\n", j/2);
					if(table[j] != NULL){
						data[j] = table[j]->dataRoot;
						for(int t1=0;t1<table[j]->dataCount;t1++){
							if(table[j+1] == NULL){//compare with constance
								//printf("compare with number or string\n");
								if(j == 0){
									//printf("%s %s %s?\n", data[j]->attr[attr[j]], keyword[op[j]+1], keyword[op[j]]);
									if(compare(data[j]->attr[attr[j]], keyword[op[j]+1], keyword[op[j]])){
										//printf("%s %s %s!\n", data[j]->attr[attr[j]], keyword[op[j]], keyword[op[j]+1]);
										candidate1.push_back(data[j]);
										candidate2.push_back(data[j]);
										data[j]->fromWhichTable = table[j]->tableName;
									}
								}
								else{
									//printf("j is %d\n", j);
									if(!strcmp(andOr[j/2-1], "and")){//if not the same 
										//remove that data in vector because is different
										//printf("find \"and\" (%s)\n", andOr[j/2-1]);
										//printf("%s %s %s?\n", data[j]->attr[attr[j]], keyword[op[j]], keyword[op[j]+1]);
										if(!compare(data[j]->attr[attr[j]], keyword[op[j]+1], keyword[op[j]])){
											//printf("%s %s %s\n", data[j]->attr[attr[j]], keyword[op[j]], keyword[op[j]+1]);
											for(int v=0;v<candidate1.size();v++){
												//printf("%s %s\n", data[j]->attr[attr[j]], keyword[op[j]+1]);
												//printf("input\n");
												//printf("%s %s %s %s\n", data[j]->attr[0], data[j]->attr[1], data[j]->attr[2], data[j]->attr[3]);
												if(compareData(data[j], candidate1[v]) || compareData(data[j], candidate2[v])){
													//printf("candidate1\n");
													//printf("%s %s %s\n", candidate1[v]->attr[0], candidate1[v]->attr[1], candidate1[v]->attr[2]);
													//printf("candidate2\n");
													//printf("%s %s %s\n", candidate2[v]->attr[0], candidate2[v]->attr[1], candidate2[v]->attr[2]);
													candidate1.erase(candidate1.begin() + v);
													candidate2.erase(candidate2.begin() + v);
													v--;
												}
											}
										}
									}
									else if(!strcmp(andOr[j/2-1], "or")){
										//printf("%s %s %s?\n", data[j]->attr[attr[j]], keyword[op[j]], keyword[op[j]+1]);
										if(compare(data[j]->attr[attr[j]], keyword[op[j]+1], keyword[op[j]])){
											//printf("%s %s %s!!!\n", data[j]->attr[attr[j]], keyword[op[j]], keyword[op[j]+1]);
											int same = 0;
											for(int v=0;v<candidate1.size();v++){
												if(compareData(data[j], candidate1[v])){
													same = 1;
												}
											}
											if(same == 0){
												candidate1.push_back(data[j]);
												candidate2.push_back(data[j]);
												data[j]->fromWhichTable = table[j]->tableName;
											}
										}
										
									}
								}
								
							}
							else{//not null
								//printf("compare with other attribute\n");
								data[j+1] = table[j+1]->dataRoot;
								for(int t2=0;t2<table[j+1]->dataCount;t2++){
									if(j == 0){ 
										//printf("%s %s %s\n", data[j]->attr[attr[j]], data[j+1]->attr[attr[j+1]], keyword[op[j]]);
										if(compare(data[j]->attr[attr[j]], data[j+1]->attr[attr[j+1]], keyword[op[j]])){
											//printf("%s %s %s\n", data[j]->attr[0], data[j]->attr[1], data[j]->attr[2]);	
											//printf("%s %s %s\n", data[j+1]->attr[0], data[j+1]->attr[1], data[j+1]->attr[2]);
												
											//printf("%s %s %s\n", data[j]->attr[attr[j]], keyword[op[j]], data[j+1]->attr[attr[j+1]]); 
											candidate1.push_back(data[j]);
											candidate2.push_back(data[j+1]);
											data[j]->fromWhichTable = table[j]->tableName;
											data[j+1]->fromWhichTable = table[j+1]->tableName;
										}
									}
									else{
										printf("j is %d\n", j);
										if(!strcmp(andOr[j/2-1], "and")){//if not the same 
											//printf("find \"and\" (%s)\n", andOr[j/2-1]);
											//remove that data in vector because is different
											//printf("%s %s %s\n", data[j]->attr[attr[j]], data[j+1]->attr[attr[j+1]], keyword[op[j]]);
											if(compare(data[j]->attr[attr[j]], data[j+1]->attr[attr[j+1]], keyword[op[j]])){
												for(int v=0;v<candidate1.size();v++){
													if(compareData(data[j], candidate1[v]) || compareData(data[j+1], candidate2[v])){
														candidate1.erase(candidate1.begin() + v);
														candidate2.erase(candidate2.begin() + v);
														v--;
													}
												}
											}
											
										}
										else if(!strcmp(andOr[j/2-1], "or")){
											if(!compare(data[j]->attr[attr[j]], data[j+1]->attr[attr[j+1]], keyword[op[j]])){
												int same = 0;
												for(int v=0;v<candidate1.size();v++){
													if(compareData(data[j], candidate1[v]) || compareData(data[j+1], candidate2[v])){
														same = 1;	
													}
												}
												if(same == 0){
													candidate1.push_back(data[j]);
													candidate2.push_back(data[j+1]);
													data[j]->fromWhichTable = table[j]->tableName;
													data[j+1]->fromWhichTable = table[j+1]->tableName;
												}
											} 
											
										}
									} 
									data[j+1] = data[j+1]->next;
								}
								
							}
							data[j] = data[j]->next;
						}
					}
				}
				
				//check what is leave in candidate
				printf("start to find what is leave in candidate vector\n");
				if(count == 1){
					printf("COUNT(%s)\n", countStr);
					printf("%d\n", candidate2.size());
				}
				else{
					Table* tmptable = selectTableRoot->next;
					//printf("%s\n", candidate1[0]->attr[1]);
					for(int j=0;j<selectTableNum;j++){
						//printf("Table : %s\n", tmptable->tableName);
						for(int k=0;k<tmptable->attrNum;k++){
							if(tmptable->isSelect[k] == 1){
								printf("%7s.%-8s ", tmptable->tableName, tmptable->attrName[k]);
								//printf("SELECTED attribute : \"%s\"\n", tmptable->attrName[k]);	
							}
						}
						tmptable = tmptable->next;
					}
					printf("\n");
					
					int total = 0;
					
					//printf("candidate size = %d\n", candidate1.size());
					for(int j=0;j<candidate1.size();j++){
						tmptable = selectTableRoot->next; 
						for(int k=0;k<selectTableNum;k++){
							//printf("-----this table is \"%s\"------\n", tmptable->tableName);
							int find = 0;
							for(int l=0;l<tmptable->attrNum;l++){
								if(tmptable->isSelect[l] == 1){
									//printf("candidate1[%d] is from %s\n", j, candidate1[j]->fromWhichTable);
									//printf("candidate2[%d] is from %s\n", j, candidate2[j]->fromWhichTable);
									if(!strcmp(candidate1[j]->fromWhichTable, tmptable->tableName)){
										//printf("candidate1 is from %s\n", candidate1[j]->fromWhichTable);
										if(sum == 1){
											//if(isdigit(candidate1[j]->attr[l])){//word2 is number
												//printf("original keyword is %s\n", candidate1[j]->attr[l]);
												stringstream w_str( candidate1[j]->attr[l] );
											    int num;
											    w_str >> num;
											    //printf("num = %d\n", num);
											    total += num;
											//}
										}
										else
											printf("%-15s ", candidate1[j]->attr[l]);
									}
									else if(!strcmp(candidate2[j]->fromWhichTable, tmptable->tableName)){
										//printf("candidate2 is from %s\n", candidate2[j]->fromWhichTable);
										if(sum == 1){
											//if(isdigit(candidate2[j]->attr[l])){//word2 is number
												//printf("original keyword is %s\n", candidate2[j]->attr[l]);
												stringstream w_str( candidate2[j]->attr[l] );
											    int num;
											    w_str >> num;
											    //printf("num = %d\n", num);
											    total += num;
											    
											//}
										}
										else
											printf("%-15s ", candidate2[j]->attr[l]);
									}
									find = 1;
									//break;
								}
							}
							if(find == 1){
								tmptable = tmptable->next;
								continue;
							}
							tmptable = tmptable->next;
							
						}
						printf("\n");	
					}
					if(sum == 1){
						printf("SUM(%s)\n", sumStr);
						printf("%d\n", total);
					}
				}
			}
		}
	}
}

