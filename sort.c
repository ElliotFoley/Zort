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

int indexOfCata(char* c, char*** cataPtr){
	if(cataPtr == NULL)
		return -1;
	int i = 0;
	char *c_cata = malloc(strlen(c) * sizeof(char)); //don't need to allocate the full thing since we are only storing until the numbers
	while((c[i] >= 'a' && c[i] <= 'z') || (c[i] >= 'A' && c[i] <= 'Z')){
		c_cata[i] = c[i];
		i++;
	}
	c_cata[i] = '\0';
	i = -1;
	//printf("c_cata: %s\n", c_cata);
	//int c_strlen = strlen(c_cata);
	while(cataPtr[++i] != NULL){
		//printf("testing %s\n", cataPtr[i][0]);
		if(!strcmp(cataPtr[i][0], c_cata)){
			//printf("weewooweewoo bad\n");
			free(c_cata);
			return i;
		}
	}
	free(c_cata);
	return -1;
}

void printCataPtr(char*** cataPtr){
	if(cataPtr == NULL){
		printf("cataPtr unintialized\n");
		return;
	}
	int i = 0;
	int j;
	printf("CataPtr is:\n");
	while(cataPtr[i] != NULL){
		j = 1;
		printf("Category %s: ", cataPtr[i][0]);
		while(cataPtr[i][j] != NULL){
			printf("%s, ", cataPtr[i][j]);
			j++;
		}
		printf("\n");
		i++;
	}
	printf("\n\n");
}

int cataPtrEnd(char*** cataPtr){
	if(cataPtr == NULL)
		return 0;
	int i = 0;
	while(cataPtr[++i] != NULL)
		;
	return i;
}

int compareFunc(const void * a, const void * b){
	int ai = 0;
	int bi = 0;
	while((*(char **)a)[ai] != '.')
		ai++;
	while((*(char **)b)[bi] != '.')
		bi++;
	if(ai > bi)
		return 1;
	if(bi > ai)
		return 0;
	for(int i = 0;i < ai; i++){
		if((*(char **)a)[i] > (*(char **)b)[i])
			return 1;
		if((*(char **)b)[i] > (*(char **)a)[i])
			return 0;
	}
	return 0;
}

int spaceBeforeIndex(char * a){
	int i = 0;
	while((a[i] >= 'a' && a[i] <= 'z') || (a[i] >= 'A' && a[i] <= 'Z'))
		i++;
	int ai = i;
	while(a[ai] != '.')
		ai++;
	return ai;
}

int getIndex(char * a){
	int i = 0;
	while((a[i] >= 'a' && a[i] <= 'z') || (a[i] >= 'A' && a[i] <= 'Z'))
		i++;
	int ai = i;
	while(a[ai] != '.')
		ai++;
	char* astr = malloc((ai - i + 1) * sizeof(char));
	memset(astr, '\0', ai - i + 1);
	strncpy(astr, &(a[i]), ai-i);
	int ans = atoi(astr);
	free(astr);
	return ans;
}

int getSubIndex(char * a){
	int i = 0;
	while((a[i] >= 'a' && a[i] <= 'z') || (a[i] >= 'A' && a[i] <= 'Z'))//Go through category
		i++;
	int ai = i;
	while(a[ai] != '.')//Go through index
		ai++;
	if(a[ai+1] > '9' || a[ai+1] < '0')
		return -1;
	i = ++ai;
	while(a[ai] != '.')//Go through index
		ai++;
	char* astr = malloc((ai - i + 1) * sizeof(char));
	memset(astr, '\0', ai - i + 1);
	strncpy(astr, &(a[i]), ai-i);
	//printf("index string %s, i: %d, ai %d\n", astr, i, ai);
	int ans = atoi(astr);
	free(astr);
	return ans;
}

int conflict(char * a, char * b){
	if(getIndex(a) != getIndex(b))
		return 0;
	int aSubIndex = getSubIndex(a);
	if(aSubIndex == -1)
		return 1;
	int bSubIndex = getSubIndex(b);
	if(bSubIndex == -1 || bSubIndex == aSubIndex)
		return 1;
	return 0;

}

int isConfliction(char ** cata){//This assumes the list is sorted
	int i = 0;
	int curIndex = -1;
	int fallback = 0;
	int conflictions = 0;
	while(cata[++i] != NULL){
		int newIndex = getIndex(cata[i]);
		if(newIndex != curIndex){
			curIndex = newIndex;
			fallback = 0;
			continue;
		}
		fallback++;
		//printf("With index %d, testing %d conflictions behind\n", newIndex, fallback);
		for(int j = 1; j <= fallback; j++){
			if(conflict(cata[i], cata[i-j])){
				fprintf(stderr, "ERROR: %s and %s conflict\n", cata[i], cata[i-j]);
				conflictions++;
			}
		}
	}
	return conflictions;
}

/*fixHoles(char *** cata, int i, int j, currentIndex, previousIndex){
	char* buffer; 
}*/

int indexHoles(char ** cata){//This assumes the list is sorted and there are no conflicts 
	int currentIndex = -1;
	int previousIndex = -1;
	int holeCount = 0;
	int holes = -1;
	for(int j = 1; cata[j] != NULL; j++){
		if(j == 1){
			currentIndex = getIndex(cata[j]);
			if(currentIndex != 1){
				holeCount++;
				holes = currentIndex;
			}
			continue;	
		}
		previousIndex = currentIndex;
		currentIndex = getIndex(cata[j]);
		if(previousIndex + 1 != currentIndex && previousIndex != currentIndex){
			printf("there is a hole between %s and %s\n", cata[j-1], cata[j]);
			holeCount++;
			if(holeCount == 1){
				holes = currentIndex;
			}
		} 
	}

	return holes;
}

int amountOfEntriesInCata(char ** cata){
	int i = 0;
	while(cata[i++] != NULL)
		;
	return (i - 2);
}

int amountOfCatas(char *** cata){
	int i = 0;
	while(cata[i++] != NULL)
		;
	return (i - 1);
}

int renameFile(char* dir, char* oldFileName, char* newFileName){
	char newDirPlusFileName[256];
	char oldDirPlusFileName[256];
    strcat(oldDirPlusFileName, dir);
	strcat(newDirPlusFileName, dir);
	strcat(oldDirPlusFileName, '/');
	strcat(newDirPlusFileName, '/');
	strcat(oldDirPlusFileName, oldFileName);
	strcat(newDirPlusFileName, newFileName);
	rename(oldDirPlusFileName,newDirPlusFileName);
}


int main(){
	char dirname[256];
	
	if(regcomp( &regex, "^[a-zA-Z]{1,10}[0-9]{1,5}(\\.[0-9]{1,5})?\\.[a-zA-Z0-9]+$", REG_EXTENDED)){
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
	char** fileNames = NULL;
	int numFileNames = 0;
	while ((currentDirent = readdir(currentDir)) != NULL) {
		if(!strcmp(currentDirent->d_name, "..") || !strcmp(currentDirent->d_name, ".") || currentDirent->d_type == DT_DIR)// WARNING OF STRCMP
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
    }
	char ***cataPtr = NULL;
	for(int i = 0; i<numFileNames; i++){
		int j = indexOfCata(fileNames[i], cataPtr);
		if(j!=-1){//it is in a category
			int ii = 0;
			while(cataPtr[j][++ii] != NULL)
				;
			//ii is the index of the NULL character
			cataPtr[j] = realloc(cataPtr[j], (ii + 2) * sizeof(char*));
			cataPtr[j][ii] = malloc(strlen((fileNames[i]) + 1) * sizeof(char));
			cataPtr[j][ii+1] = NULL;
			strcpy(cataPtr[j][ii], fileNames[i]);
		}
		else{//make new category and put it in
			//printf("making new catagory\n");
			j = cataPtrEnd(cataPtr);
			cataPtr = realloc(cataPtr, (j + 2) * sizeof(char**));
			cataPtr[j] = malloc(3 * sizeof(char*));
			cataPtr[j+1] = NULL;
			
			int ii = 0;
			while((fileNames[i][ii] >= 'a' && fileNames[i][ii] <= 'z') || (fileNames[i][ii] >= 'A' && fileNames[i][ii] <= 'Z')){
				ii++;
			}
			cataPtr[j][0] = malloc((ii+1) * sizeof(char));
			strncpy(cataPtr[j][0],fileNames[i], ii);
			cataPtr[j][0][ii] = '\0';

			cataPtr[j][1] = malloc((1+strlen(fileNames[i])) * sizeof(char));
			strcpy(cataPtr[j][1],fileNames[i]);
			
			cataPtr[j][2] = NULL;

		}
	}

	//Sorting the list
	int i = -1;
	while(cataPtr[++i] != NULL){
		int ii = 0;
		while(cataPtr[i][++ii] != NULL)
			;
		qsort(&(cataPtr[i][1]), ii-1, sizeof(char *), compareFunc);
	}
	printf("Sorted cataPtr\n");
	printCataPtr(cataPtr);

	//Detection confliction errors	
	i = -1;
	int conflictionCounter = 0;
	while(cataPtr[++i] != NULL){
		printf("Testing category %s\n", cataPtr[i][0]);
		conflictionCounter += isConfliction(cataPtr[i]);
	}
	
	if(conflictionCounter == 0){
		int amountCatas = amountOfCatas(cataPtr);
		int *holes;
		holes = malloc(sizeof(int) * amountCatas + 1);
		holes[amountCatas] = -1;
		for(int i = 0; cataPtr[i] != NULL; i++){
			holes[i] = indexHoles(cataPtr[i]);	
		}
		for(int i = 0; i < amountCatas; i++){
			if(holes[i] != -1){
				printf("This is the index of a hole: %d in cata %s\n", holes[i], cataPtr[i][0]);
			}
		}

		free(holes);
				
	}	


	//finishing up, cleaning memory
	closedir(currentDir);
	for(int i = 0; i < numFileNames; i++){
		free(fileNames[i]);
	}
	free(fileNames);
	regfree(&regex);

	return 0;
}
