
#ifndef LABELS_H
#define LABELS_H
#include "boolean.h"
#include "context.h"
#include "typedef.h"


/**
 * @enum def_type
 * @brief Specifies how a label is defined.
 *
 * - NORMAL: Regular label.
 * - EXTERN: Label declared as external.
 */
typedef enum def_type {
    NORMAL,
    EXTERN
} def_type;

/**
 * @enum addr_type
 * @brief Indicates the memory region.
 *
 * - CODE: Label refers to instruction memory.
 * - DATA: Label refers to data memory.
 * - UNKNOWN_ADDR_TYPE: Type not yet determined.
 */
typedef enum addr_type {
    CODE,
    DATA,
    UNKNOWN_ADDR_TYPE
} addr_type;

/**
 * @struct label
 * @brief Represents a label in the assembler.
 *
 * Each label is stored in a linked list and contains:
 *   - name: Label name (dynamically allocated).
 *   - address: The memory address associated with the label.
 *   - type: Memory type (CODE or DATA).
 *   - definition: Definition type (NORMAL or EXTERN).
 *   - is_entry: Marks whether the label is defined as entry.
 *   - next: Pointer to the next label in the list.
 */
typedef struct label {
    char *name;
    unsigned int address;
    addr_type type;
    def_type definition;
    label_ptr next;
    boolean is_entry;
} label;



/**
 * @brief Search for and extract a label definition from a line of assembly code.
 *
 * If a label definition exists, the function:
 *   - Removes the label from the line.
 *   - Returns a newly allocated string with the label's name (caller must free).
 *
 * If no label is found, returns NULL and leaves the line unchanged.
 *
 * @param line Input line to check and modify.
 * @param asmContext Global assembler context (for validation and error reporting).
 * @return char* Newly allocated label name, or NULL if no label is defined.
 */
char *find_label_definition(char *line, assembler_context *asmContext);

/**
 * @brief Remove a label definition from a line, if it exists.
 *
 * If a label definition is present (ends with ':'), it is stripped from
 * the line buffer. If not, the line is unchanged.
 * necessary for the second pass when its ignoring any label definition;
 * @param line Input line to modify.
 */
void remove_label_from_line(char *line);

/**
 * @brief Add a new label node to the labels linked list.
 *
 * Creates a new label, initializes it with provided parameters, and
 * appends it to the end of the list.
 *
 * @param new_name Label name to add.
 * @param address Address associated with the label.
 * @param type Address type (CODE, DATA, UNKNOWN_ADDR_TYPE).
 * @param definition Label definition type (NORMAL, EXTERN).
 * @param labels_list Pointer to the head of the labels list.
 * @return true if successfully added, false otherwise.
 */
boolean add_label(const char *new_name, unsigned int address, addr_type type, def_type definition, label_ptr *labels_list);

/**
 * @brief Free the entire labels list.
 *
 * Iterates through all label nodes and releases their allocated memory.
 *
 * @param labels_list Pointer to the head of the labels list.
 */
void free_label_list(label_ptr *labels_list);

/**
 * @brief Print all labels in the list with their properties.
 *
 * Prints label name, address, type (code/data), and definition (entry/external/default).
 *
 * @param labels_list Head of the labels list.
 */
void print_labels(label_ptr labels_list);

/**
 * @brief Get the address of a label by name.
 *
 * Searches the labels list and returns the address if the label exists.
 * Returns 0 if the label is not found.
 *
 * @param name Name of the label.
 * @param labels_list Head of the labels list.
 * @return unsigned int Label address, or 0 if not found.
 */
unsigned int get_label_address(const char* name, label_ptr labels_list);

/**
 * @brief Check if a label with a given name is defined.
 *
 * @param name Name of the label to search for.
 * @param labels_list Head of the labels list.
 * @return true if label exists, false otherwise.
 */
boolean is_label_defined(const char *name, label_ptr labels_list);

/**
 * @brief Retrieve the full label node by name.
 *
 * Searches the labels list and returns the label node if found.
 *
 * @param name Name of the label.
 * @param labels_list Head of the labels list.
 * @return label_ptr Pointer to the label node, or NULL if not found.
 */
label_ptr get_label(const char* name, label_ptr labels_list);

/**
 * @brief Print all labels marked as "entry".
 *
 * Displays entry labels and their addresses in binary form.
 *
 * @param labels_list Head of the labels list.
 */
void print_entry_labels(label_ptr labels_list);

/**
 * @brief Check if any label in the list is marked as "entry".
 *
 * @param labels_list Head of the labels list.
 * @return true if at least one entry label exists, false otherwise.
 */
boolean is_entry_label_exist(label_ptr labels_list);


#endif
