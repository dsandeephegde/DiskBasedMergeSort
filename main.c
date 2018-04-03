#include<stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

int comparator(const void *p, const void *q);

int getNumberOfElements(FILE *file);

char *getRunFileName(const char *inputFileName, int run);

void mergeRuns(int startRun, int endRun, FILE *outFile, const char *runFilePrefix, int *outBuffer, int *inBuffer, int mergeRuns);

char *getSuperRunPrefix(const char *inputFileName) ;

void multiStepMerge(FILE *out, const char *inputFileName, int *outBuffer, int *inBuffer, int runs);

int replacementSelect(int *inputBuffer, int *outBuffer, FILE *fp, FILE *runFile, int *remainingElements, int *bIndex,
                      int *bSize);

int main(int argc, char *argv[]){
    FILE *fp;
    FILE *out;
    FILE *runFile;
    char *outFileName, *inputFileName, *sortMethod;
    int outBuffer[1000], inBuffer[1000];
    struct timeval startTime, endTime, execTime;
    int runFileNumber;
    if(argc != 4){
        printf("Enter Correct number of arguments, Usage: assn_3 --basic input.bin sort.bin");
        exit(0);
    }

    sortMethod = argv[1];
    inputFileName = argv[2];
    outFileName = argv[3];
    fp = fopen(inputFileName, "r+b");
    int numberOfElements = getNumberOfElements(fp);

    gettimeofday(&startTime, NULL);
    int remainingElements = numberOfElements;
    int runs = (int) ceil((double)numberOfElements / 1000);
    out = fopen(outFileName, "w+b");
    if(remainingElements < 1000){
        fread(inBuffer, sizeof( int ), remainingElements, fp);
        qsort(inBuffer, remainingElements, sizeof(int), comparator);
        fwrite(inBuffer, sizeof(int), remainingElements, out);
        fclose(out);
    } else {
        if (strcmp(sortMethod, "--replacement") != 0) {
            runFileNumber = 0;
            while (remainingElements > 1000) {
                fread(inBuffer, sizeof(int), 1000, fp);
                remainingElements -= 1000;
                qsort(inBuffer, 1000, sizeof(int), comparator);
                char *runFileName = getRunFileName(inputFileName, runFileNumber++);
                runFile = fopen(runFileName, "w+b");
                fwrite(inBuffer, sizeof(int), 1000, runFile);
                fclose(runFile);
            }
            if (remainingElements != 0) {
                fread(inBuffer, sizeof(int), remainingElements, fp);
                qsort(inBuffer, remainingElements, sizeof(int), comparator);
                char *runFileName = getRunFileName(inputFileName, runFileNumber);
                runFile = fopen(runFileName, "w+b");
                fwrite(inBuffer, sizeof(int), remainingElements, runFile);
                fclose(runFile);
            }
            fclose(fp);
            if (strcmp(sortMethod, "--basic") == 0) {
                mergeRuns(0, runs-1, out, inputFileName, outBuffer, inBuffer, runs);
            } else if (strcmp(sortMethod, "--multistep") == 0) {
                multiStepMerge(out, inputFileName, outBuffer, inBuffer, runs);
            } else {
                printf("Incorrect Sort Method");
                exit(1);
            }
        } else {
            if (remainingElements >= 1000) {
                fread(inBuffer, sizeof(int), 1000, fp);
                remainingElements -= 1000;
            } else {
                fread(inBuffer, sizeof(int), remainingElements, fp);
                remainingElements = 0;
            }
            int bIndex = 750, bSize = 250, pending = 0;
            runFileNumber = 0;
            do {
                char *runFileName = getRunFileName(inputFileName, runFileNumber);
                runFile = fopen(runFileName, "w+b");
                pending = replacementSelect(inBuffer, outBuffer, fp, runFile, &remainingElements, &bIndex, &bSize);
                fclose(runFile);
                runFileNumber++;
            } while (pending == 0);
            if (pending > 0 && pending != 750) {
                char *runFileName = getRunFileName(inputFileName, runFileNumber);
                runFile = fopen(runFileName, "w+b");
                qsort(inBuffer + pending, 750 - pending, sizeof(int), comparator);
                fwrite(inBuffer + pending, sizeof(int), 750 - pending, runFile);
                fclose(runFile);
                runFileNumber++;
            }
            fclose(fp);
            mergeRuns(0, runFileNumber-1, out, inputFileName, outBuffer, inBuffer, runs);
        }
    }
    fclose(out);
    gettimeofday(&endTime, NULL);
    timersub(&endTime, &startTime, &execTime);
    printf("Time: %ld.%06ld\n", execTime.tv_sec, (long) execTime.tv_usec);
    return 0;
}

void swap( int *array, int i, int j) {
    int temp;
    temp = array[i];
    array[i] = array[j];
    array[j] = temp;
}

void sift(int *array, int index, int n) {
    int left, right, smallIndex;
    while (index < n/2){
        left = index*2 + 1;
        right = left + 1;
        if(right < n && array[right] <= array[left]){
            smallIndex = right;
        } else{
            smallIndex = left;
        }
        if(array[smallIndex] >= array[index]) {
            return;
        }
        swap(array, smallIndex, index);
        index = smallIndex;
    }
}

void heapify(int *array, int n){
    int i;
    for (i = n/2; i >= 0; i--) {
        sift(array, i, n);
    }
}

int replacementSelect(int *inputBuffer, int *outBuffer, FILE *fp, FILE *runFile, int *remainingElements, int *bIndex,
                      int *bSize) {
    int outIndex = 0;
    heapify(inputBuffer, 750);
    int hSize = 750, pIndex = 0;
    while(hSize != 0){
        outBuffer[outIndex++] = inputBuffer[0];
        if(inputBuffer[0] <= inputBuffer[*bIndex]){
            inputBuffer[0] = inputBuffer[*bIndex];
        } else{
            inputBuffer[0] = inputBuffer[hSize - 1];
            inputBuffer[hSize - 1] = inputBuffer[*bIndex];
            hSize--;
        }
        (*bIndex)++;
        sift(inputBuffer, 0, hSize);
        if(*remainingElements==0 && (*bSize)+750 == *bIndex){
            pIndex = hSize;
            break;
        }
        if(*bIndex == 1000){
            *bIndex = 750;
            if(*remainingElements >= 250){
                fread(inputBuffer+750, sizeof( int ), 250, fp);
                *remainingElements -= 250;
            } else{
                fread(inputBuffer+750, sizeof( int ), *remainingElements, fp);
                *bSize = *remainingElements;
                *remainingElements = 0;
            }
        }
        if(outIndex == 1000){
            fwrite(outBuffer, sizeof(int), 1000, runFile);
            outIndex = 0;
        }
    }
    if(*remainingElements == 0 && (*bSize)+750 == *bIndex){
        while(hSize != 0) {
            outBuffer[outIndex++] = inputBuffer[0];
            inputBuffer[0] = inputBuffer[hSize - 1];
            sift(inputBuffer, 0, hSize);
            hSize--;
            if(outIndex == 1000){
                fwrite(outBuffer, sizeof(int), 1000, runFile);
                outIndex = 0;
            }
        }
    }
    fwrite(outBuffer, sizeof(int), outIndex, runFile);
    return pIndex;
}

void multiStepMerge(FILE *out, const char *inputFileName, int *outBuffer, int *inBuffer, int runs) {
    int i;
    int superRuns = (int) ceil((double)runs / 15);
    char *superRunPrefix = getSuperRunPrefix(inputFileName);
    FILE *superRunFile;
    for (i = 0; i < superRuns; ++i) {
        int startRun = i*15;
        int endRun = (i != (superRuns-1)) ? ((i+1)*15)-1 : (runs-1);
        char *runFileName = getRunFileName(superRunPrefix, i);
        superRunFile = fopen(runFileName, "w+b");
        mergeRuns(startRun, endRun, superRunFile, inputFileName, outBuffer, inBuffer, endRun-startRun+1);
        fclose(superRunFile);
    }
    mergeRuns(0, superRuns - 1, out, getSuperRunPrefix(inputFileName), outBuffer, inBuffer, runs);
}

void mergeRuns(int startRun, int endRun, FILE *outFile, const char *runFilePrefix, int *outBuffer, int *inBuffer, int mergeRuns) {
    int i,j,l;
    FILE *runFiles[endRun - startRun + 1];
    int numberOfKeys = (int) floor(1000 / (endRun - startRun + 1));
    int runFileSize[endRun - startRun + 1];
    int runFileIndex[endRun - startRun + 1];
    for (l = startRun; l <= endRun; ++l) {
        char *runFileName = getRunFileName(runFilePrefix, l);
        runFiles[l - startRun] = fopen(runFileName, "rb");
        runFileSize[l - startRun] = getNumberOfElements(runFiles[l - startRun]);
    }
    int inputIndex = 0;
    for (j = startRun; j <= endRun; j++) {
        if(runFileSize[j - startRun] >= numberOfKeys){
            fread(inBuffer+inputIndex, sizeof(int), numberOfKeys, runFiles[j - startRun]);
            runFileIndex[j - startRun] = inputIndex;
            inputIndex += numberOfKeys;
            runFileSize[j - startRun] -= numberOfKeys;
        } else{
            fread(inBuffer+inputIndex, sizeof(int), runFileSize[j - startRun], runFiles[j - startRun]);
            runFileIndex[j - startRun] = inputIndex;
            inputIndex += runFileSize[j - startRun];
            runFileSize[j - startRun] = 0;
        }
    }
    while (inputIndex < 1000) {
        inBuffer[inputIndex++] = INT32_MAX;
    }
    int foundRun = 0;
    int foundInputIndex = 0;
    for (i = 0; i < mergeRuns; i++) {
        int outIndex = 0;
        int min;
        while (outIndex < 1000) {
            min = INT32_MAX;
            for (j = startRun; j <= endRun; ++j) {
                if (min > inBuffer[runFileIndex[j - startRun]]) {
                    min = inBuffer[runFileIndex[j - startRun]];
                    foundRun = runFileIndex[j - startRun] / numberOfKeys;
                    foundInputIndex = runFileIndex[j - startRun];
                }
            }
            if(min == INT32_MAX){
                break;
            }
            outBuffer[outIndex++] = min;
            runFileIndex[foundRun]++;
            inBuffer[foundInputIndex] = INT32_MAX;
            if((foundInputIndex+1) % numberOfKeys == 0){
                if(runFileSize[foundRun] >= numberOfKeys){
                    fread(inBuffer+foundInputIndex+1-numberOfKeys, sizeof(int), numberOfKeys, runFiles[foundRun]);
                    runFileIndex[foundRun] = foundInputIndex+1-numberOfKeys;
                    runFileSize[foundRun] -= numberOfKeys;
                } else if(runFileSize[foundRun] != 0){
                    fread(inBuffer+foundInputIndex+1-runFileSize[foundRun], sizeof(int), runFileSize[foundRun], runFiles[foundRun]);
                    runFileIndex[foundRun] = foundInputIndex+1-runFileSize[foundRun];
                    runFileSize[foundRun] = 0;
                } else {
                    runFileIndex[foundRun] = foundRun*numberOfKeys;
                }
            }
        }
        fwrite(outBuffer, sizeof(int), outIndex, outFile);
    }
    for (l = startRun; l <= endRun; l++) {
        fclose(runFiles[l - startRun]);
    }
}

char *getRunFileName(const char *inputFileName, int run) {
    char *fileName = malloc(strlen(inputFileName)+4);
    sprintf(fileName, "%s.%03d", inputFileName, run);
    return fileName;
}

char *getSuperRunPrefix(const char *inputFileName) {
    char *fileName = malloc(strlen(inputFileName)+6);
    sprintf(fileName, "%s.%s", inputFileName, "super");
    return fileName;
}

int getNumberOfElements(FILE *file)
{
    fseek( file, 0, SEEK_END );
    int length = ftell(file) / sizeof(int);
    fseek( file, 0, SEEK_SET );
    return length;
}

int comparator(const void *p, const void *q)
{
    return *(const int *)p - *(const int *)q;
}
