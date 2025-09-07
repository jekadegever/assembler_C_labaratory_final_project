
#ifndef DATA_MEMORY_H
#define DATA_MEMORY_H
#include "boolean.h"
#include "typedef.h"





/**
 * @struct data_mem
 * @brief Represents a node in the assembler's data memory list.
 *
 * Each node stores a single data value and its corresponding memory address.
 * The nodes are linked together to form a list that represents the entire
 * data memory image of the assembler.
 *
 * @var data_mem::value
 *      The numeric value stored in this memory cell.
 *
 * @var data_mem::address
 *      The absolute memory address associated with this value.
 *
 * @var data_mem::next
 *      Pointer to the next node in the data memory list (or NULL if this is the last node).
 */
typedef struct data_mem {
    int value;                /**< The numeric value stored in this memory cell. */
    unsigned int address;     /**< The absolute memory address of this value. */
    data_ptr next;            /**< Pointer to the next node in the list, or NULL. */
} data_mem;




/**
 * @brief Adds a new value to the assembler's data memory list.
 *
 * Allocates a new node, assigns it the given value and current data counter (DC),
 * then appends it to the end of the linked list representing the data memory.
 * Updates the data counter and memory usage accordingly.
 *
 * @param val           The value to store in the new data memory node.
 * @param data_memory   Pointer to the head of the data memory linked list.
 * @param DC            Pointer to the data counter (incremented after insertion).
 * @param memory_usage  Pointer to the memory usage counter (incremented after insertion).
 *
 * @return true  if the value was successfully added.
 * @return false if input pointers are NULL, memory space exceeded,
 *               or memory allocation failed.
 */
boolean add_data_to_memory(int val, data_ptr *data_memory, unsigned int *DC, unsigned int *memory_usage);


/**
 * @brief Prints the contents of the data memory list.
 *
 * Iterates through the linked list of data memory nodes and prints
 * each stored value along with its corresponding memory address.
 *
 * @param data_memory  Pointer to the head of the data memory linked list.
 */
void print_data_memory(data_ptr data_memory);


/**
 * @brief Frees all nodes in the data memory list.
 *
 * Iterates through the linked list of data memory nodes, freeing
 * each node and setting the head pointer to NULL at the end.
 *
 * @param data_memory  Pointer to the head of the data memory linked list.
 *                     After the call, *data_memory will be NULL.
 */
void free_data_memory(data_ptr *data_memory);


#endif
