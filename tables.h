
#ifndef TABLES_H
#define TABLES_H
#include "instructions.h"


/**
 * @brief Retrieves the global opcode table.
 *
 * The opcode table defines all supported instructions of the assembler,
 * including their opcode value, name, number of operands, valid addressing
 * modes for source and destination, and default encoding type.
 *
 * @return Pointer to the static opcode table array.
 */
const opcode *get_opcode_table(void);

/**
 * @brief Retrieves the list of available registers.
 *
 * The register table contains all valid register names supported by the assembler.
 *
 * @return Pointer to an array of register name strings (e.g., "r0", "r1", ...).
 */
const char **get_registers(void);

/**
 * @brief Retrieves the list of supported data directives.
 *
* Data directives  declaring data, strings, or matrix.
 *
 * @return Pointer to an array of data directive strings.
 */
const char **get_data_directives_table(void);

/**
 * @brief Retrieves the list of supported attribute directives.
 *
 * Attribute directives include commands such as `.entry` and `.extern`,
 *
 * @return Pointer to an array of attribute directive strings.
 */
const char **get_attributes_directives_table(void);

/**
 * @brief Retrieves the list of macro declaration keywords.
 *
 * These keywords are reserved by the preprocessor for defining macros.
 *
 * @return Pointer to an array of macro declaration keywords (e.g., "mcro", "mcroend").
 */
const char **get_macro_declaration_table(void);


#endif
