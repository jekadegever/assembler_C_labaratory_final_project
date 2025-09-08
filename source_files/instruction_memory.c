

#include "instruction_memory.h"
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "util.h"
#include "errors.h"
#include "sys_memory.h"



/**
 * @file Instruction_memory.c
 * @brief Implements the instruction memory management for the assembler.
 *
 * This module provides functionality to manage the instruction memory,
 * which is stored as a linked list of encoded machine words.
 *
 * Features include:
 *  - Adding new instructions into memory while updating IC and memory usage.
 *  - Printing the instruction memory contents in binary format with addresses.
 *  - Freeing all allocated instruction memory at the end of processing.
 *
 * The instruction memory represents the program’s machine code section
 * and is later written into the object (.obj) output file.
 *
 * @author Ivgeny tokarzhevsky
 * @date 01/09/2025
 */




boolean add_instruction_to_memory(unsigned short encoded_val, instruction_ptr *instruction_memory, unsigned int* IC, unsigned int *memory_usage) {
    instruction_ptr temp;
    instruction_ptr new_inst_node;

    /*validate that all input params exist*/
    if (!instruction_memory || !IC || !memory_usage) {
        print_internal_error(ERROR_CODE_25,"add_instruction_to_memory");
        return false;
    }

    /*verify that the memory is not full*/
    if (*memory_usage == MEMORY_AVAILABLE_SPACE) {
        return false;
    }

    /*allocate memory for new node*/
    new_inst_node = (instruction_ptr)handle_malloc(sizeof(inst_mem));

    /*insert values into the new node*/
    new_inst_node->value = encoded_val;
    new_inst_node->address = *IC;
    new_inst_node->next = NULL;


    /*insert the node at head if head is empty*/
    if(*instruction_memory == NULL) {
        *instruction_memory = new_inst_node;
        (*IC)++;
        (*memory_usage)++;
        return true;
    }

    else {/*otherwise append in the end of the list*/
        temp = *instruction_memory;
        while(temp->next != NULL) {
            temp = temp->next;
        }

        temp->next = new_inst_node;
        (*IC)++;/*increment the memory usage*/
        (*memory_usage)++;/*increment the instructions counter*/
        return true;
    }

}


void print_instruction_memory(instruction_ptr instruction_memory) {
    instruction_ptr temp;
    temp=instruction_memory;

    /*print the instruction memory in binary format*/
    while (temp != NULL) {
        printf("binary: ");
        print_binary(temp->value,10);
        printf("\tdecimal: %d \tin address -> %d\n",temp->value, temp->address);
        temp = temp->next;
    }
}




void free_instruction_memory(instruction_ptr *instruction_memory) {
    instruction_ptr temp = NULL;

    /*free the instruction memory*/
    while(*instruction_memory != NULL) {
        temp = *instruction_memory;
        *instruction_memory = (*instruction_memory)->next;
        safe_free((void**)&temp);
    }
}

