
#ifndef INSTRUCTION_MEMORY_H
#define INSTRUCTION_MEMORY_H

#include "typedef.h"
#include "boolean.h"




/**
 * @brief Represents a single instruction word in memory.
 *
 * Each node stores the encoded machine word, its assigned address,
 * and a pointer to the next instruction node.
 *
 * Used to build a linked list representing the instruction memory.
 */
typedef struct inst_mem {
    int value;                  /**< Encoded machine word value. */
    unsigned int address;       /**< Instruction address. */
    instruction_ptr next;       /**< Pointer to the next instruction node. */
} inst_mem;

/**
 * @brief Add a new instruction word to the instruction memory list.
 *
 * Allocates a new node containing the encoded value, sets its address,
 * and appends it to the end of the list. Updates the IC and memory usage counters.
 *
 * @param encoded_val       Encoded instruction word to insert.
 * @param instruction_memory Pointer to the head of the instruction memory list.
 * @param IC                Pointer to the instruction counter (updated on insert).
 * @param memory_usage      Pointer to memory usage counter (updated on insert).
 * @return true on success, false if memory is full or allocation fails.
 */
boolean add_instruction_to_memory(unsigned short encoded_val,instruction_ptr *instruction_memory,unsigned int* IC,unsigned int *memory_usage);

/**
 * @brief Print the  instruction memory.
 *
 * Iterates over the instruction memory list and prints each word
 * in binary format along with its address.
 *
 * @param instruction_memory Head of the instruction memory list.
 */
void print_instruction_memory(instruction_ptr instruction_memory);

/**
 * @brief Free the entire instruction memory list.
 *
 * Releases all  allocated instruction nodes
 * and resets the list head to NULL.
 *
 * @param instruction_memory Pointer to the head of the instruction memory list.
 */
void free_instruction_memory(instruction_ptr *instruction_memory);


#endif
