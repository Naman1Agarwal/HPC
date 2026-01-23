#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>

typedef uint8_t value_t;
typedef uint32_t index_t;
typedef int32_t  init_t;

typedef struct matrix{
    index_t rows;
    index_t cols;
    value_t** arr;
}matrix;

typedef struct simulation{
    init_t generations;
    init_t freq;
    init_t seed;
    matrix* old;
    matrix* new;
}simulation;


/**
* @brief    prints 2d grid of numbers
* 
* @param[in] m        contains 2D array to print
* @param[in] offset   starts at row offset and column offset
*
*/
void printArray(matrix* m, index_t offset);


/**
* @brief    reads generations and freq from cmd lin
* 
* @param[out] sim     struct obj that will contain freq and generations
* @param[in] offset   starts at argv[offset]
* @param[in] argv     string array containing args
* 
* @return    int      0 if successful
*/
int parseArgs(simulation* sim, index_t offset, char* argv[]);


/**
* @brief      mallocs memory for matrix object and 2d grid
* 
* @param[in]  rows     num of rows
* @param[in]  cols     num of cols
* 
* @return     matrix*  pointer to object
*/
matrix* initMat(index_t rows, index_t cols);


/**
* @brief      reads data from file and inits grid with lives for game of life
* 
* file should have lines with two numbers each
* the function updates grid as follows:
* grid[num1][num2] = 1
* 
* @param[in]  fd       file descriptor for file
* 
* @return     matrix*  pointer to initialized grid
*/
matrix* readFile(FILE* fd);


/**
* @brief      uses rand() to generate random lives in 2d grid
* 
* @param[out] mat*     2d grid
* @param[in]  offset   wont generate lives before offset
*
*/
void genRandVals(matrix* mat, index_t offset);


/**
* @brief      returns if grid[i][j] becomes alive or dead based on gol rules
* 
* @param[in]  m     2d grid
* @param[in]  i     row index
* @param[in]  j     col index
* 
* @return     value_t  1 if alive, 0 if dead
*/
value_t nodeUpdate(value_t** m, index_t i, index_t j);


/**
* @brief      based on the wrapping principle, the function updates the 
*             edges of m
* 
* @param[out]  m        2d grid
* @param[in]   offset   offset is where the actual lives are in the grid
* 
*/
void updateEdges(matrix* m, index_t offset);


/**
* @brief      calls node update on all nodes in old
* 
* @param[in]  old   we update lives based off this
* @param[out] new   stores the next gen of lives 
* 
*/
void update(matrix* old, matrix* new);


/**
* @brief      calls update for generation times
* 
* @param[in, out]  sim*   we update the lives and print
*                         the new grid based on freq
* 
*/
void iterate(simulation* sim);