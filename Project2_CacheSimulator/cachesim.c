/*
 * CS3375 Computer Architecture
 * Course Project
 * Cache Simulator Design and Development
 * FALL 2017
 * By Yong Chen
 */

/* included header files for the project */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "cachesim.h"
#include <time.h>


int main(int argc, char *argv[])
{
    srand((unsigned int)time(NULL));  // set seed for random number generator
    PLRU = 0;  // preset for PLRU
    BLOCK_SIZE = 64;  // preset for block size
    CACHE_SIZE = 32768;  // preset for cache size
    argc = 5;
    argv[1] = "8-way";
    argv[2] = "trace.stream_10";
    argv[3] = "128";
    argv[4] = "32768";
    if (argc < 2 || argc > 6) {  // number of arguements required to simulate the cache
        printf("Usage 1: %s <direct, n-way (replace n with a number), or fully> <trace file name>\n", argv[0]);
        printf("Usage 2: %s <direct, n-way (replace n with a number), or fully> <trace file name> <PLRU (1 or 0)>\n", argv[0]);
        printf("Usage 3: %s <direct, n-way (replace n with a number), or fully> <trace file name> <Block Size> <Cache Size>\n", argv[0]);
        printf("Usage 4: %s <direct, n-way (replace n with a number), or fully> <trace file name> <Block Size> <Cache Size> <PLRU (1 or 0)>\n", argv[0]);
        return 1;
    }
    else if(argc == 4 && (atoi(argv[3]) == 1 || atoi(argv[3]) == 0)){  // usage # 2 allows the change of PRLU
        PLRU = atoi(argv[3]);
    }
    else if(argc == 5 && atoi(argv[4]) > atoi(argv[3])){  // usage # 3 allows the change of block size and cache size
        BLOCK_SIZE = atoi(argv[3]);
        CACHE_SIZE = atoi(argv[4]);
    }
    else if(argc == 6 && (atoi(argv[5]) == 1 || atoi(argv[5]) == 0) && argv[4] > argv[3]){  // usage # 4 allow the change of PLRU, block size, and cache size
        BLOCK_SIZE = atoi(argv[3]);
        CACHE_SIZE = atoi(argv[4]);
        PLRU = atoi(argv[5]);
    }
    NUM_BLOCKS = (CACHE_SIZE / BLOCK_SIZE);  // calculate the number of block for the cache
    /* Opening the memory trace file */
    char* trace_file_name = argv[2];  // setup to open the file
    fp = fopen(trace_file_name, "r");  // open the test file
    if(fp ==  NULL){  // if the file cannot be open exit let use know and exit the program
        printf("File could not be opened!\n");
        return 1;
    }
    if (strncmp(argv[1], "direct", 6) == 0){  // setup for direct-mapped cache
        WAY_SIZE = 1;  // way size = 1 for direct
        NUM_SETS = NUM_BLOCKS;  // number of sets = number of blocks for direct
    }
    else if (strncmp(argv[1], "fully", 5) == 0){  // setup for fully associative cache
        WAY_SIZE = NUM_BLOCKS;  // way size = number of blocks for fully associative cache 
        NUM_SETS = 1;  // number of sets =  1 for fully associative cache
    }
    else{
        if(strstr(argv[1], "-way") != NULL){  // setup for n-set associative cache
            char *argvCopy;
            argvCopy = malloc(sizeof(char) * (strlen(argv[1])+1));  // allocated memory to make a copy of argv[1] so that it may be modified
            strcpy(argvCopy, argv[1]);  // copy argv[1] so that we can modify the n-way string
            WAY_SIZE = atoi(strtok(argvCopy, "-"));  // get the way size from the string
            NUM_SETS = (NUM_BLOCKS / WAY_SIZE);  // calculate the number of sets
        }
        else{
            printf("Invalid Arguements\n");
            return 1;
        }
    }
    if(PLRU == 1){
        printf("PLRU is not yet implemented resting PLRU value to 0 for random replacement");
        PLRU = 0;
    }
    runCache();  // simulate the correct cache base upon the correct way size
    fclose(fp);  // close the file
    return 0;
}


int replacementPolicy()
{
    int replace;
    if(!PLRU){  // random replacement policy
        replace = rand() % WAY_SIZE; // calculates the random index in the set to be replaced
    }
    return replace;
}


void runCache()
{
    struct direct_mapped_cache d_cache;
    char mem_request[20];
    
    DBGFP = fopen("output.txt", "w");  //open output file
    
    /* Initialization */
    d_cache.valid_field = (unsigned*)malloc(NUM_BLOCKS * sizeof(unsigned));
    d_cache.dirty_field = (unsigned*)malloc(NUM_BLOCKS * sizeof(unsigned));
    d_cache.tag_field = (uint64_t*)malloc(NUM_BLOCKS * sizeof(uint64_t));
    for (int i=0; i<NUM_BLOCKS; i++){
        d_cache.valid_field[i] = 0;
        d_cache.dirty_field[i] = 0;
        d_cache.tag_field[i] = 0;
    }
    d_cache.hits = 0;
    d_cache.misses = 0;
    
    /* Read the memory request address and access the cache */
    while (fgets(mem_request, 20, fp)!= NULL) {
        address = convert_address(mem_request);
        if(WAY_SIZE == 1) direct_mapped_cache_access(&d_cache, address); /* Simulating direct-mapped cache */
        else if(WAY_SIZE == NUM_BLOCKS) fully_associative_cache_access(&d_cache, address);  /* Simlating Fully Associative, Cache */
        else n_set_associative_cache_access(&d_cache, address);/* Simlating N-Way Associative, Cache */
    }
    
    /*Calculate hit and miss rates */
    double hitRate = (double)d_cache.hits / (double)(d_cache.hits + d_cache.misses);  // hit rate is hits/(hits + misses)
    double missRate = 1 - hitRate;  // miss rate is 1 - hit rate before turning the hit rate into a %
    
    /*Print out the results to the console*/
    #ifdef DBG
        printf("\n");
        fprintf(DBGFP, "\n");
    #endif
    printf("BLOCK SIZE = %d Bytes\n", BLOCK_SIZE);
    printf("%d-WAY\n", WAY_SIZE);
    printf("CACHE SIZE = %d Bytes\n", CACHE_SIZE);
    printf("NUMBER OF BLOCKS = %d\n", NUM_BLOCKS);
    printf("NUMBER OF SETS = %d\n\n", NUM_SETS);
    printf("\n=======================================\n");
    if(WAY_SIZE == NUM_BLOCKS) printf("Cache type:    Fully associative Cache\n");
    else if(WAY_SIZE == 1) printf("Cache type:    Direct-Mapped Cache\n");
    else printf("Cache type:    %d-Way associative Cache\n", WAY_SIZE);
    printf("=======================================\n");
    printf("Cache Hits:      %d\n", d_cache.hits);
    printf("Cache Misses:    %d\n", d_cache.misses);
    printf("Cache Hit Rate:  %d%%\n", (int)(hitRate * 100));
    printf("Cache Miss Rate: %d%%\n", (int)(missRate * 100));

    /*Print out the results to the output file*/
    fprintf(DBGFP, "BLOCK SIZE = %d Bytes\n", BLOCK_SIZE);
    fprintf(DBGFP, "%d-WAY\n", WAY_SIZE);
    fprintf(DBGFP, "CACHE SIZE = %d Bytes\n", CACHE_SIZE);
    fprintf(DBGFP, "NUMBER OF BLOCKS = %d\n", NUM_BLOCKS);
    fprintf(DBGFP, "NUMBER OF SETS = %d\n\n", NUM_SETS);
    fprintf(DBGFP, "\n=======================================\n");
    if(WAY_SIZE == NUM_BLOCKS) fprintf(DBGFP, "Cache type:    Fully associative Cache\n");
    if(WAY_SIZE == 1) fprintf(DBGFP, "Cache type:    Direct-Mapped Cache\n");
    else fprintf(DBGFP, "Cache type:    %d-Way associative Cache\n", WAY_SIZE);
    fprintf(DBGFP, "=======================================\n");
    fprintf(DBGFP, "Cache Hits:      %d\n", d_cache.hits);
    fprintf(DBGFP, "Cache Misses:    %d\n", d_cache.misses);
    fprintf(DBGFP, "Cache Hit Rate:  %d%%\n", (int)(hitRate * 100));  // outputs the hit rate after turning the hit rate into a percentage
    fprintf(DBGFP, "Cache Miss Rate: %d%%\n", (int)(missRate * 100));  // outputs the miss rate after turning the hit rate into a percentage
    
    fclose(DBGFP);  // close output file
}


void fully_associative_cache_access(struct direct_mapped_cache *cache, uint64_t address)
{
    uint64_t block_addr = address >> (unsigned)log2(BLOCK_SIZE);  // get the block address by shifting for the offset
    // no index since the cache is fully associative
    uint64_t tag = block_addr; // the tag = the block address in a fully associative cache
    int hitFlag = 0;  // flag to check for a hit
    uint64_t way = 0;  // index to loop through the set and find hit and miss and provide index for replacement
    for(; way < NUM_BLOCKS; way++){  // loop through the set
        if(cache->valid_field[way] && cache->tag_field[way] == tag){  // checks for a hit
            hitFlag = 1;
            break;
        }
        else if(cache->valid_field[way] == 0){  // check for the first empty slot in the cache for a miss since we are filling the set from low index to high index
            break;
        }
    }
    
    /* prints debug statements to console and output file */
    #ifdef DBG
        printf("Memory address: %llu, Block address: %llu, Tag: %llu ", address, block_addr, tag);
        fprintf(DBGFP, "Memory address: %llu, Block address: %llu, Tag: %llu ", address, block_addr, tag);
    #endif

    if (hitFlag) { 
        /* Cache hit */
        cache->hits += 1;
        #ifdef DBG
                printf("Way %llu Hit!\n", way);
                fprintf(DBGFP, "Way %llu Hit!\n", way);
        #endif

    }
    else{
        /* Cache miss */
        cache->misses +=1;
        if(way == NUM_BLOCKS){  // if the set is full get replacement index
            way = replacementPolicy();  // gets the index to be replace based upone PLRU value
            /* prints debug statements to console and output file */
            #ifdef DBG
                    printf("Way %llu Miss! Replacement!\n", way);
                    fprintf(DBGFP, "Way %llu Miss! Replacement!\n", way);
            #endif
        }
        else{  // if the set is not full place at the first empty index
            /* prints debug statements to console and output file */
            #ifdef DBG
                    printf("Way %llu Miss!\n", way);
                    fprintf(DBGFP, "Way %llu Miss!\n", way);
            #endif
        }
        
        if (cache->valid_field[way] && cache->dirty_field[way]) {
            /* Write the cache block back to memory */
        }
        /* place values in the cache */
        cache->tag_field[way] = tag;
        cache->valid_field[way] = 1;
        cache->dirty_field[way] = 0;
    }
}


void n_set_associative_cache_access(struct direct_mapped_cache *cache, uint64_t address)
{
    uint64_t block_addr = address >> (unsigned)log2(BLOCK_SIZE);  // get the block address by shifting for the offset
    uint64_t index = block_addr % NUM_SETS;  // calculates the index based upon the block address and the number of set in a n-way cache
    uint64_t tag = block_addr >> (unsigned)log2(NUM_SETS);  // calculates the tag base upon the block address and the number of sets in a n-way cache
    int hitFlag = 0;
    uint64_t way = 0;
    for(; way < WAY_SIZE; way++){  // loop through the set
        if(cache->valid_field[index * WAY_SIZE + way] && cache->tag_field[index * WAY_SIZE + way] == tag){   // checks for a hit
            hitFlag = 1;
            break;
        }
        else if(cache->valid_field[index * WAY_SIZE + way] == 0){  // check for the first empty slot in the cache for a miss since we are filling the set from low index to high index
            break;
        }
    }
    /* prints debug statements to console and output file */
    #ifdef DBG
        printf("Memory address: %llu, Block address: %llu, Index: %llu, Tag: %llu ", address, block_addr, index, tag);
        fprintf(DBGFP, "Memory address: %llu, Block address: %llu, Index: %llu, Tag: %llu ", address, block_addr, index, tag);
    #endif

    if (hitFlag) { 
        /* Cache hit */
        cache->hits += 1;
        /* prints debug statements to console and output file */
        #ifdef DBG
                printf("Way %llu Hit!\n", way / WAY_SIZE);
                fprintf(DBGFP, "Way %llu Hit!\n", way / WAY_SIZE);
        #endif

    }
    else{
        /* Cache miss */
        cache->misses +=1;
        if(way == WAY_SIZE){  // if the set is full get replacement index
            way = replacementPolicy();  // gets the index to be replace based upone PLRU value
            /* prints debug statements to console and output file */
            #ifdef DBG
                    printf("Way %llu Miss! Replacement!\n", way);
                    fprintf(DBGFP, "Way %llu Miss! Replacement!\n", way);
            #endif
        }
        else{  // if the set is not full place at the first empty index
            /* prints debug statements to console and output file */
            #ifdef DBG
                    printf("Way %llu Miss!\n", way);
                    fprintf(DBGFP, "Way %llu Miss!\n", way);
            #endif
        }
        if (cache->valid_field[index * WAY_SIZE + way] && cache->dirty_field[index * WAY_SIZE + way]) {
            /* Write the cache block back to memory */
        }
        /* place values in the cache*/
        cache->tag_field[index * WAY_SIZE + way] = tag;
        cache->valid_field[index * WAY_SIZE + way] = 1;
        cache->dirty_field[index * WAY_SIZE + way] = 0;
    }
}


void direct_mapped_cache_access(struct direct_mapped_cache *cache, uint64_t address)
{
    uint64_t block_addr = address >> (unsigned)log2(BLOCK_SIZE);  // shifts the offset to get the block addres for a direct mapped cache
    uint64_t index = block_addr % NUM_BLOCKS;  // gets the index from the block address for a direct mapped cache
    uint64_t tag = block_addr >> (unsigned)log2(NUM_BLOCKS);  // get the tag based upone the block address and the number of block in the cache
    /* prints debug statements to console and output file */
    #ifdef DBG
        printf("Memory address: %llu, Block address: %llu, Index: %llu, Tag: %llu ", address, block_addr, index, tag);
        fprintf(DBGFP, "Memory address: %llu, Block address: %llu, Index: %llu, Tag: %llu ", address, block_addr, index, tag);
    #endif

    if (cache->valid_field[index] && cache->tag_field[index] == tag) { /* Cache hit */
        cache->hits += 1;
        /* prints debug statements to console and output file */
        #ifdef DBG
                printf("Hit!\n");
                fprintf(DBGFP, "Hit!\n");
        #endif

    } else {
        /* Cache miss */
        cache->misses += 1;
        /* no replacement algorithm need since direct mapped is has a set associative of 1 */
        /* prints debug statements to console and output file */
        #ifdef DBG
                printf("Miss!\n");
                fprintf(DBGFP, "Miss!\n");
        #endif
        if (cache->valid_field[index] && cache->dirty_field[index]) {
            /* Write the cache block back to memory */
        } 
        /* place values in the cache */
        cache->tag_field[index] = tag;
        cache->valid_field[index] = 1;
        cache->dirty_field[index] = 0;
    }
}


uint64_t convert_address(char memory_addr[])
/* Converts the physical 32-bit address in the trace file to the "binary" \\
 * (a uint64 that can have bitwise operations on it) */
{
    uint64_t binary = 0;
    int i = 0;
    
    while (memory_addr[i] != '\n') {
        if (memory_addr[i] <= '9' && memory_addr[i] >= '0') {
            binary = (binary*16) + (memory_addr[i] - '0');
        } else {
            if(memory_addr[i] == 'a' || memory_addr[i] == 'A') {
                binary = (binary*16) + 10;
            }
            if(memory_addr[i] == 'b' || memory_addr[i] == 'B') {
                binary = (binary*16) + 11;
            }
            if(memory_addr[i] == 'c' || memory_addr[i] == 'C') {
                binary = (binary*16) + 12;
            }
            if(memory_addr[i] == 'd' || memory_addr[i] == 'D') {
                binary = (binary*16) + 13;
            }
            if(memory_addr[i] == 'e' || memory_addr[i] == 'E') {
                binary = (binary*16) + 14;
            }
            if(memory_addr[i] == 'f' || memory_addr[i] == 'F') {
                binary = (binary*16) + 15;
            }
        }
        i++;
    }
    
    #ifdef DBG
        printf("%s converted to %llu\n", memory_addr, binary);
        fprintf(DBGFP, "%s converted to %llu\n", memory_addr, binary);
    #endif
    
    return binary;
}
