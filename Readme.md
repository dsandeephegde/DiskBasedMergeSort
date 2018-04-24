## Disk-Based Merge Sort

#### Goal 
To compare the performance of different algorithms for creating and merging runs during merge sort.

#### Merge Sort Methods
- Basic MergeSort
- Multistep MergeSort
- Replacement Selection MergeSort

##### Basic MergeSort
- Open input.bin and read its contents in 1000-key blocks using the input buffer.
- Sort each block and write it to disk as a run file. Name each run file index-file.n, where n is a 3-digit run identifier, starting at 0. For example, if the input index file is input.bin, the run files would be named

    input.bin.000
    input.bin.001
     ...

- Open each run file and buffer part of its contents into the input buffer. The amount of each run you can buffer will depend on how many runs you are merging (e.g., merging 50 runs using the 1000-key input buffer allows you to buffer 20 keys per run).
- Merge the runs to produce sorted output. Use the output buffer to write the results in 1000-key chunks as binary data to sort.bin.
- Whenever a run's buffer is exhausted, read another block from the run file. Continue until all run files are exhausted.

##### Multistep MergeSort
- Create the initial runs for input.bin, exactly like the basic mergesort.
- Merge a set of 15 runs to produce a super-run. Open the first 15 run files and buffer them using your input buffer. Merge the 15 runs to produce sorted output, using your output buffer to write the results as binary data to a super-run file.
- Continue merging sets of 15 runs until all of the runs have been processed. Name each super-run file index-file.super.n, where n is a 3-digit super-run identifier, starting at 0. For example, if the input file is input.bin, the super-run files would be named

    input.bin.super.000
    input.bin.super.001
     ...

- Merge all of the super-runs to produce sorted output. Use the input buffer to read part of the contents of each super-run. Use the output buffer to write the results in 1000-key chunks as binary data to sort.bin.

##### Replacement Selection MergeSort
- Divide your input buffer into two parts: 750 entries are reserved for a heap H1 ... H750, and 250 entries are reserved as an input buffer B1 ... B250 to read keys from input.bin.
- Read the first 750 keys from input.bin into H, and the next 250 keys into B. Rearrange H so it forms an ascending heap.
- Append H1 (the smallest value in the heap) to the current run, managed through the output buffer. Use replacement selection to determine where to place B1.
    
    - If H1 ≤ B1, replace H1 with B1.
    - If H1 > B1, replace H1 with H750, reducing the size of the heap by one. Replace H750 with B1, increasing the size of the secondary heap by one.
        
        Adjust H1 to reform H into a heap.
- Continue replacement selection until H is empty, at which point the current run is completed. The secondary heap will be full, so it replaces H, and a new run is started.
- Run creation continues until all values in input.bin have been processed. Name the runs exactly as you did for the basic mergesort (i.e., input.bin.000, input.bin.001, and so on).
- Merge the runs to produce sorted output, exactly like the merge step in the basic mergesort.

#### Available Memory Constraint
The program assumes that the input buffer and output buffers are sized to hold a maximum of 1000 integer keys.

#### Program Execution

`
cc main.c
`

`
./a.out mergeSortMethod indexFile sortedIndexFile
`

Example: 

`./a.out --multistep input.bin sort.bin`

#### Test cases

Test cases can be run by following command

`sh test.sh sizeOfInput`

Example: 

`sh test.sh 30000`
