

#ifndef DIRECTIVES_H
#define DIRECTIVES_H
#include "boolean.h"
#include "context.h"


/**
 * @brief enum of the available data directives.
 * also used as data directive table's index
 */
typedef enum  {
    DATA_DIRECTIVE,
    STRING_DIRECTIVE,
    MAT_DIRECTIVE

}data_directive;


/**
 * @brief enum of the available attribute directives.
 * also used as attribute directive table's index
 */
typedef enum {
    ENTRY_DIRECTIVE,
    EXTERN_DIRECTIVE
}attribute_directive;




/**
 * @brief Parse and handle a single data directive line (.data / .string / .mat).
 *
 * Locates the directive kind, parses its values, validates ranges,
 * and appends the resulting words to the data image (DC/memory_usage updated).
 *
 * @param line        the data directive line  (will be tokenized/advanced).
 * @param asmContext  Assembler context with tables and data image.
 * @return true on success, false on any parse/format/memory error.
 */
boolean handle_data_directives_line(char *line, assembler_context *asmContext);



/**
 * @brief Parse and handle a single data directive line (.data / .string / .mat).
 *
 * Locates the directive kind, parses its values, validates ranges,
 * and appends the resulting words to the data image (DC/memory_usage updated).
 *
 * @param line        Mutable line buffer (will be tokenized/advanced).
 * @param asmContext  Assembler context with tables and data image.
 * @return true on success, false on any parse/format/memory error.
 */
boolean handle_data_directives_line(char *line, assembler_context *asmContext);

/**
 * @brief Parse a .string directive payload into ascii values (including '\0').
 *
 * Extracts a quoted string, validates trailing content, converts to ascii,
 * and returns a newly allocated array with length in out_count and out_result.    .
 *
 * @param line        Line starting at the .string payload.
 * @param out_result  [out pointer] Newly allocated int array for string ascii values (with '\0'),
 * @param out_count   [out pointer] Number of chars found (len+1).
 * @param asmContext  Assembler context (for error reporting).
 * @return true on success, false on format or allocation error.
 * @note the caller func own the out params and must be free.
 */
boolean parse_string_directive(char *line, int **out_result, unsigned int *out_count, assembler_context *asmContext);




/**
 * @brief Parse a .data directive payload into integers.
 *
 * verify line format (.data -> numbers) and no unnecessary token after it.
 * Returns a newly allocated array and count of found numbers.
 *
 * @param line        Line starting at  the .data payload.
 * @param out_result  [out pointer] Newly allocated int array with declared numbers.
 * @param out_count   [out pointer] Number of found numbers.
 * @param asmContext  Assembler context (for error reporting).
 * @return true on success, false on format or allocation error.
 * @note the caller func own the out params and must be free.
 */
boolean parse_data_directive(char *line, int **out_result, unsigned int *out_count, assembler_context *asmContext);


/**
 * @brief Parse a .mat directive: dimensions "[rows][cols]" and the matrix numbers.
 *
 * Validates the mat directive format(.mat -> [col][row] - >numbers and no unnecessary token after it.
 *
 * @param line        Line starting at  the .mat payload.
 * @param out_result  [out pointer] Newly allocated int array (rows*cols) with extracted numbers.
 * @param out_count   [out pointer] size of the matrix (rows*cols).
 * @param asmContext  Assembler context (for error reporting).
 * @return true on success, false on format or allocation error.
 * @note the caller func own the out params and must be free.
 */
boolean parse_mat_directive(char *line, int **out_result, unsigned int *out_count, assembler_context *asmContext);


/**
* @brief Extract comma-separated numbers from a line into a new array.
*
* Extracting the numbers separated by commas ',' sign, and validate format(no double comma
* or unnecessary comma in the end.
* Format: (+/- optional sign)->number->comma ','->number->etc ...
*
* @param line        line pointing after .data\.mat. token.
* @param out_count   [out pointer] Number of found numbers (0 if none).
* @param out_result  [out pointer] Newly allocated int array with found numbers.
* @param asmContext  Assembler context (for error reporting).
* @return true on success (including 0 numbers), false on format/alloc error.
* @note the caller func own the out params and must be free.
 */

boolean extract_directive_numbers(const char *line, unsigned int *out_count, int **out_result, assembler_context *asmContext);


/**
 * @brief Extract the quoted string of a .string directive.
 *
 * Accepts only alphanumeric chars/spaces between quotes, requires open & close quotes
 * validate format (no double quotes,missing quotes.
 * Returns a newly allocated copy of found string.
 *
 * @param line        Line pointing after .string token.
 * @param asmContext  Assembler context (for error reporting).
 * @return Newly allocated C-string on success, NULL on error.
 */
char *extract_directive_string(char * line, assembler_context *asmContext);


/**
 * @brief Check whether a token is any directive (.data/.string/.mat/.entry/.extern).
 *
 * @param name        Candidate token.
 * @param asmContext  Assembler context with directive tables.
 * @return true if @p name matches a known directive, false otherwise.
 */
boolean is_directive(const char *name, assembler_context *asmContext);



/**
 * @brief Check whether a token is a data directive (.data/.string/.mat).
 *
 * @param name        Candidate token.
 * @param asmContext  Assembler context with data-directives table.
 * @return true if matches a data directive, false otherwise.
 */
boolean is_data_directive(const char *name, assembler_context *asmContext);

/**
 * @brief Check whether a token is an attribute directive (.entry/.extern).
 *
 * @param name        Candidate token.
 * @param asmContext  Assembler context with attribute-directives table.
 * @return true if matches an attribute directive, false otherwise.
 */
boolean is_attribute_directive(const char *name, assembler_context *asmContext);

/**
 * @brief Check if a token is ".entry".
 *
 * @param name        Candidate token.
 * @param asmContext  Assembler context with attribute-directives table.
 * @return true if ".entry", false otherwise.
 */
boolean is_entry_directive(const char * name, assembler_context *asmContext);

/**
 * @brief Check if a token is ".extern".
 *
 * @param name        Candidate token.
 * @param asmContext  Assembler context with attribute-directives table.
 * @return true if ".extern", false otherwise.
 */
boolean is_extern_directive(const char * name, assembler_context *asmContext);

/**
 * @brief Parse an .extern line and return the external label name.
 *
 * Validates directive token exist, the label name (rules + uniqueness),
 * and that no extra tokens exist. Returns a newly allocated copy of the name.
 *
 * @param line        line before .extern token.
 * @param asmContext  Assembler context (for validation and errors).
 * @return Newly allocated label string on success, NULL on error.
 */
char* get_extern_label(char* line, assembler_context *asmContext);

/**
 * @brief Parse an .entry line and return the entry label name.
 *
 * Validates directive token, the label name (rules), and ensures that
 * no unnecessary token after th entry label name.
 * Returns a newly allocated copy of the extern label name, or NULL on error.
 *
 * @param line        Line before .entry token.
 * @param asmContext  Assembler context (for validation and errors).
 * @return Newly allocated copy of the entry label name, or NULL on error.
 */
char* get_entry_label(char* line, assembler_context *asmContext);



#endif
