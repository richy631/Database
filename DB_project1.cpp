#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <cctype>
#include <string>
#include <sstream> 

using namespace std;

void GetInstruction();
int Analysis(char* str, char** keyword);
void showtable();

struct Data{
	char* attr[7];//the real data
	Data* next;
	Data* previous; 
	Data(){
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
	
	
};

Table *root;
Table *tail;

int numofTable = 0;

int main(int argc, char ** argv){
	char c;
	root = new Table;
	tail = root; 
	
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
				printf("%-16s", nowdata->attr[l]);
			}
			printf("\n");
			nowdata = nowdata->next;
		}
		
		nowtable = nowtable->next;
	}	
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
			//str[cnt] = c;
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
						printf("start to insert : %s\n", keyword[i]);
						for(int k=0;k<nowtable->attrNum;k++){
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
										break;
									}
									check = check->next;
								}
							}
							if(conflict == 0){
								//printf("Not primary key!!\n");
								if(nowtable->types[k] == 'v'){//is varchar
									if(strlen(keyword[i]) <= nowtable->varcharLength[k]){
										data->attr[k] = keyword[i++];
										printf("insert : %s\n", keyword[i-1]);
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
		else if(! strcmp(keyword[i], "select")){
			//start to find FROM & WHERE
			int from = 0;
			int where = 0;
			int whereCondition = 0;
			printf("number of word is %d\n", keywordNum);
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
			printf("from = %d, where = %d, whereCondition = %d\n", from, where, whereCondition);
			
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
							fromTable = tmptable;
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
							if( !strcmp(keyword[j+1], "as")){
								j = j+2;//now j is alias
								fromTable->alias = keyword[j][0];
								printf("now %s has alias %c\n", fromTable->tableName, fromTable->alias);
							}
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
							fromTable = tmptable;
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
							if( !strcmp(keyword[j+1], "as")){
								j = j+2;//now j is alias
								fromTable->alias = keyword[j][0];
								printf("now %s has alias %c\n", fromTable->tableName, fromTable->alias);
							}
						}
						tmptable = tmptable->next;
					}
					if(tableFound == 0){
						printf("404 table not found!!\n");
						break;
					}
				}
			} 
			/*Table* nowtable;
			nowtable = fromTableRoot->next;
			for(int j=0;j<fromTableNum;j++){		
				printf("%s\n", nowtable->tableName);
				nowtable = nowtable->next;
			}*/
			
			
			//start to print the query
			//the SELECT PART
			if(!strcmp(keyword[i+1], "*")){
				Table* tmptable = fromTableRoot->next;
				for(int j=0;j<fromTableNum;j++){
					for(int l=0;l<tmptable->attrNum;l++){
						tmptable->isSelect[l] = 1;	
					}
				}
			}
			else if(!strcmp(keyword[i+1], "count")){
				
			}
			else if(!strcmp(keyword[i+1], "sum")){
				
			}
			else{
				for(int j=1;j<from;j++){ 
					Table* tmptable = fromTableRoot->next;
					//if the attribute has prefix table name
					int find = 0;
					//printf("start to find the word %s...\n", keyword[j]);
					int attributeFound = 0;
					for(int k=0;k<fromTableNum;k++){
						//full name or alias
						if(!strcmp(keyword[j], tmptable->tableName) || (keyword[j][0] == tmptable->alias)){
							//find which table
							//then start to find attribute
							attributeFound = 0;
							for(int l=0;l<tmptable->attrNum;l++){
								if(!strcmp(keyword[j+1], tmptable->attrName[l])){
									//printf("in \"SELECT\" part\n");
									//printf("find the attribute!!\n");
									//printf("attribute name = %s\n", tmptable->attrName[l]);
									//printf("it is from table : %s\n\n", tmptable->tableName);
									
									
									attributeFound = 1;
									//find that attribute
									tmptable->isSelect[l] = 1;
									j = j + 1;
									find = 1;
								}	
							}
							if(attributeFound == 0){
								printf("404 attribute not found in table \"%s!!\"\n", tmptable->tableName);
								break;
							}
						}
						if(find) break;
						
						//printf("start to find next table\n");
						tmptable = tmptable->next;
					}
					if(attributeFound == 0)	break;
					if(find) continue;
					
					//printf("there is no table name!!, Just attribute\n");
					
					//if nothing happend then the first word is attribute
					//find where table this attribute from
					tmptable = fromTableRoot->next;
					for(int k=0;k<fromTableNum;k++){
						//printf("now keyword[%d] is %s\n", j, keyword[j]);
						//printf("this table is %s\n", tmptable->tableName);
						
						for(int l=0;l<tmptable->attrNum;l++){
							if(!strcmp(tmptable->attrName[l], keyword[j])){
								//found that attribute
								//printf("in \"SELECT\" part\n");
								//printf("find the attribute!!\n");
								//printf("attribute name = %s\n", tmptable->attrName[l]);
								//printf("it is from table : %s\n\n", tmptable->tableName);
								
								attributeFound = 1;
								tmptable->isSelect[l] = 1;
								j = j + 1;
								find = 1;
							}
						}
						if(find) break;
						tmptable = tmptable->next;
					}
					if(attributeFound == 0){
						printf("404 attribute not found!!\n");
						break;	
					}
					printf("j = %d, from = %d\n", j, from);
				}
				printf("from = %d; where = %d; keywordNum = %d\n", from, where, keywordNum);
				printf("now go to the WHERE part\n\n");
			}
			//the WHERE PART
			//there is no WHERE
			if(!where){
				/*Table *nowtable;
				Data *nowdata;
				nowtable = fromTableRoot->next;
				
				for(int k=0;k<fromTableNum;k++){
					nowdata = nowtable->dataRoot;
					//printf("----------Table %d : %s (%c)----------\n", k+1, nowtable->tableName, nowtable->alias);
					
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
				}*/
			}
			else{//has "WHERE" condition
				Data* data[10];  //just tell us where this attribute from
				Table* table[10];//just tell us where this attribute from
				memset(data, NULL, 10*sizeof(Data*));
				memset(table, NULL, 10*sizeof(Table*));
				char* andOr[5];
				int attr[10];
				printf("there has \"WHERE\"\n");
				int op[5];
				
				int condition = 0;
				int andOrCnt = 0;
				int eqStart = 0;
				for(int j=where+1;j<keywordNum;j++){
					int find = 0;
					int tableFound = 0;
					printf("this word is %s\n", keyword[j]);
					//if the attribute has prefix table name
					Table* tmptable = fromTableRoot->next;
					for(int k=0;k<fromTableNum;k++){
						//full name or alias
						if(!strcmp(keyword[j], tmptable->tableName) || (keyword[j][0] == tmptable->alias)){
							printf("this table is %s\n", tmptable->tableName);
							tableFound = 1;
							printf("start to find attribute : %s\n", keyword[j+1]);
							//find the table
							//then start to find attribute
							int attributeFound = 0;
							for(int l=0;l<tmptable->attrNum;l++){
								if(!strcmp(keyword[j+1], tmptable->attrName[l])){
									//find that attribute
									find = 1;
									attributeFound = 1;
									table[condition] = new Table;
									table[condition] = tmptable;
									data[condition] = new Data;
									data[condition] = table[condition]->dataRoot;
									attr[condition] = l;
									attributeFound = 1;
									printf("there are %d conditions\n", condition);
									//printf("eqStart = %d\n", eqStart);
									
									if(eqStart == 0){
										//printf("equation start, eqStart = %d\n", eqStart);
										//the operator will appear
										//(next keyword will be "=" or something else)
										eqStart = !eqStart;
										if(condition < 2*whereCondition || whereCondition==0){
											op[condition] = j+2;
											printf("op[%d] = \"%s\"\n", condition, keyword[op[condition]]);
											condition ++;
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
											printf("andOr[%d] get %s\n", andOrCnt, keyword[j]); 
											andOrCnt ++;
										}
									}

								}
								if(attributeFound){
									break;	
								}
							}
							if(attributeFound == 0){
								printf("404 attribute not found in table \"%s\"\n", tmptable->tableName);
								break;	
							}
						}
						if(tableFound){
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
								//printf("first data is %s\n", data[condition]->attr[l]);
								attr[condition] = l;
								attributeFound = 1;
								//printf("there are %d conditions\n", condition);
								//printf("eqStart = %d\n", eqStart);
								
								if(eqStart == 0){
									//printf("equation start, eqStart = %d\n", eqStart);
									//the operator will appear
									//(next keyword will be "=" or something else)
									eqStart = !eqStart;
									if(condition < 2*whereCondition || whereCondition==0){
										op[condition] = j+1;
										printf("op[%d] = \"%s\"\n", condition, keyword[op[condition]]);
										condition ++;
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
										printf("andOr[%d] get %s\n", andOrCnt, keyword[j]); 
										andOrCnt ++;
									}
								}
							}
						}
						tmptable = tmptable->next;
					}
					if(find) continue;
					//just a number or string
					printf("condition = %d\n", condition);
					printf("this keyword is %s\n", keyword[j]);
					condition ++;
				}
				printf("condition = %d\n", condition);
				//start to check the condition
				printf("-----start to print the answer-----\n\n");
				//showtable();
				printf("\n\n");
				//printf("compare all the column form\n");
				//printf("table : %s ; attrbute : %s\n", table[0], table[0]->attrName[attr[0]]);
				//printf("table : %s ; attrbute : %s\n", table[1], table[0]->attrName[attr[1]]);
				Data* tmpdata[10];
				tmpdata[0] = data[0];
				for(int x=0;x<whereCondition;x++){
					/*for(int j=0;j<table[0]->dataCount;j++){//table one
						tmpdata[1] = data[1];
						for(int k=0;k<table[1]->dataCount;k++){//table two
							if(!strcmp(op, "=")){
								//printf("attribute name : %s\n", table[0]->attrName[attr[0]]);
								//printf("now, data in table 1 = %s\n", tmpdata[0]->attr[attr[0]]);
								//printf("attribute name : %s\n", table[0]->attrName[attr[1]]);
								//printf("now, data in table 2 = %s\n", tmpdata[1]->attr[attr[1]]);
								if(!strcmp(tmpdata[0]->attr[attr[0]], tmpdata[1]->attr[attr[1]])){
									//printf("these two data is ready to print!!\n");
									for(int l=0;l<table[0]->attrNum;l++){
										if(table[0]->isSelect[l] != 0)
											printf("%-15s", tmpdata[0]->attr[l]);
									}
									for(int l=0;l<table[1]->attrNum;l++){
										if(table[1]->isSelect[l] != 0)
											printf("%-15s", tmpdata[1]->attr[l]);
									}
									printf("\n");
								}
							}
							else if(!strcmp(op, "<>")){
								//printf("these two data is ready to print!!\n");
								if(strcmp(tmpdata[0]->attr[attr[0]], tmpdata[1]->attr[attr[1]])){
									for(int l=0;l<table[0]->attrNum;l++){
										if(table[0]->isSelect[l] != 0)
											printf("%-15s", tmpdata[0]->attr[l]);
									}
									for(int l=0;l<table[1]->attrNum;l++){
										if(table[1]->isSelect[l] != 0)
											printf("%-15s", tmpdata[1]->attr[l]);
									}
									printf("\n");
								}
							}
							else if(!strcmp(op, ">")){
								stringstream s_str1( tmpdata[0]->attr[attr[0]] );
							    int num1;
							    s_str1 >> num1;
							    
							    stringstream s_str2( tmpdata[1]->attr[attr[1]] );
							    int num2;
							    s_str2 >> num2;
							    if(num1 > num2){
							    	for(int l=0;l<table[0]->attrNum;l++){
										if(table[0]->isSelect[l] != 0)
											printf("%-15s", tmpdata[0]->attr[l]);
									}
									for(int l=0;l<table[1]->attrNum;l++){
										if(table[1]->isSelect[l] != 0)
											printf("%-15s", tmpdata[1]->attr[l]);
									}
									printf("\n");
							    }
							}
							else if(!strcmp(op, "<")){
								stringstream s_str1( tmpdata[0]->attr[attr[0]] );
							    int num1;
							    s_str1 >> num1;
							    
							    stringstream s_str2( tmpdata[1]->attr[attr[1]] );
							    int num2;
							    s_str2 >> num2;
							    if(num1 < num2){
							    	for(int l=0;l<table[0]->attrNum;l++){
										if(table[0]->isSelect[l] != 0)
											printf("%-15s", tmpdata[0]->attr[l]);
									}
									for(int l=0;l<table[1]->attrNum;l++){
										if(table[1]->isSelect[l] != 0)
											printf("%-15s", tmpdata[1]->attr[l]);
									}
									printf("\n");
							    }
							}
							tmpdata[1] = tmpdata[1]->next;
							//printf("go to check next data in table 2\n");
						}
						tmpdata[0] = tmpdata[0]->next;
					}*/ 
				}
			}
		}
	}
}

