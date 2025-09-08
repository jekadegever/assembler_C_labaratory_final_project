


#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "labels.h"



/**
 * @brief Executes the assembler workflow on one or more source files.
 *
 * This function orchestrates the entire assembly process:
 *  - Validates input file names and lengths.
 *  - Runs the preprocessor stage (.am file generation).
 *  - Performs the first pass (finding labels list, data memory insertion and first encoding stage).
 *  - Performs the second pass (entry labels, memory relocation and final encoding).
 *  - Generates output files (.obj, .ext, .ent) or removes them if not needed.
 *  - Frees all allocated memory and resources per file.
 *
 * @param argc  Number of command-line arguments (from main).
 * @param argv  Array of command-line arguments (from main).
 *
 * @return true  if the assembler finished processing  successfully.
 * @return false if no source file was provided or system/internal error found
 *
 * @note This function is the main entry point of the assembler logic,
 *       designed to be invoked from `main()`.
 */
boolean run_assembler(int argc, char *argv[]);



/**
 * @brief Resets the assembler context to its initial state.
 *
 * Sets all file names, counters, error flags, and list pointers in the
 * given context to NULL or 0. This ensures a clean state before
 * processing a new source file.
 *
 * @param asmContext Pointer to the assembler_context to initialize.
 * @return true on success, false if context pointer is NULL.
 */
int init_assembler(assembler_context* asmContext);

#endif
