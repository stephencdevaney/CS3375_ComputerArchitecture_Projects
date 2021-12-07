/*
 * CS3375 Computer Architecture
 * Course Project
 * Cache Simulator Design and Development
 * FALL 2017
 * By Yong Chen
 */

int PLRU;
int BLOCK_SIZE;               /* Cache block size (or cache line size) in bytes \\
                                 (must be power of 2). 4 Bytes = 1 Word */
int WAY_SIZE;                 /* Associativity; 1-way = direct-mapped */
int CACHE_SIZE;               /* Cache capacity in bytes (must be power of 2)*/
int NUM_BLOCKS;
int NUM_SETS;

//#define DBG

FILE *DBGFP;
FILE *fp;
uint64_t address;

/*The data structure of direct-mapped cache*/
struct direct_mapped_cache {
    unsigned *valid_field;   /* Valid field */
    unsigned *dirty_field;   /* Dirty field; since we don't distinguish writes and \\
                                           reads in this project yet, this field doesn't really matter */
    uint64_t *tag_field;     /* Tag field */
    char **data_field;  /* Data field; since we don't really fetch data, this field doesn't really matter size of [NUM_BLOCKS][BLOCK_SIZE] */
    int hits;                          /* Hit count */
    int misses;                        /* Miss count */
};

/*Read the memory traces and convert it to binary*/
uint64_t convert_address(char memory[]);


void runCache();
void direct_mapped_cache_access(struct direct_mapped_cache *cache, uint64_t address); /*Simulate the direct-mapped cache*/
void n_set_associative_cache_access(struct direct_mapped_cache *cache, uint64_t address); /*Simulate the n-way associative cache*/
void fully_associative_cache_access(struct direct_mapped_cache *cache, uint64_t address); /*Simulate the fully associative cache*/
