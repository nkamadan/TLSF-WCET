/*
 ============================================================================
 Name        : wcet.c
 Author      : Nureddin Kamadan
 Description : Worst case execution time calculation for TLSF in C, Ansi-style
 ============================================================================

Based on the article "TLSF: a New Dynamic Memory Allocator for Real-Time Systems"

"The worst case for malloc occurs when there is only one large
free block and the application requests a small block.

And for the free operation it occurs when the released block
has to be coalesced with its two neighbour blocks."
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

struct node {
	void* ptr;
	unsigned long int allocated_size;
};

void cache_flush(long long int cache_size){

	//If you want to flush "all of the cache" for testing reason,
	//malloc() a big block that is larger than CPU's cache (In this function, it is made sizeof(int) times of the input size),
	//write any old garbage into it, and just flush that entire block.
	//In Linux systems, all CPU cache size can be found with the following command: $ lscpu | grep cache

	//int flush_result =  cacheflush(void *addr, int nbytes, int cache);

    int* ptr;
    ptr = (int*) malloc(cache_size * sizeof(int));

    if(ptr==NULL){
      printf("Memory not allocated. \n");
    }
    // Where memory is allocated
    // printf("The address of the pointer is:%u\n ", ptr);

    // Assign values to the ptr created
    for(int i=0;i<cache_size;i++){
      ptr[i] = 123;
    }

    free(ptr);

}

void wcet_malloc(){ //Pre-condition: No block is initally allocated.

	//printf( "WCET for malloc() occurs when no blocks are initially allocated and program requests a small block\n...\n");
	//printf( "In this program, a synthetic workload model is used where initially no memory is allocated and program requests for a 40 byte block\n...\n");

		// This pointer will hold the
	    // base address of the block created
	    void* ptr;

	    // Dynamically allocate memory using malloc()
	    clock_t start, end;
	    double cpu_time_used;

	    start = clock();
	    ptr = malloc(40);
	    end = clock();

	    cpu_time_used = ((double) (end - start)); // CLOCKS_PER_SEC;

	    // Check if the memory has been successfully
	    // allocated by malloc or not
	    if (ptr == NULL) {
	        printf("Failed to allocate memory with malloc().\n");
	        exit(0);
	    }
	    else {

	        // Memory has been successfully allocated
	        printf("wcet_malloc_time:%lf;", cpu_time_used);

	    }
	    //Free the allocated block and flush the cache.
	    free(ptr);

}

void wcet_free(){

	//printf( "WCET for free() occurs when the released block has to be coalesced with its two neighbour blocks\n...\n" );
	//printf( "In this program, a synthetic workload model is used where three 512-byte blocks are allocated, "
			//"then the first and third blocks are released and the time to release the second block is the worst case\n...\n" );

	clock_t start, end;
	double cpu_time_used;

	void* ptr1;
	void* ptr2;
	void* ptr3;

	ptr1 = malloc(512);
	if(ptr1 == NULL){
		printf("Allocation failed for ptr1, try again!");
	}
	else{
		ptr2 = malloc(512);
		if(ptr2 == NULL){
			printf("Allocation failed for ptr2, try again!");
			free(ptr1);
		}
		else{
			ptr3 = malloc(512);
			if(ptr3 == NULL){
				printf("Allocation failed for ptr3, try again!");
				free(ptr1);
				free(ptr2);
			}
			else{
				//All allocations are successful.
				//First free the first allocated block.
				free(ptr1);
				//Then, free the third allocated block.
				free(ptr3);
				//Finally, worst case happens when the second block which needs to be coalesced with its neighbors is freed.
				start = clock();
				free(ptr2);
				end = clock();
				cpu_time_used = ((double) (end - start)); // CLOCKS_PER_SEC;

		        	printf("wcet_free_time:%lf;", cpu_time_used);

			}
		}
	}

}

void average_malloc(){

	clock_t start, end;
	double cpu_time_used=0;
	struct node array[1000]; //Create an array of struct node of size 1000 to make 1000 malloc() allocation
	for(int x=0; x<1000; x++){
		int random_size = rand();
		start = clock();
		array[x].ptr = malloc(random_size);
	    end = clock();
	    cpu_time_used += ((double) (end - start)); // CLOCKS_PER_SEC;
	}


	printf("average_malloc_time:%lf;", cpu_time_used/1000);



	//Free all allocated blocks
	for(int x=0; x<1000; x++){
			free(array[x].ptr);
		}
}

void average_free(){

	clock_t start, end;
	double cpu_time_used=0;
	struct node array[1000]; //Create an array of struct node of size 1000 to make 1000 malloc() allocation
	for(int x=0; x<1000; x++){
		int random_size = rand();
		array[x].ptr = malloc(random_size);
	}

	//Free all allocated blocks
	for(int x=0; x<1000; x++){
			start = clock();
			free(array[x].ptr);
			end = clock();
			cpu_time_used += ((double) (end - start)); // CLOCKS_PER_SEC;
		}
	cpu_time_used = cpu_time_used/1000;

	printf("average_free_time:%lf;", cpu_time_used);

}

void run_all(long long int cache_size){
	wcet_malloc();
	cache_flush(cache_size);
	average_malloc();
	cache_flush(cache_size);
	wcet_free();
	cache_flush(cache_size);
	average_free();
	cache_flush(cache_size);
}

int main(int argc, char* argv[]) {

	int option = 0;
	int cache_size;
	if(argc ==1){
		printf("Missing option!\n");
		fprintf(stderr, "For help: %s [-h]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	while ((option = getopt(argc, argv, "a:h")) != -1) {
	        switch (option) {
				case 'a':
					cache_size = atoi(argv[2]); //cache_size is sent through argv[2]
					run_all(cache_size);
					break;
				case 'h':
					printf("Options:\n");
					printf("-a        Get all average and worst case outputs\n");
					printf("Arguments:\n");
					printf("Provide cache size\n");
					printf("Usage: %s [-a] [cache size]\n", argv[0]);
					break;
				case '?':
					printf("Wrong inputs!\n");
					fprintf(stderr, "For help: %s [-h]\n", argv[0]);
					exit(EXIT_FAILURE);

	        }
	    }

	return EXIT_SUCCESS;

}
