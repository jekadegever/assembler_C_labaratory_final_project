
#include "sys_memory.h"
#include <addresses.h>
#include <data_memory.h>
#include <labels.h>
#include <pre_processor.h>
#include "errors.h"
#include <stdlib.h>
#include "externals.h"
#include "lines_map.h"
#include "instruction_memory.h"


/**
 * @file sys_memory.c
 * @brief Centralized memory management for the assembler.
 *
 * This module wraps all dynamic memory operations (`malloc`, `realloc`, `free`)
 * with tracking logic to guarantee safe allocation and cleanup.
 *
 * Features:
 *  - Tracks every allocation in a linked list (`allocationNode`).
 *  - Ensures proper deallocation via `safe_free()` and `free_all_tracked_allocations()`.
 *  - Automatically updates tracking when memory is reallocated.
 *  - On allocation failure:
 *      - Prints a system error.
 *      - Frees all tracked allocations.
 *      - Terminates the program gracefully.
 *
 * Usage notes:
 *  - Always use `handle_malloc()`, `handle_realloc()`, and `safe_free()` instead
 *    of raw `malloc`, `realloc`, or `free`.
 *  - Call `free_all_memory()` at the end of each file’s assembly or when aborting
 *    to release both assembler data structures and tracked allocations.
 *  - Because allocation failures terminate the program, **no need to check for NULL**
 *    after `handle_malloc()` or `handle_realloc()`.
 *
 * This design eliminates memory leaks and ensures consistent memory management
 * across the entire assembler project.
 *
 * @author Ivgeny Tokarzhevsky
 * @date 28/08/2025
 */

/**==================================== PROTOTYPES =======================================*/
/**
 * @struct AllocationNode
 * @brief Node in the memory tracking linked list.
 *
 * Each dynamically allocated pointer is wrapped in an `allocationNode`
 * and linked into a global list. This allows:
 *  - Centralized tracking of active allocations.
 *  - Automatic cleanup via `free_all_tracked_allocations()`.
 *  - Safe updates when reallocating memory.
 *
 * @var AllocationNode::ptr
 *   Pointer to the allocated memory block.
 *
 * @var AllocationNode::next
 *   Pointer to the next node in the tracking list (NULL if last).
 */
typedef struct AllocationNode {
    void *ptr;                    /**< Pointer to allocated memory block. */
    struct AllocationNode *next;  /**< Next node in tracking list. */
} allocationNode;

/**
 * @brief Add a newly allocated pointer to the tracking list.
 *
 * Keeps track of dynamically allocated memory so it can be freed later
 * (via safe_free() or free_all_tracked_allocations()).
 *
 * @param ptr Pointer to the allocated memory block.
 */
static void allocation_track_add(void *ptr);


/**
 * @brief Remove a pointer from the tracking list.
 *
 * Used when a memory block is freed to ensure the tracking list stays in sync.
 *
 * @param ptr Pointer to the memory block being freed.
 */
static void allocation_track_remove(const void *ptr);


/**
 * @brief Update an existing tracked pointer after realloc().
 *
 * If realloc() changes the pointer address, this updates the entry in the list.
 * If the old pointer is not found, the new pointer is added.
 *
 * @param old_ptr The old pointer before reallocation.
 * @param new_ptr The new pointer returned by realloc().
 */
static void allocation_track_update(const void *old_ptr, void *new_ptr);





/**==================================== CODE =======================================*/



static allocationNode *allocation_node_ptr = NULL;

static void allocation_track_add(void *ptr) {


    allocationNode *node;

    if (!ptr) return;/*not input ptr*/

    node = (allocationNode*)malloc(sizeof(allocationNode));/*allocate memory for ptr*/
    if (!node) return; /*in the worst case, node wouldn't be tracked*/

    /*set values*/
    node->ptr = ptr;
    node->next = allocation_node_ptr;
    allocation_node_ptr = node;
}

static void allocation_track_remove(const void *ptr) {

    allocationNode *previous = NULL;
    allocationNode *current = allocation_node_ptr;

    /*search for the input node*/
    while (current) {

        if (current->ptr == ptr) {
            /*node found*/
            if (previous != NULL) {

                /*remove the current node*/
                previous->next = current->next;
            }
            else {
                allocation_node_ptr = current->next;
            }
            /*free the removed node memory*/
            free(current);
            return;
        }
        /*search in the next node*/
        previous = current;
        current = current->next;
    }
}

static void allocation_track_update(const void *old_ptr, void *new_ptr) {
/*used to change pointer on memory reallocation*/

    allocationNode *current = allocation_node_ptr;

    /*search for the old node*/
    while (current) {

        if (current->ptr == old_ptr) {
            /*set the old node as new node*/
            current->ptr = new_ptr;
            return;
        }
        current = current->next;
    }
    /*if old pointer wasn't exist or NULL, add the new ptr*/
    allocation_track_add(new_ptr);
}


void free_all_tracked_allocations(void) {

    allocationNode *current = allocation_node_ptr;
    allocationNode *next = NULL;
    while (current) {
        /*hold the next node and free the current*/
        next = current->next;

        /*free the ptr if not NULL*/
        if (current->ptr) {
            free(current->ptr);
        }

        /*free the node*/
        free(current);
        current = next;
    }

    /*set the head as NULL*/
    allocation_node_ptr = NULL;
}



void* handle_malloc(unsigned long size) {

    void *ptr;

    /*allocate memory*/
    ptr = (void*)malloc(size);
    if (ptr == NULL) {
        /*allocation failed, print an error*/
        print_system_error(ERROR_CODE_1);
    }

    /*add allocated memory pointer to track*/
    allocation_track_add(ptr);
    /*return ptr*/
    return ptr;
}

void* handle_realloc(void* pointer, unsigned long size) {
    void *new_ptr;

    new_ptr = (void*)realloc(pointer, size);
    if (new_ptr == NULL) {
        print_system_error(ERROR_CODE_11);  /* “Memory reallocation failed” */
        return NULL;
    }

    /*if the input pointer wasn't allocated ( initial allocation)*/
    if (pointer == NULL) {
        /*add pointer to tracking list*/
        allocation_track_add(new_ptr);

    }
    /*verify that the address not changed*/
    else if (new_ptr != pointer) {

        /*update the tacking list with new address*/
        allocation_track_update(pointer, new_ptr);


    }/*pointer address didn't change*/


    return new_ptr;
}



void safe_free(void **ptr_ptr) {

    /*verify that the input is not NULL, and the pointer value not NULL*/
    if (ptr_ptr && *ptr_ptr) {

        /*remove the pointer from tracking list*/
        allocation_track_remove(*ptr_ptr);
        /*free memory*/
        free(*ptr_ptr);
        /*set the pointer as NULL*/
        *ptr_ptr = NULL;
    }
}





void free_all_memory(assembler_context *asmContext) {

    /*execute free functions for all linked lists*/

    free_addr_update_req_list(&asmContext->address_update_requests);
    free_macro_list(&asmContext->macros);
    free_label_list(&asmContext->labels);
    free_data_memory(&asmContext->data_memory);
    free_instruction_memory(&asmContext->instruction_memory);
    free_externals_usage_list(&asmContext->external_labels);
    free_lines_map(&asmContext->lines_maper);

    /*free assembler context allocated memory*/

    safe_free((void**)&asmContext->am_file_name);
    safe_free((void**)&asmContext->ext_file_name);
    safe_free((void**)&asmContext->obj_file_name);
    safe_free((void**)&asmContext->ent_file_name);
    safe_free((void**)&asmContext->as_file_name);
    safe_free((void**)&asmContext->bin_file_name);
    safe_free((void**)&asmContext->file_path);
    safe_free((void**)&asmContext->am_full_file_name);
    safe_free((void**)&asmContext->as_full_file_name);


    free_all_tracked_allocations();


}
