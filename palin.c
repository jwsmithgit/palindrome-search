/*
* palin.c
* CSC 360 assignment 1
* Jacob Smith
* V00700979
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include <time.h>*/

#define MAX_LINE_LEN	30

/* timers */
/*clock_t begin, end;
double time_spent;*/

/*  reads in the words from text file
*	to dynamic character array
*   inspired by stackoverflow
*/
int readFile(char * **words_p, int lines_allocated ){
	/* dereference words pointer */
	char ** words = *words_p;
	/* file pointer */
    FILE *fp = fopen("words", "r");
    if (fp == NULL){
        fprintf(stderr,"Error opening file.\n");
        exit(2);
	}
	/* go through every line in file */
	int i;
    for (i=0;1;i++){
        int j;

        /* over currently allocated size */
        if (i >= lines_allocated){
            int new_size;

            /* double allocation and re-allocate */
            new_size = lines_allocated*2;
            words = (char **)realloc(words,sizeof(char*)*new_size);
            if (words==NULL){
                fprintf(stderr,"Out of memory.\n");
                exit(3);
            }
            lines_allocated = new_size;
        }

        /* allocate space for next word */
        words[i] = (char *) malloc(sizeof(char) * MAX_LINE_LEN);
        if (words[i]==NULL){
            fprintf(stderr,"Out of memory.\n");
            exit(4);
        }
		
		/* reached end of file, break out of loop */
        if (fgets(words[i],MAX_LINE_LEN-1,fp)==NULL)
            break;

        /* get rid of \n and \rat end of line */
		/* start from end of word, work backwards to null character */
        for ( j=strlen(words[i]) -1; j>=0 && (words[i][j]=='\n' || words[i][j]=='\r'); j--);
        words[i][j+1]='\0';
   	}

	/* print all lines
    int j;
    for(j = 0; j < i; j++)
        printf("%s\n", words[j]);*/
	
	/* point words pointer to new data */
	*words_p = words;
	return i;
}

/* reverses the parameter string
 inspired by stackoverflow */
char * reverseString( char * str ){
	char *start = str;
    char *end = start + strlen(str) - 1; /* -1 for \0 */
    char temp;

    while (end > start)
    {
    	/* swap calues in pointers */
    	temp = *start;
    	*start = *end;
    	*end = temp;

    	/* move start and end pointers */
    	++start;
    	--end;
    }
		
	/*printf("%s\n", str);*/
	return str;
} 


/* binary searches through words of size for word
*/
int binaryStringSearch(char *word, char**words, int size){
	
	int first = 0;
	int last = size - 1;
	int middle = (first+last)/2;

	while( first <= last ){
		
		if ( strcmp(word, words[middle]) > 0 )
			first = middle + 1;    
		else if ( strcmp(word, words[middle]) == 0 ){
			/* found */
			/*printf("%s - at line %d.\n", words[middle], middle+1); */
			return middle+1;
		}
		else
			last = middle - 1;

	middle = (first + last)/2;
	}
	/* not found */
	return 0;   
}

/* searches words_p string array for palindromes 
*  and stores in palins_p string array */
int searchPalins( char * **words_p, int num_words, char * **palins_p, int lines_allocated ){
	/* dereference palins and words */
	char ** palins = *palins_p;
	char ** words = *words_p;
	int n = 0; /* number of palins */
	
	/* lastWord checks for duplicates */
	/*char lastWord[MAX_LINE_LEN] = "";*/
	/* go through each word in words array */
	/* only i gets added to palins array to maintain sorted order */
	int i;
	for(i=0;i<num_words;i++){
		
		/* reverse word */
		/*char *rWord;*/
		char * rWord = (char *) malloc(sizeof(char) * MAX_LINE_LEN);
		if (rWord==NULL){
			fprintf(stderr,"Out of memory.\n");
			exit(3);
		}

		strcpy( rWord, words[i]);
		rWord = reverseString(rWord);
		/*printf("%d\n",i);
		printf("%s\n",words[i]);*/

		/* make sure this word isn't a dupe of the last word */
		/*if( strcmp( words[i], lastWord) != 0 ){*/
			
			if ( binaryStringSearch( rWord, words, num_words ) != 0 ){
					/* over currently allocated size */
					if (n >= lines_allocated){
						int new_size;

						/* double allocation and re-allocate */
						new_size = lines_allocated*2;
						palins = (char **)realloc(palins,sizeof(char*)*new_size);
						if (palins==NULL){
							fprintf(stderr,"Out of memory.\n");
							exit(3);
						}
						lines_allocated = new_size;
					}
					/* allocate space for palindrome */
					palins[n] = (char *) malloc(sizeof(char) * MAX_LINE_LEN);
					if (palins[n]==NULL){
						fprintf(stderr,"Out of memory.\n");
						exit(4);
					}
					
					/* copy to palindromes array */
					strcpy( palins[n], words[i] );
					/* copy to lastWord string to check for duplicates */
					/*strcpy( lastWord, words[i] );*/
					
					printf("%s\n", palins[n]);
					/* printf("%d\n", n); */

					n++;
			}

			/* go through each word again to check for palindromes */
			/* only i gets added to palins array to maintain sorted order */
			/*int j;
			for(j=0;j<num_words;j++){
				if( strcmp(rWord, words[j]) == 0){
        			
					*//* over currently allocated size */
					/*if (n >= lines_allocated){
						int new_size;

						*//* double allocation and re-allocate */
						/*new_size = lines_allocated*2;
						palins = (char **)realloc(palins,sizeof(char*)*new_size);
						if (palins==NULL){
							fprintf(stderr,"Out of memory.\n");
							exit(3);
						}
						lines_allocated = new_size;
					}
					*//* allocate space for palindrome */
					/*palins[n] = malloc(MAX_LINE_LEN);
					if (palins[n]==NULL){
						fprintf(stderr,"Out of memory.\n");
						exit(4);
					}
					
					*//* copy to palindromes array */
					/*strcpy( palins[n], words[i] );
					*//* copy to lastWord string to check for duplicates */
					/*strcpy( lastWord, words[i] );
					
					printf("%s\n", palins[n]);
					n++;
					break;
				}
			}*/
			
		/*}*/
	}
	
	/*int j;
    for(j=0; j<n; j++)
        printf("%s\n", palins[j]);*/

	/* point palins pointer to new data */
	*palins_p = palins;

	return n;
}

int main(){
	/*begin = clock();*/

    int lines_allocated = 128;

    /* allocate memory for words array*/
    char **words = (char **)malloc(sizeof(char*)*lines_allocated);
    if (words==NULL){
        fprintf(stderr,"Out of memory (1).\n");
        exit(1);
    }
	
	/* allocate memory for palindrom array */
	char **palins = (char **)malloc(sizeof(char*)*lines_allocated);
	if (palins==NULL){
		fprintf(stderr,"Out of memory (1).\n");
		exit(1);
	}

	/* read file */
	int num_words = readFile( &words, lines_allocated );
	/* search for palindromes */
	int num_palins = searchPalins( &words, num_words, &palins, lines_allocated );

	/*printf("number of words: %d\n", num_words);
	printf("number of palins: %d\n", num_palins);*/
    /* free memory */
	int i;
    for (i=0;i<num_words;i++)
        free(words[i]);
    free(words);

    for (i=0;i<num_palins;i++)
        free(palins[i]);
	free(palins);
	
	/*end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("%lf\n", time_spent);*/

	return 0;
}

