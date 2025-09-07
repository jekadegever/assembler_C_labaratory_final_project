

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "boolean.h"
#include "config.h"
#include "context.h"




/**
 * @brief Encoding type for machine words the ERA field.
 */
typedef enum {
    ABSOLUTE = 0,   /**< Absolute encoding. */
    EXTERNAL = 1,   /**< External encoding. */
    RELOCATABLE = 2,/**< Relocatable encoding. */
    UNKNOWN = 3     /**< Unknown or not yet resolved. */
} encoding_type;


/**
 * @brief Addressing mode for operands.
 */
typedef enum {
    IMMEDIATE_ACCESS = 0, /**< Immediate value (#num). */
    DIRECT_ACCESS   = 1,  /**< Direct access (label). */
    MATRIX_ACCESS   = 2,  /**< Matrix access (label[r1][r2]). */
    REGISTER_ACCESS = 3   /**< Register access (r0–r7). */
} addr_Mode;

#define AM_BIT(m) (1u << (m))

/**
 * @brief Groups of allowed addressing modes for each opcode.
 */
typedef enum {
    AM_1_2   = (AM_BIT(DIRECT_ACCESS) | AM_BIT(MATRIX_ACCESS)),
    AM_1_2_3 = (AM_BIT(DIRECT_ACCESS) | AM_BIT(MATRIX_ACCESS) | AM_BIT(REGISTER_ACCESS)),
    ALL      = (AM_BIT(DIRECT_ACCESS) | AM_BIT(MATRIX_ACCESS) | AM_BIT(REGISTER_ACCESS) | AM_BIT(IMMEDIATE_ACCESS)),
    NONE     = 0
} addr_mode_group;


/**
 * @brief Structure representing an opcode entry in the opcode table.
 */
typedef struct opcode {
    int opcode;               /**< Numeric opcode value. */
    char *name;               /**< Opcode name. */
    int operands_amount;      /**< Expected number of operands (0–2). */
    addr_mode_group source;   /**< Allowed addressing modes for source operand. */
    addr_mode_group dest;     /**< Allowed addressing modes for destination operand. */
    encoding_type encoding;   /**< encoding type. */
} opcode;


/**
 * @brief Matrix operand details (label + two registers).
 */
typedef struct {
    char label[NAME_MAX_LEN + 1]; /**< Matrix label name. */
    int reg_1;                    /**< First register index (column). */
    int reg_2;                    /**< Second register index (row). */
    encoding_type reg_encoding;   /**< Encoding type for registers. */
} Mat_operand;


/**
 * @brief Operand value union (matrix, label, register, or immediate value).
 */
typedef union {
    Mat_operand matrix;                 /**< Matrix operand (struct). */
    char label[NAME_MAX_LEN + 1];       /**< Label operand (string). */
    int val;                            /**< Immediate operand (val). */
    int reg;                            /**< Register operand (val). */
} operand_val;


/**
 * @brief Structure representing a parsed operand.
 */
typedef struct {
    addr_Mode type;          /**< Addressing mode of operand. */
    operand_val operand_val; /**< Operand values (union). */
    encoding_type encoding;  /**< Encoding type of the operand. */
    int file_line;           /**< Source file operand usage line number. */
} operand;

/**
 * @brief Handles the parsing and encoding of a full instruction line.
 *
 * This function identifies the opcode, extracts operands, validates them,
 * and encodes the instruction into machine code. The encoded words are then
 * stored into the instruction memory.
 *
 * @param line        Input line containing the instruction.
 * @param asmContext  Pointer to the assembler context.
 *
 * @return true if the instruction was successfully parsed and encoded,
 *         false otherwise.
 */
boolean handle_instruction_line(char *line, assembler_context *asmContext);


/**
 * @brief Handles a single-operand instruction line.
 *
 * Extracts and validates one operand, parses it into an operand structure,
 * and verifies its addressing mode against the opcode definition.
 *
 * @param opcode_info Information about the current opcode.
 * @param entire_line The line containing the operand start after the opcode name..
 * @param dest_operand Output operand struct to be filled.
 * @param asmContext   Pointer to assembler context.
 *
 * @return true if the operand was successfully parsed, false otherwise.
 */
boolean handle_one_operand_line(opcode opcode_info, char *entire_line, operand *dest_operand, assembler_context *asmContext);


/**
 * @brief Handles a two-operand instruction line.
 *
 * Extracts and validates two operands, parses them into operand structures,
 * and checks their addressing modes against the opcode definition.
 *
 * @param opcode_info Information about the current opcode.
 * @param entire_line The line containing the operands starts after the opcode name.
 * @param src_operand Output source operand struct to be filled.
 * @param dest_operand Output destination operand struct to be filled.
 * @param asmContext   Pointer to assembler context.
 *
 * @return true if both operands were successfully parsed, false otherwise.
 */
boolean handle_two_operands_line(opcode opcode_info, char *entire_line, operand *src_operand, operand *dest_operand, assembler_context *asmContext);


/**
 * @brief Extracts tokens that represents the operands from a line of assembly code.
 *
 * Operands are split by commas according to format. Supports format errors detecting like
 * missing commas/unnecessary commas, multiple words per operand and unnecessary tokens in the end.
 *
 * @param line Input line to parse.
 * @param src_out  Pointer to store the source operand string (if found).
 * @param dest_out Pointer to store the destination operand string (if found).
 * @param asmContext Pointer to assembler context.
 *
 * @return Number of operands found (0, 1, or 2).
 *         Returns -1 if a format error was detected.
 */
int extract_operands(char *line, char **src_out, char **dest_out, assembler_context *asmContext);


/**
 * @brief Parses a string into an operand structure.
 *
 * Determines the operand type (register, immediate, matrix, direct/label)
 * and fills the operand struct accordingly.
 *
 * @param operand_str Input string representing the operand.
 * @param operand     Output operand struct to be filled.
 * @param asmContext  Pointer to assembler context.
 *
 * @return true if parsing succeeded, false otherwise.
 */
boolean parse_operand(char *operand_str, operand *operand, assembler_context *asmContext);


/**
 * @brief Returns the register number if the given string is a register.
 *
 * @param str        Input string.
 * @param asmContext Pointer to assembler context.
 *
 * @return Register number (0–7) if valid, -1 otherwise.
 */
int get_reg_num(const char* str, assembler_context *asmContext);


/**
 * @brief Checks if a string is a valid register name.
 *
 * @param str        Input string.
 * @param asmContext Pointer to assembler context.
 *
 * @return true if the string is a valid register name, false otherwise.
 */
boolean is_register(const char *str, assembler_context *asmContext);


/**
 * @brief Attempts to parse a matrix operand of the form label[reg][reg].
 *
 * Validates label name, register names, and overall format.
 * On success, fills the operand struct with the extracted values.
 *
 * @param operand_str Input string to parse.
 * @param operand     Output parsed operand struct.
 * @param asmContext  Pointer to assembler context.
 *
 * @return true if matrix operand found even if error found in the format
 * like (invalid label name or unknown register names) still return true,
 * false if the operand is not matrix operand.
 */
boolean try_parse_matrix_operand(const char *operand_str, operand *operand, assembler_context *asmContext);


/**
 * @brief Attempts to parse a register operand.
 *
 * @param operand_str Input string to check.
 * @param operand     Output operand struct.
 * @param asmContext  Pointer to assembler context.
 *
 * @return true if the string is a valid register operand, false otherwise.
 */
boolean try_parse_reg_operand(const char *operand_str, operand *operand, assembler_context *asmContext);


/**
 * @brief Attempts to parse an immediate operand  according to format ( # -> +/- (optional) -> number).
 *
 * @param operand_str Input string to check.
 * @param operand     Output operand struct.
 * @param asmContext  Pointer to assembler context.
 *
 * @return true if immediate operand found, even if error found in the format, false otherwise.
 */
boolean try_parse_immediate_operand(const char *operand_str, operand *operand, assembler_context *asmContext);


/**
 * @brief Attempts to parse a direct operand (label).
 *
 * @param operand_str Input string to check.
 * @param operand     Output operand struct.
 * @param asmContext  Pointer to assembler context.
 *
 * @return true if the string is a valid label operand, false otherwise.
 */
boolean try_parse_direct_operand(const char *operand_str, operand *operand, const assembler_context *asmContext);


/**
 * @brief Retrieves opcode information by name.
 *
 * Searches the opcode table for a given opcode name.
 *
 * @param opcode_name Input opcode string.
 * @param asmContext  Pointer to assembler context.
 *
 * @return Pointer to a newly allocated opcode struct if found, NULL otherwise.
 */
opcode* get_opcode_info(const char *opcode_name, assembler_context *asmContext);


/**
 * @brief Checks if a string matches a valid opcode name.
 *
 * @param name       Input string to check.
 * @param asmContext Pointer to assembler context.
 *
 * @return true if the string is a valid opcode, false otherwise.
 */
boolean is_opcode(const char *name, assembler_context *asmContext);

#endif
