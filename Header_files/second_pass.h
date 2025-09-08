
#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "boolean.h"
#include "context.h"


/**
 * @brief Executes the second pass of the assembler.
 *
 * This function performs the assembler's second pass over the preprocessed `.am` file.
 * Its responsibilities include:
 *   - Reading each line of the `.am` file.
 *   - Ignoring label declarations (already handled in the first pass).
 *   - Handling `.entry` directives:
 *       - Validating that the referenced label exists and is not external.
 *       - Marking the label as an entry if valid.
 *   - Performing memory relocation:
 *       - Adjusting instruction memory addresses by applying a fixed memory offset.
 *       - Relocating data memory to follow instruction memory.
 *       - Updating all label addresses after relocation.
 *   - Processing pending address update requests:
 *       - Replacing placeholder addresses in instruction memory
 *         with the final resolved addresses of labels.
 *
 * If errors occur (e.g., undeclared entry labels, invalid extern-entry conflict,
 * relocation failures, or unresolved address update requests),
 * they are reported and the assembler context is updated accordingly.
 *
 * @param asmContext Pointer to the assembler context containing file names,
 *                   labels, memory structures, and error state.
 * @return `true` if the second pass completes successfully without errors,
 *         `false` otherwise.
 */
boolean execute_second_pass(assembler_context *asmContext);


#endif
