#include<stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

int comparator(const void *p, const void *q)
{
    return *(const int *)p - *(const int *)q;
}

int main(int argc, char *argv[]){
    char *inputFileName = "input.bin";
    char *sortedFileName = "sort.bin";
    FILE *inputFile, *sortedFile;
    int n = 35000;
    if(argc == 2){
        n = atoi(argv[1]);
    }
    int ar[n];
    for (int i = 0; i < n; ++i) {
        ar[i] = rand();
    }
    inputFile = fopen(inputFileName, "w+b");
    fwrite(ar, sizeof(int), n, inputFile);
    fclose(inputFile);
    qsort(ar, n, sizeof(int), comparator);
    sortedFile = fopen(sortedFileName, "w+b");
    fwrite(ar, sizeof(int), n, sortedFile);
    fclose(sortedFile);
}