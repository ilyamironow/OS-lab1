#include <stdio.h>
#include <sys/mman.h>
#include <math.h>
#include <pthread.h>

#define B 0xED930667
#define A 232
#define D 20 //threads

void * filler(void *arg) {
    fill(*data);
    return NULL;
}

int main(int agrc, char **argv){
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
	pthread_t thread[D];
	for (int i=0; i<D; i++) {
		pthread_create(&newthread[i], NULL, filler, NULL);
		
	}
	pthread_join(&newthread, NULL);
	return 0;
}
