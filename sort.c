#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <dirent.h>
#include <regex.h>
#include <stdlib.h>

regex_t regex;

int stringUnsortable(char* c){
	return regexec( &regex, c, 0, NULL, 0) == 0 ? 0 : 1;
}

int isInstPrevCatas(char* c, char*** cataPtr){
	if(cataPtr == NULL)
		return 0;
	int i = 0;
	//printf("c: %s\n", c);
	//fflush(stdout);
	char *c_cata = malloc(strlen(c) * sizeof(char)); //don't need to allocate the full thing since we are only storing until the numbers
	//printf("orca1\n");
	while((c[i] >= 'a' && c[i] <= 'z') || (c[i] >= 'A' && c[i] <= 'Z')){
		c_cata[i] = c[i];
		i++;
	}
	c_cata[i] = '\0';
	//printf("orca2\n");
	//fflush(stdout);
	i = -1;
	int c_strlen = strlen(c_cata);
	while(cataPtr[++i] != NULL){
		if(strcmp(*(cataPtr[i]), c_cata)){
			free(c_cata);
			return i;
		}
	}
	//printf("orca3\n");
	//fflush(stdout);
	free(c_cata);
	return 0;
}

void printCataPtr(char*** cataPtr){
	if(cataPtr == NULL){
		printf("cataPtr unintialized\n");
		return;
	}
	int i = 0;
	int j;
	//printf("ORCA1\n");
	//printf("Category %s:", cataPtr[0][0]);
	while(cataPtr[i] != NULL){
		j = 1;
		//printf("ORCA2\n");
		//printf("Category %s:", cataPtr[0][0]);
		while(cataPtr[i][j] != NULL){
			printf("%s ", cataPtr[i][j]);
			j++;													//this was i needs to be j
		}
		printf("\n");
		i++;
	}
}

int cataPtrEnd(char*** cataPtr){
	if(cataPtr == NULL)
		return 0;
	int i = 0;
	while(cataPtr[++i] != NULL)
		;
	return i;
}


int main(){
	char dirname[256];
	
	printf("Here is a totally normal string: ^[a-zA-Z]{1,10}[0-9]{1,5}(\\.[0-9]{1,5})?\\.[a-zA-Z0-9]+$\n");
	if(regcomp( &regex, "^[a-zA-Z]{1,10}[0-9]{1,5}(\\.[0-9]{1,5})?\\.[a-zA-Z0-9]+$", REG_EXTENDED)){
	//if(regcomp( &regex, "[0-9]+\\..*", 0)){
		printf("Error compiling regex\n");
		return 1;
	}
	memset(dirname, 0, 256*sizeof(char)); 
	getcwd(dirname, 255);
	//get current working directory c);
	printf("current directory%s\n", dirname);
	DIR *currentDir;
	currentDir = (struct DIR *) opendir(dirname);
	//open a directory
	struct dirent *currentDirent;// = readdir(currentDir);
	//readdir(currentDir);
	char** fileNames = NULL;
	int numFileNames = 0;
	while ((currentDirent = readdir(currentDir)) != NULL) {
		//printf("tesing file: %s\n", currentDirent->d_name);
		if(!strcmp(currentDirent->d_name, "..") || !strcmp(currentDirent->d_name, ".") || currentDirent->d_type == DT_DIR)
			continue;
		if(stringUnsortable(currentDirent->d_name)){
			printf("Unsortable entry: %s\n", currentDirent->d_name);
			continue;
		}
		//printf("first test passed\n");
		numFileNames++;
		fileNames = realloc(fileNames, numFileNames * sizeof(char*));
		if((fileNames) == NULL){
			printf("get moar memory scrub\n");
			exit(2);
		}
		fileNames[numFileNames - 1] = malloc(sizeof(char) * (1+strlen(currentDirent->d_name)));
		if((fileNames[numFileNames - 1]) == NULL){
			printf("get moar memory scrub\n");
			exit(2);
		}
		strcpy(fileNames[numFileNames - 1], currentDirent->d_name);
        //printf("Added file to list: %s\n", fileNames[numFileNames - 1]);   
    }
	char ***cataPtr = NULL;
	for(int i = 0; i<numFileNames; i++){
		printCataPtr(cataPtr);
		//printf("fileNames[0]: %s\n", fileNames[0]);
		//fflush(stdout);
		int j = isInstPrevCatas(fileNames[i], cataPtr);
		if(j!=0){//test it against the others in the category
			printf("testing against others\n");
			int ii = 0;
			while(cataPtr[++ii] != NULL)
				;
			cataPtr[j] = realloc(cataPtr[j], (ii + 1) * sizeof(char*));
			cataPtr[j][ii] = malloc(strlen((fileNames[i]) + 1) * sizeof(char));
			cataPtr[j][ii+1] = NULL;
			strncpy(cataPtr[j][ii], fileNames[i], strlen(fileNames[i]) + 1);
		}
		else{//make new category and put it in
			printf("making new catagory\n");
			j = cataPtrEnd(cataPtr);
			cataPtr = realloc(cataPtr, (j + 2) * sizeof(char**));
			cataPtr[j] = malloc(3 * sizeof(char*));
			cataPtr[j+1] = NULL;
			
			int ii = 0;
			while((fileNames[i][ii] >= 'a' && fileNames[i][ii] <= 'z') || (fileNames[i][ii] >= 'A' && fileNames[i][ii] <= 'Z')){
				ii++;
			}
			cataPtr[j][0] = malloc((ii+1) * sizeof(char));
			//printf("orca2, %s\n", fileNames[i]);
			strncpy(cataPtr[j][0],fileNames[i], ii);
			cataPtr[j][0][ii] = '\0';
			//printf("orca3, %s\n", cataPtr[0][0]);

			//printf("orca1\n");
			cataPtr[j][1] = malloc((1+strlen(fileNames[i])) * sizeof(char));
			//printf("orca2, %s\n", fileNames[i]);
			strcpy(cataPtr[j][1],fileNames[i]);
			//printf("orca3, %s\n", cataPtr[j][1]);
			
			cataPtr[j][2] = NULL;

		}
	}
	//printCataPtr(cataPtr);
	
	
	//finishing up, cleaning memory
	closedir(currentDir);
	for(int i = 0; i < numFileNames; i++){
		free(fileNames[i]);
	}
	free(fileNames);
	regfree(&regex);
	return 0;
}