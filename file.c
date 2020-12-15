#include <stdio.h>
#include <sys/mman.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>

#define B 0xED930667
#define A 232//232
#define D 20 //threads
#define E 116 //file sizes in MB
#define G 95 //IO block size

int check = 0;
long long secheck = 0;

typedef struct {
    char *adr;
    size_t bytes;
    char *randomnums;
} Args;

void *filler(void *arg) {
    Args *forthread_ptr = (Args *) arg;
    FILE *fdr = fopen(forthread_ptr->randomnums, "rb");
    check += fread(forthread_ptr->adr, 1, forthread_ptr->bytes, fdr);
    fclose(fdr);
    return NULL;
}

int main() {
//mapping A megabytes from B address (if possible) using mmap
    void *first = mmap((void *) B, A * pow(2, 20), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (first == MAP_FAILED) {
        printf("Bro, we failed");
        return 0;
    } else
        printf("Starting address of mapped area is %p\n", first);
//filling our mapped area with random numbers in D threads
    char *randomnums = "/dev/urandom";
    long block = A * pow(2, 20) / D;
    Args forthread[D];
    pthread_t thread[D - 1];
    for (int i = 0; i < D - 1; i++) {
        forthread[i].adr = first;
        forthread[i].bytes = block;
        forthread[i].randomnums = randomnums;
        pthread_create(&thread[i], NULL, filler, &forthread[i]);
        first += block;
    }
    //thread number D
    forthread[D - 1].adr = first;
    forthread[D - 1].bytes = block + (int)(A * pow(2, 20)) % D;
    forthread[D - 1].randomnums = randomnums;
    pthread_create(&thread[D - 1], NULL, filler, &forthread[D - 1]);
//check for the end of filling with random data
    for (int i = 0; i < D; i++) {
        pthread_join(thread[i], NULL);
    }
    //printf("Nu vot");
    printf("%.0f should be filled, but was filled %d\n", A * pow(2, 20), check);
    //printf("Nu vot");
//putting our area of data in files
    //fwrite ...
    //2 potoka: chitat' i zapisivat'; posledovatel'no to est' v nachalo faila potom v sled block, sled block po G byte poka ne zapolnu
    //potoki cherez pthread create
    //vo vremya etogo vsego dolzhni rabotat' potoki kotoriye pishut v oblast' pamyati iz /dev/urandom
    //printf("Nu vot");
    for (int k = 1; secheck < A * pow(2, 20); k++) {
    	//printf("Nu vot");
        char* filename = malloc(sizeof(char) * 11);
        snprintf(filename, 11, "file_%d.bin", k);
        int fileBlockCount = E * pow(2, 20) / G;
        FILE *fd = fopen(filename, "wb");
        //if (fd == NULL)
        for (int i = 0; i < fileBlockCount; i++) {
            if (i == fileBlockCount - 1) {
                secheck += fwrite(first - block * (D - 1), 1, E * pow(2, 20) - G * fileBlockCount, fd);
            } else {
                secheck += fwrite(first - block * (D - 1), 1, G, fd);
            }
        }
        fclose(fd);
    }

//unmapping our area of A MB
    munmap(first - block * (D - 1), A * pow(2, 20));
    return 0;
}
