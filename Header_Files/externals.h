


#ifndef EXTERNALS_H
#define EXTERNALS_H
#include "typedef.h"
#include "externals.h"
#include "util.h"


/**
 * @brief Represents a single external label usage.
 *
 * Each node stores the name of an external label and the memory
 * address where it was referenced. The nodes are linked together
 * in a singly linked list.
 */
typedef struct external {
    char* label_name;           /**< The name of the external label. */
    unsigned int mem_address;   /**< The memory address where the label is used. */
    external_ptr next;          /**< Pointer to the next external usage in the list. */
} external;


/**
 * @brief Add a new external label usage to the externals list.
 *
 * Allocates a new node, copies the label name, and appends it
 * to the end of the linked list.
 *
 * @param label_name     The name of the external label.
 * @param mem_addr       The memory address where the label is referenced.
 * @param externals_list Pointer to the head of the externals list.
 *
 * @return true on success, false if memory allocation fails or input is invalid.
 */
boolean add_external_usage(const char* label_name, unsigned int mem_addr, external_ptr *externals_list);


/**
 * @brief Print all external label usages.
 *
 * Iterates over the externals list and prints each label name
 * and its corresponding memory address.
 *
 * @param externals_list Head of the externals list.
 */
void print_external_usages(external_ptr externals_list);


/**
 * @brief Check if there are any external label usages.
 *
 * @param externals_list Head of the externals list.
 * @return true if the list contains at least one usage, false otherwise.
 */
boolean is_externals_usage_exist(external_ptr externals_list);


/**
 * @brief Free the entire externals usage list.
 *
 * Releases all memory allocated for label names and list nodes.
 *
 * @param externals_list Pointer to the head of the externals list.
 */
void free_externals_usage_list(external_ptr *externals_list);


#endif
