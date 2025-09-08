

#include <stdio.h>
#include "externals.h"
#include "errors.h"
#include "labels.h"
#include "util.h"
#include "sys_memory.h"


/**
 * @file externals.h
 * @brief Management of external label usages in the assembler.
 *
 * This module provides data structures and functions to handle
 * `.extern` directives encountered during the assembly process.
 * Each external label usage (a label defined outside the current
 * source file but referenced inside it) is tracked along with the
 * memory address where it is used.
 *
 * Features:
 *  - Add new external label usage to a linked list.
 *  - Check whether external labels were used.
 *  - Print all external label usages in a human-readable format.
 *  - Free all memory allocated for the externals list.
 *
 * Data is stored in a simple singly linked list, where each node
 * represents one label usage with its name and memory address.
 *
 * @note This module ensures external references are properly
 *       collected so that an `.ext` file can be generated
 *       in the assembler's output stage.
 *
 * @author Ivgeny Tokarzhevsky
 * @date 01/09/2025
 */





boolean add_external_usage(const char* label_name, unsigned int mem_addr, external_ptr *externals_list) {
    char* extern_name;
    external_ptr temp;
    external_ptr new_extern_node = NULL;

    /*validate input parameters*/
    if (!label_name ||!externals_list) {
        print_internal_error(ERROR_CODE_25,"add_external_usage");
        return false;
    }
    /*allocate memory for new node*/
    new_extern_node = (external_ptr)handle_malloc(sizeof(external));


    /*insert value into the new allocated node*/
    extern_name = copy_string(label_name);
    if (extern_name == NULL) {return false;}

    new_extern_node->label_name = extern_name;
    new_extern_node->mem_address = mem_addr;
    new_extern_node->next=NULL;

    /*insert the node at the head if it's empty*/
    if(*externals_list == NULL) {
        *externals_list = new_extern_node;
        return true;
    }

    /*otherwise append in the end of the list*/
    else {
        temp = *externals_list;
        while(temp->next != NULL) {
            temp = temp->next;
        }

        temp->next = new_extern_node;

        return true;
    }

}


void print_external_usages(external_ptr externals_list) {

    /*prints each node's values*/
    while (externals_list != NULL) {
        printf("extern label: %s -> used in address: ", externals_list->label_name);
        print_binary(externals_list->mem_address,10);
        printf("\n");
        externals_list = externals_list->next;
    }
}


void free_externals_usage_list(external_ptr *externals_list) {
    external_ptr temp = NULL;

    /*free each node including the label attached name*/
    while (*externals_list != NULL) {
        temp=*externals_list;
        *externals_list = (*externals_list)->next;

        safe_free((void**)&temp->label_name);
        safe_free((void**)&temp);
    }
}

boolean is_externals_usage_exist(external_ptr externals_list) {
    /*return true if the head is not empty*/
    return externals_list!=NULL;
}

