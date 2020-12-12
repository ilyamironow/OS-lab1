#include <stdio.h>
#include <sys/mman.h>
#include <math.h>
#include <pthread.h>

#define B 0xED930667
#define A 232
#define D 20 //threads

typedef struct {
    char* adr;
    size_t bytes;
    FILE * randomnums;
} Args;

void* filler(void * arg) {
    Args *forthread_ptr = (Args *)arg;
    fread((*forthread_ptr).adr, 1, (*forthread_ptr).bytes, (*forthread_ptr).randomnums);
    return NULL;
}

int main() {
//mapping A megabytes from B address (if possible) using mmap
	void* first = mmap((void*)B, A*pow(2,20), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (first == MAP_FAILED) {
		printf("Bro, we failed");
		return 0;
	}
	else 
		printf("Starting address of mapped area is %p\n", first);
//filling our mapped area with random numbers in D threads
	FILE * randomnums = fopen("/dev/urandom", "rb");
	long block = A*pow(2,20)/D;
	Args forthread[D];
	pthread_t thread[D-1];
	for (int i=0; i<D-1; i++) {
		forthread[i] = {first, block, randomnums};
		pthread_create(&thread[i], NULL, filler, &forthread[i]);
		first+=block;
	}
	//thread number D
	forthread[D-1] = {first, block + A*pow(2,20)%D, randomnums};
	pthread_create(&thread[D-1], NULL, filler, &forthread[D-1]);
//check for the end of filling with random data
	for (int i=0; i<D; i++) {
		pthread_join(&thread[i], NULL);
	}
	flcose(randomnums);
	return 0;
}
