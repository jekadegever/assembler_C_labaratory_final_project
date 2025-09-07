
#ifndef MEMORY_RELOCATION_H
#define MEMORY_RELOCATION_H


#include "context.h"
#include "instructions.h"





/**
 * @brief Represents a pending request to update an instruction word with a resolved label address.
 *
 * During the first pass, operands that reference labels cannot be fully encoded
 * because label addresses are not yet known. Instead, an address-update request
 * is queued. Later, in the second pass, these requests are processed once all
 * labels have been resolved.
 *
 * Fields:
 *  - operand : Pointer to the operand that references the label.
 *  - address : Instruction memory address of the word that requires patching.
 *  - next    : Pointer to the next request in the list.
 */
typedef struct address_update_request {
    operand* operand;                  /**< Operand referencing the unresolved label. */
    unsigned int address;              /**< Instruction memory address to patch. */
    address_update_request_ptr next;   /**< Next request in the linked list. */
} address_update_request;


/**
 * @brief Add a new address update request to the list.
 *
 * Used when encoding operands that reference labels whose addresses
 * are not yet resolved (e.g., direct or matrix operands). The request
 * will later be processed to patch the final label address.
 *
 * @param IC_addr       Instruction counter (IC) address to be updated.
 * @param operand       Pointer to the operand requiring address resolution.
 * @param request_list  Pointer to the head of the address update request list.
 * @return true on success, false on failure.
 */
boolean add_addr_update_request(unsigned int IC_addr, operand* operand, address_update_request_ptr *request_list);


/**
 * @brief Print all address update requests in the list.
 *
 * Prints the operand label name and the instruction memory address
 * where the final label address should be patched.
 *
 * @param request_list Head of the address update request list.
 */
void print_addr_update_requests(address_update_request_ptr request_list);


/**
 * @brief Process all pending address update requests.
 *
 * Resolves label references in operands by updating the corresponding
 * instruction memory words with the correct relocated label addresses.
 * Also updates encoding type (e.g., EXTERNAL or RELOCATABLE).
 *
 * @param asmContext Pointer to the assembler context containing labels,
 *                   instruction memory, and request list.
 * @return true if all requests were successfully processed, false otherwise.
 */
boolean process_addr_update_requests(assembler_context *asmContext);


/**
 * @brief Relocate data memory after instruction memory.
 *
 * Adjusts the addresses of all data memory words by adding the instruction
 * counter (IC) size and the global memory offset, ensuring data memory
 * is placed after the instruction memory.
 *
 * @param asm_context Pointer to the assembler context containing data memory.
 * @return true on success, false on failure.
 */
boolean relocate_data_memory(assembler_context *asm_context);


/**
 * @brief Update all label addresses after relocation.
 *
 * Adjusts label addresses based on their type:
 * - CODE labels are shifted by the memory offset.
 * - DATA labels are shifted by the memory offset and the final IC.
 *
 * @param asmContext Pointer to the assembler context containing labels.
 * @return true on success, false on failure.
 */
boolean update_labels_addresses(assembler_context *asmContext);


/**
 * @brief Free the address update request list.
 *
 * Releases all dynamically allocated memory used by the address update
 * request list, including associated operand copies.
 *
 * @param request_list Pointer to the head of the request list.
 */
void free_addr_update_req_list(address_update_request_ptr *request_list);


/**
 * @brief Relocate instruction memory.
 *
 * Shifts the addresses of all instructions in memory by the global
 * memory offset to ensure correct placement in the final memory layout.
 *
 * @param asmContext
 * @return true on success, false on failure.
 */
boolean relocate_instructions_memory(assembler_context *asmContext);


/**
 * @brief Update request list addresses after relocation.
 *
 * Adjusts all instruction addresses stored in the address update request
 * list by adding the global memory offset.
 *
 * @param request_list Head of the address update request list.
 */
void update_request_list_addresses(address_update_request_ptr request_list);





/**
 * @brief Remove previously generated assembler output files.
 *
 * This function deletes all old files (if exist) related to the current source file:
 *   - Object file (.obj)
 *   - Preprocessed file (.am)
 *   - Entry file (.ent)
 *   - Externals file (.ext)
 *
 * For each file type, the file name is rebuilt using `change_file_extension()`,
 * then removed with `remove()`.
 * After removal, the dynamically allocated file name strings stored in
 * `asmContext` are freed using `safe_free()`.
 *
 * @param asmContext Pointer to the assembler context containing the source
 *                   file name and the associated output file names.
 *
 * @note This function should be called at the start of an assembly process to
 *       ensure no outdated or conflicting files remain from previous runs.
 *       All file name pointers in the context are released, so they must not
 *       be accessed afterward.
 */
void remove_old_files(assembler_context *asmContext);


#endif
