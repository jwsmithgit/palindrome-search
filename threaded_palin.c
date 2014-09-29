/*
* threaded_palin.c
* CSC 360 assignment 1
* Jacob Smith
* V00700979
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_LINE_LEN	30
#define N	5
#define	BUFFER	10

/* timers */
/*clock_t begin, end;
double time_spent;*/

/* semaphores */
sem_t mutex; /* binary semaphore */
sem_t empty;
sem_t full;

/* buffer and palindrome arrays */
char **buffer;
char **palins;

int num_palins = 0;
int lines_allocated = 128;

int threads_finished = 0;

int pin = 0; // index for producer to produce
int cout = 0; // index for consumer to consume
/*int pid = 0; // id for producers
int cid = 0; // id for consumers*/


typedef struct {
	char **words;
	int num_words;
	/*char * **palins;
	int *num_palins;
	int *lines_allocated;*/
	int start_pos;
	int end_pos;
} thread_args_t;



/*  reads in the words from text file
*	to dynamic character array
*   inspired by stackoverflow
*/
int readFile(char * **words_p ){
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

        /* get rid of \n and \r at end of line */
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

/* reverses the string
 inspired by stackoverflow */
char * reverseString( char * str ){
	char *start = str;
    char *end = start + strlen(str) - 1; /* -1 for \0 */
    char temp;

    while (end > start)
    {
    	/* swap values of pointers*/
    	temp = *start;
    	*start = *end;
    	*end = temp;

    	/* move pointers */
    	++start;
    	--end;
    }
		
	/*printf("%s\n", str);*/
	return str;
} 

/* binary searches for a word in words of size size
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

/* Producers */
void *produce(char *word){
	
	/*int id = ++pid;*/
	sem_wait(&empty);
	sem_wait(&mutex);
	
	/* critical section */
	/*printf("%s\n", word);*/
	
	/* place word in buffer */
	buffer[pin] = word;
	pin = (pin +1) % BUFFER;	

	sem_post(&mutex);
	sem_post(&full);

	return NULL;
}

/* Consumers */
void *consume(){

	char *item;
	/*int id = ++cid;*/
	while(1){
		sem_wait(&full);
		sem_wait(&mutex);
	
		if(threads_finished==1){
			sem_post(&mutex);
			break;
		}

		/* grab item from buffer */
		item = buffer[cout];
		cout = (cout + 1) % BUFFER;
		
		/*printf("%d\n", num_palins);
		printf("%d\n", lines_allocated); */
		/* over currently allocated size */
		if (num_palins >= lines_allocated){
			int new_size;
			/* double allocation and re-allocate */
			new_size = (lines_allocated)*2;
			palins = (char **)realloc(palins,sizeof(char*)*new_size);
			if (palins==NULL){
				fprintf(stderr,"Out of memory.\n");
				exit(3);
			}
			lines_allocated = new_size;
		}

		/* allocate space for palindrome */
		palins[num_palins] = (char *) malloc(sizeof(char) * MAX_LINE_LEN);
		if (palins[num_palins]==NULL){
			fprintf(stderr,"Out of memory.\n");
			exit(4);
		}
		
		/* copy to palindromes array */
		strcpy( palins[num_palins], item );
		
		num_palins = num_palins + 1;

		sem_post(&empty);
		sem_post(&mutex);
	}

	return NULL;
}

/* searches words_p string array for palindromes 
*  and stores in palins_p string array */
void *searchPalins( void *args ){
	thread_args_t *arg;
	arg = (thread_args_t *)args;
	char **words = arg->words;
	int num_words = arg->num_words;
	/*char * **palins = arg->palins;
	int * num_palins = arg->num_palins;
	int * lines_allocated = arg->lines_allocated;*/
	int sp = arg->start_pos;
	int ep = arg->end_pos;
	
	/* lastWord checks for duplicates */
	/*char lastWord[MAX_LINE_LEN] = "";*/

	/* go through each word in words array */
	/* only i gets added to palins array to maintain sorted order */
	int i;
	for(i=sp;i<=ep;i++){
		
		/* reverse word */
		/*char * rWord = (char *) malloc(MAX_LINE_LEN);
		if (rWord==NULL){
			fprintf(stderr,"Out of memory.\n");
			exit(3);
		}*/
		char rWord[MAX_LINE_LEN] = "";
	
		strcpy( rWord, words[i]);
		strcpy( rWord, reverseString(rWord));

		/* make sure this word isn't a dupe of the last word */
		/*if( strcmp( words[i], lastWord) != 0 ){*/
			
			if ( binaryStringSearch( rWord, words, num_words ) != 0 ){
					/*sem_wait(&mutex);*/
					/* over currently allocated size */
					/*if (*num_palins >= *lines_allocated){
						int new_size;*/
						/* double allocation and re-allocate */
						/*new_size = *(lines_allocated)*2;
						*palins = (char **)realloc(*palins,sizeof(char*)*new_size);
						if (*palins==NULL){
							fprintf(stderr,"Out of memory.\n");
							exit(3);
						}
						*lines_allocated = new_size;
					}*/
		
					/* allocate space for palindrome */
					/**palins[*num_palins] = (char *) malloc(sizeof(char *) * MAX_LINE_LEN);
					if (*palins[*num_palins]==NULL){
						fprintf(stderr,"Out of memory.\n");
						exit(4);
					}*/
					
					/* copy to palindromes array */
					/*strcpy( *palins[*num_palins], words[i] );*/
					
					/*produce_args_t pargs1 = { .pWord = words[i] };*/
					produce(words[i]);

					/* copy to lastWord string to check for duplicates */
					/*strcpy( lastWord, words[i] );*/

					/*num_palins = num_palins + 1;*/
					/*sem_post(&mutex);*/
			}
		/*}*/
	}

	
	/*int j;
    for(j=0; j<num_palins; j++)
        printf("%s\n", palins[j]);*/

	/* point palins pointer to new data */
	/**palins_p = palins;
	arg->palins = palins_p;*/
	/*return num_palins;*/
	/*return((void *) args);*/

	return NULL;
}

/* compare function for qsort */
int compare(const void* a, const void* b){
	const char *pa = *(const char **)a;
	const char *pb = *(const char **)b;
	return strcmp(pa, pb);
}


int main(){
	/*begin = clock();*/

    /* allocate memory for words array*/
    char **words = (char **)malloc(sizeof(char*)*lines_allocated);
    if (words==NULL){
        fprintf(stderr,"Out of memory (1).\n");
        exit(1);
    }
	
	/* allocate memory for palindrome array */
	palins = (char **)malloc(sizeof(char*)*lines_allocated);
	if (palins==NULL){
		fprintf(stderr,"Out of memory (1).\n");
		exit(1);
	}

	/* allocate memory for buffer array */
	buffer = (char **)malloc(sizeof(char*)*BUFFER);
	if (palins==NULL){
		fprintf(stderr,"Out of memory (1).\n");
		exit(1);
	}
	int i;
	for(i=0;i<BUFFER;i++){
		buffer[i] = (char *)malloc(sizeof(char)*BUFFER);
		if (palins==NULL){
			fprintf(stderr,"Out of memory (1).\n");
			exit(1);
		}
	}
	

	/* read words in */
	int num_words = readFile( &words);
	/* int num_palins = searchPalins( &words, num_words, &palins, lines_allocated ); */

	lines_allocated = 128; // reset

	/* start pointers for each thread */
	/* divides work load */
	int sp1 = 0;
	int sp2 = (num_words/5);
	int sp3 = (num_words/5) * 2;
	int sp4 = (num_words/5) * 3;
	int sp5 = (num_words/5) * 4;

	/* end pointers for each thread */
	int ep1 = ((num_words/5)) -1;
	int ep2 = ((num_words/5) * 2) -1;
	int ep3 = ((num_words/5) * 3) -1;
	int ep4 = ((num_words/5) * 4) -1;
	int ep5 = ((num_words)) -1;

	/* initialize threads */
	pthread_t thread1, thread2, thread3, thread4, thread5;
	/*pthread_t threads1[N];*/
	pthread_t threads2[N];
	int ret[N];

	/* initialize semaphores */
	sem_init(&mutex, 0, 1);
	sem_init(&empty, 0, BUFFER);
	sem_init(&full, 0, 0);

	// N producers
	/*for(i=0;i< N ;i++) {
          ret[i]= pthread_create(&threads1[i], NULL, produce, (void *) (&i));
          if(ret[i] != 0) {
                 printf("Producer %d Creation Fails.\n", i);
                 exit(1);
          }
    }*/
	// producers
	thread_args_t args1 = { .words = words, .num_words = num_words,/* .palins = &palins, .num_palins = &num_palins, .lines_allocated = &lines_allocated, */.start_pos = sp1, .end_pos = ep1 };
	pthread_create(&thread1, 0, searchPalins, (void *)&args1);
	thread_args_t args2 = { .words = words, .num_words = num_words,/* .palins = &palins, .num_palins = &num_palins, .lines_allocated = &lines_allocated, */.start_pos = sp2, .end_pos = ep2  };
	pthread_create(&thread2, 0, searchPalins, (void *)&args2);
	thread_args_t args3 = { .words = words, .num_words = num_words,/* .palins = &palins, .num_palins = &num_palins, .lines_allocated = &lines_allocated, */.start_pos = sp3, .end_pos = ep3  };
	pthread_create(&thread3, 0, searchPalins, (void *)&args3);
	thread_args_t args4 = { .words = words, .num_words = num_words,/* .palins = &palins, .num_palins = &num_palins, .lines_allocated = &lines_allocated, */.start_pos = sp4, .end_pos = ep4  };
	pthread_create(&thread4, 0, searchPalins, (void *)&args4);
	thread_args_t args5 = { .words = words, .num_words = num_words,/* .palins = &palins, .num_palins = &num_palins, .lines_allocated = &lines_allocated, */.start_pos = sp5, .end_pos = ep5  };
	pthread_create(&thread5, 0, searchPalins, (void *)&args5);
	

	// N Consumers
	for(i = 0; i < N; i++) {
          ret[i]= pthread_create(&threads2[i], NULL, consume, NULL);
          if(ret[i] != 0) {
                 printf("Customer %d Creation Fails.\n", i);
                 exit(1);
          }
    }

	/* wait for producer threads to finish */
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);
	pthread_join(thread4, NULL);
	pthread_join(thread5, NULL);

	threads_finished = 1;
	sem_post(&full);
	sem_post(&full);
	sem_post(&full);
	sem_post(&full);
	sem_post(&full);

	/* wait for consumer threads to finish */
	for(i=0;i< N ;i++) {
          /*pthread_join(threads1[i], NULL);*/
          pthread_join(threads2[i],NULL);
    }


	/* destroy semaphores */
	sem_destroy(&mutex);
	sem_destroy(&empty);
	sem_destroy(&full);

	/*printf("number of words: %d\n", num_words);
	printf("number of palins: %d\n", num_palins);*/

	/* sort palins */
	/*char * ** palins_p = &palins;*/

	qsort( palins, num_palins, sizeof(char *), compare );

	/* print output */
	for(i=0;i<num_palins;i++){
		printf("%s\n", palins[i]);
	}

    /* free memory */
	/*int i;*/
    /*for (i=0;i<num_words;i++)
        free(words[i]);
    free(words);

    for (i=0;i<num_palins;i++)
        free(palins[i]);
	free(palins);

    for (i=0;i<BUFFER;i++)
        free(buffer[i]);
	free(buffer);*/

		
	/*end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("%lf\n", time_spent);*/

	return 0;
}

