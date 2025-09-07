
#ifndef PRE_PROCESSOR_H
#define PRE_PROCESSOR_H

#include <stdio.h>
#include "boolean.h"
#include "typedef.h"
#include "context.h"

/**
 * @brief Run the assembler preprocessor on a single source file.
 *
 * Expands macros from the input .as file into a generated .am file:
 *  - Scans for macro declarations (mcro/mcroend), validates names and duplicates.
 *  - Captures macro bodies and stores them in a linked list.
 *  - Replaces macro invocations with their stored content.
 *  - Maintains a line mapping (.as → .am) for accurate error reporting.
 *
 * On success, writes the expanded content to asmContext->am_file_name.
 *
 * @param asmContext Assembler context (file names, macro list, line map, flags).
 * @return true on success, false on any error (also sets asmContext->preproccess_error).
 */
boolean execute_preprocessor(assembler_context* asmContext);

/**
 * @brief Append a new macro node to the macro list.
 *
 * The function takes ownership of @p name and @p content pointers.
 *
 * @param name         macro name (NUL-terminated).
 * @param content      macro body content (including trailing newlines).
 * @param lines_amount Number of lines in @p content.
 * @param macro_list   Pointer to list head.
 * @return true on success, false on allocation or argument error.
 */
boolean add_macro(char *name, char *content, int lines_amount, macro_ptr *macro_list);

/**
 * @brief Print macro names and their content (debug helper).
 *
 * @param macro_list Head of the macro list (may be NULL).
 */
void print_macro_names(macro_ptr macro_list);

/**
 * @brief Free the entire macro list.
 *
 * Frees each node's name, content, and the node itself; sets *macro_list to NULL.
 *
 * @param macro_list Pointer to list head.
 */
void free_macro_list(macro_ptr *macro_list);

/**
 * @brief Detect a macro declaration line and return the macro name.
 *
 * Accepts lines of the form: "mcro <name>" with no trailing tokens.
 * Returns a newly allocated copy of <name> on success; NULL otherwise.
 *
 * @param line       Line from file.
 * @param asmContext Context (for reserved words and error reporting).
 * @return char* Newly allocated macro name, or NULL if not a declaration / on error.
 */
char *is_start_of_macro(char *line, assembler_context *asmContext);

/**
 * @brief Check whether a line marks the end of a macro body.
 *
 * Accepts a single token "mcroend" with no trailing tokens.
 *
 * @param line       Line from file.
 * @param asmContext Context (for error reporting if trailing tokens appear).
 * @return true if the line is a valid macro end; false otherwise.
 */
boolean is_end_of_macro(char *line, assembler_context *asmContext);

/**
 * @brief Read a macro body from the current file position until 'mcroend'.
 *
 * Accumulates lines into a single heap-allocated buffer and counts lines.
 * Fails if 'mcroend' is missing or the macro body is empty.
 *
 * @param fp_in           Open FILE* positioned after the 'mcro <name>' line.
 * @param content_out     Out: heap-allocated buffer with the macro body.
 * @param lines_count_out Out: number of lines in the macro body.
 * @param asmContext      Context (line counters, errors).
 * @return true on success, false on error.
 */
boolean read_macro_content(FILE *fp_in, char** content_out, int *lines_count_out, assembler_context *asmContext);

/**
 * @brief Determine whether a line is a macro invocation.
 *
 * If the first token matches a defined macro name and no extra tokens exist,
 * returns the corresponding macro node; otherwise returns NULL.
 *
 * @param line        Line from file.
 * @param macro_list  Macro list head.
 * @param asmContext  Context for error reporting on trailing tokens.
 * @return macro_ptr Node of the invoked macro, or NULL if not a call / on error.
 */
macro_ptr is_macro_call(char *line, macro_ptr macro_list, assembler_context *asmContext);

/**
 * @brief Find and return a macro node by name.
 *
 * @param name       Macro name to search.
 * @param macro_list Macro list head.
 * @return macro_ptr Matching node or NULL if not found.
 */
macro_ptr get_macro(char *name, macro_ptr macro_list);

/**
 * @brief Check whether a macro with the given name is defined.
 *
 * @param name       Macro name.
 * @param macro_list Macro list head.
 * @return true if defined, false otherwise.
 */
boolean is_macro_defined(const char *name, macro_ptr macro_list);

/**
 * @brief Check if a name is reserved by the preprocessor (e.g., "mcro", "mcroend").
 *
 * @param name       Candidate name.
 * @param asmContext Context containing the reserved words table.
 * @return true if reserved; false otherwise.
 */
boolean is_PP_saved_name(const char* name, const assembler_context *asmContext);

/**
 * @brief Macro directive kinds recognized by the preprocessor.
 */
typedef enum {
    MACRO_START,  /**< "mcro" */
    MACRO_END     /**< "mcroend" */
} macro_declarations;

/**
 * @brief Macro list node.
 *
 * Represents a single macro definition collected during preprocessing.
 * The @p name and @p content pointers are heap-allocated and owned by this node.
 */
typedef struct macro {
    char *name;          /**< Macro name (NUL-terminated). */
    char *content;       /**< Full macro body as a single string. */
    int   lines;         /**< Number of lines in @p content. */
    macro_ptr next;      /**< Next node in the macro list. */
}macro;




#endif

