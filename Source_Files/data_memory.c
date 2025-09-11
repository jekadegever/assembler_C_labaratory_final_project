
#include "data_memory.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "util.h"
#include "sys_memory.h"


/**
 * @file data_memory.c
 * @brief Data memory management for the assembler.
 *
 * This module implements the handling of the assembler's **data memory image**,
 * which stores numeric values, strings, and matrix declarations encountered
 * during the assembly process (from `.data`, `.string`, `.mat` directives).
 *
 * Responsibilities:
 *  - Dynamically allocate and store data values in a linked list structure.
 *  - Track memory usage and update the Data Counter (DC).
 *  - Provide functions for printing the final data memory image
 *    (address → value).
 *  - Safely release all allocated data memory resources at cleanup.
 *
 * Data is stored sequentially in a linked list, with each node containing:
 *  - `value`  – the numeric data value.
 *  - `address` – the assigned memory address (relative to DC).
 *  - `next`    – pointer to the next node.
 *
 * Error handling:
 *  - Ensures no memory overflows (`MEMORY_AVAILABLE_SPACE`).
 *  - Reports system/internal errors when allocation fails.
 *
 * This module is used by the **first pass** when directives are parsed
 * and by the **memory relocation stage** when final addresses are calculated.
 *
 * @see first_pass.c
 * @see memory_relocation.c
 *
 * @author Ivgeny Tokarzhevsky
 * @date 01/09/2025
 */



boolean add_data_to_memory(int val, data_ptr *data_memory, unsigned int *DC, unsigned int *memory_usage) {

    data_ptr temp =NULL;
    data_ptr new_data_node = NULL;

    /*verify that all necessary input params exist*/
    if (!data_memory || !memory_usage || !DC) {
        print_internal_error(ERROR_CODE_25,"add_data_to_memory");
        return false;
    }

    /*check available memory capacity for new node*/
    if (*memory_usage >= MEMORY_AVAILABLE_SPACE) {
        return false;
    }

    /*allocate memory for new data node*/
    new_data_node = (data_ptr)handle_malloc(sizeof(data_mem));


    /*set new node values*/
    new_data_node->value = val;
    new_data_node->address = *DC;
    new_data_node->next = NULL;


    /*if the head of the list is NULL, assign the new node to the head*/
    if(!*data_memory) {
        *data_memory = new_data_node;
        (*DC)++;
        (*memory_usage)++;
        return true;
    }

    else {/*search for the last node*/
        temp = *data_memory;
        while(temp->next != NULL) {
            temp = temp->next;
        }

        /*insert the new node in the end of the list*/
        temp->next = new_data_node;
        (*DC)++;
        (*memory_usage) ++;
        return true;
    }

}


void print_data_memory(data_ptr data_memory) {


    /*run over the list and print each cell values */
    while (data_memory != NULL) {
        printf("binary: ");
        print_binary(data_memory->value, 10, STDOUT, NULL);
        printf("\tdecimal: %d",data_memory->value);
        printf("\tin address: %d\n", data_memory->address);

        data_memory = data_memory->next;
    }
}


void free_data_memory(data_ptr *data_memory) {

    data_ptr temp = NULL;

    while (*data_memory != NULL) {
        temp=*data_memory;/*hold the head address*/
        *data_memory = (*data_memory)->next;/*assign the next node as head*/
        safe_free((void**)&temp);/*free the first node (previously the head)*/
    }
}

