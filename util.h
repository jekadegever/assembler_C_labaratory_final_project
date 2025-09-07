
#ifndef UTIL_H
#define UTIL_H
#include "boolean.h"
#include "context.h"


/**
 * @enum line_type
 * @brief Represents the classification of an assembly source line.
 *
 * Each parsed line is categorized into one of the following types:
 *
 * - UNKNOWN_LINE          → The line type could not be determined
 *                           (invalid or unrecognized syntax).
 * - INSTRUCTION_LINE      → A valid instruction line containing an opcode
 *                           and optional operands.
 * - DATA_DIRECTIVE_LINE   → A data directive line such as `.data`, `.string`, `.mat`.
 * - ENTRY_DIRECTIVE_LINE  → An `.entry` directive line, marking a label as entry.
 * - EXTERN_DIRECTIVE_LINE → An `.extern` directive line, declaring an external label.
 *
 * This enum is used by the assembler to decide how to handle each line
 * during the first and second passes.
 */
typedef enum {
    UNKNOWN_LINE,
    INSTRUCTION_LINE,
    DATA_DIRECTIVE_LINE,
    ENTRY_DIRECTIVE_LINE,
    EXTERN_DIRECTIVE_LINE
} line_type;


 /**
  * @brief Make an allocated copy of the input string.
  * @param string Source string (must not be NULL).
  * @return Newly allocated copy, or NULL on allocation failure.
  */
char* copy_string(const char *string);

/**
 * @brief Validate an identifier (label/macro) name.
 * Rules: length ≤ NAME_MAX_LEN, starts with alpha, continues with [A-Za-z0-9_].
 * @param str The candidate name.
 * @return true if valid, false otherwise.
 */
boolean is_name_valid(const char *str);

/**
 * @brief Check that a name is available (not reserved and not already used).
 * Tests against opcodes, directives, registers, preprocessor words, labels, macros.
 * @param str Candidate name.
 * @param asmContext Assembler context (tables, symbol lists).
 * @return true if can be added, false otherwise.
 */
boolean can_add_name(const char *str, assembler_context *asmContext);

/**
 * @brief Trim spaces from the both sides of the string.
 * @param str String to trim (ignored if NULL).
 */
void trim_edge_white_space(char *str);

/**
 * @brief Remove spaces immediately inside square brackets “[]”
 * only near the brackets.
 * Example: "[  r1  ][ r2 ]" -> "[r1][r2]" ,"[ r 2][r 1]" -> "[r 2][r 1]".
 * @param line Line buffer (ignored if NULL).
 */
void trim_bracket_edge_spaces(char *line);

/**
 * @brief Check if a string contains exactly one token (one “word”).
 * Leading/trailing spaces are allowed; no internal spaces.
 * @param str Input string.
 * @return true if a single word, false otherwise.
 */
boolean is_single_word(const char *str);

/**
 * @brief Print an unsigned integer as in binary number in provided bits size.
 * @param num  The number to print.
 * @param bits Number of bits to print (MSB first).
 */
void print_binary(unsigned int num, int bits);

/**
 * @brief Classify an input line (instruction / directives / unknown).
 * Uses opcode and directives tables from the context.
 * @param line Input line.
 * @param asmContext Context with tables.
 * @return Detected line_type (INSTRUCTION_LINE, DATA_DIRECTIVE_LINE, ENTRY_DIRECTIVE_LINE,
 *         EXTERN_DIRECTIVE_LINE, or UNKNOWN_LINE).
 */
line_type get_line_type(const char *line, assembler_context *asmContext);

/**
 * @brief Safe realloc with uniform error reporting.
 * On failure prints a system error and returns NULL.
 * @param pointer Previous allocation (may be NULL).
 * @param size    New size in bytes.
 * @return New pointer or NULL on failure.
 */
void* handle_realloc(void* pointer, unsigned long size);

/**
 * @brief Allocates memory safely with unified error handling.
 *
 * Wraps the standard `malloc()` function and provides consistent
 * error reporting. If allocation fails, a system error is printed
 * and the program is terminated — ensuring that the caller does not
 * need to manually check for allocation failures.
 *
 * @param size Number of bytes to allocate.
 * @return void* Pointer to the allocated memory block.
 *
 * @note This function never returns NULL. In case of failure,
 *       it reports the error and stops the program.
 */
void* handle_malloc(unsigned long size);

/**
 * @brief Test whether a line is empty or a comment.
 * A comment line starts (after optional spaces) with ';'.
 * @param line Input line.
 * @return true if empty/comment, false otherwise.
 */
boolean is_comment_or_empty_line(const char *line);

/**
 * @brief Check if a name is reserved by the system (opcodes, directives, registers, PP words).
 * @param name Candidate name.
 * @param asmContext Context for lookups.
 * @return true if reserved, false otherwise.
 */
boolean is_sys_saved_name(const char *name, assembler_context *asmContext);

/**
 * @brief Check if a name is already used by a label or macro.
 * @param name Candidate name.
 * @param asmContext Context holding labels/macros lists.
 * @return true if already used, false otherwise.
 */
boolean is_used_name(const char *name, assembler_context *asmContext);

/**
 * @brief Convert an unsigned number to base-4 string using digits {a,b,c,d}.
 * If @p word_len == -1 the length is minimal; otherwise pads to fixed width.
 * @param num       Number to convert.
 * @param word_len  Target width or -1 for minimal width.
 * @param result_out Output buffer (must be large enough).
 */
void to_base4_str(unsigned int num, int word_len, char *result_out);

/**
 * @brief Free all dynamic structures stored in the assembler context.
 * Frees: fixup list, macros, labels, data memory, instruction memory,
 * externals list, and lines map.
 * @param asmContext Assembler context.
 */
void free_all_memory(assembler_context *asmContext);

/**
 * @brief Free a pointer and set it to NULL (if not already NULL).
 * @param ptr Address of the pointer to free.
 */
void safe_free(void **ptr);


/**
 * @brief Print debug values while program in debug mode (macro names, labels
 * instruction memory image, data image, IC/DC etc...).
* @param asmContext Assembler context.
 */
void debug_data_print(const assembler_context *asmContext);
#endif
