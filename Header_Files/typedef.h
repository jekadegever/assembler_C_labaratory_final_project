
#ifndef TYPEDEF_H
#define TYPEDEF_H


/**
 * @typedef data_ptr
 * @brief Pointer to a node in the data memory linked list.
 *
 * Used to reference and manage dynamically allocated nodes
 * that represent `.data`, `.string`, or `.mat` directives
 * in the assembler’s data memory.
 */
typedef struct data_mem *data_ptr;

/**
 * @typedef instruction_ptr
 * @brief Pointer to a node in the instruction memory linked list.
 *
 * Each node holds an encoded machine instruction and its assigned address.
 */
typedef struct inst_mem *instruction_ptr;

/**
 * @typedef external_ptr
 * @brief Pointer to a node in the externals usage list.
 *
 * Each node tracks usage of an external label and the memory address
 * where the label was referenced.
 */
typedef struct external *external_ptr;

/**
 * @typedef address_update_request_ptr
 * @brief Pointer to a node in the address update request list.
 *
 * Holds a reference to an operand and the instruction memory address
 * that requires relocation once all labels are resolved.
 */
typedef struct address_update_request *address_update_request_ptr;

/**
 * @typedef macro_ptr
 * @brief Pointer to a node in the macro definitions list.
 *
 * Each node represents a macro, including its name, content,
 * and number of lines it expands to.
 */
typedef struct macro *macro_ptr;

/**
 * @typedef lines_map_ptr
 * @brief Pointer to a node in the source-to-preprocessed line mapping list.
 *
 * Maps an original `.as` file line number to its corresponding `.am` file line number,
 * ensuring accurate error reporting after preprocessing.
 */
typedef struct lines_LUT *lines_map_ptr;

/**
 * @typedef label_ptr
 * @brief Pointer to a node in the labels list.
 *
 * Each node stores a label name, its address, type (CODE/DATA),
 * definition type (NORMAL/EXTERN), and whether it is marked as entry.
 */
typedef struct label *label_ptr;

/**
 * @typedef opcode_ptr
 * @brief Pointer to an opcode structure in the opcode table.
 *
 * Provides information about an opcode, including its binary code,
 * name, number of operands, valid addressing modes, and encoding type.
 */
typedef struct opcode *opcode_ptr;


/**
 * @typedef const_opcode_ptr
 * @brief Pointer to a constant opcode structure in the opcode table.
 *
 * Provides information about an opcode, including its binary code,
 * name, number of operands, valid addressing modes, and encoding type.
 */
typedef const struct opcode* const_opcode_ptr;


#endif
