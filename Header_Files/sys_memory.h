

#ifndef SYS_MEMORY_H
#define SYS_MEMORY_H

#include <context.h>


/**
 * @brief Free all memory tracked by the allocation system.
 *
 * Iterates over the tracking list, freeing both the allocated memory blocks
 * and the tracking nodes themselves. Resets the tracking list head to NULL.
 */
void free_all_tracked_allocations(void);


/**
 * @brief Safe malloc wrapper with tracking and error handling.
 *
 * Allocates memory of the given size and tracks the allocation.
 * If the allocation fails, a system error is printed, all tracked
 * memory is freed, and the program exits.
 *
 * @note Because this function terminates the program on failure,
 *       callers do NOT need to check the return value for NULL.
 *
 * @param size Number of bytes to allocate.
 * @return Pointer to allocated memory (guaranteed non-NULL).
 */
void* handle_malloc(unsigned long size);


/**
 * @brief Safe realloc wrapper with tracking and error handling.
 *
 * Reallocates memory for a given pointer and updates the tracking list.
 * Handles both fresh allocations (pointer == NULL) and pointer relocation.
 * If reallocation fails, a system error is printed, all tracked
 * memory is freed, and the program exits.
 *
 * @note Because this function terminates the program on failure,
 *       callers do NOT need to check the return value for NULL.
 *
 * @param pointer Pointer to the previously allocated memory (may be NULL).
 * @param size New allocation size in bytes.
 * @return Pointer to the new memory block (guaranteed non-NULL).
 */
void* handle_realloc(void* pointer, unsigned long size);


/**
 * @brief Safe free wrapper with tracking removal.
 *
 * Ensures the freed pointer is removed from the tracking list
 * and sets the caller's pointer to NULL to prevent dangling pointers.
 *
 * @param ptr_ptr Address of the pointer to free (e.g., (void**)&my_ptr).
 */
void safe_free(void **ptr_ptr);


/**
 * @brief Free all memory associated with the assembler context.
 *
 * Calls free functions for all linked lists, frees context-owned strings,
 * and finally clears any remaining tracked allocations.
 *
 * @param asmContext Pointer to the assembler context to clean up.
 */
void free_all_memory(assembler_context *asmContext);





#endif
