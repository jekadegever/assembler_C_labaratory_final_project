
#ifndef FIRST_PASS_H
#define FIRST_PASS_H
#include "context.h"
#include "boolean.h"




 /**
  * @brief Executes the first pass of the assembler.
  *
  * Reads the `.am` source file line by line, identifies each line type
  * (instruction, data directive, extern, entry, or comment), and processes
  * it accordingly:
  *  - **Instruction line**: validates and encodes the instruction, stores it
  *    in instruction memory, and adds any label to the label table.
  *  - **Data directive line**: parses and stores data into data memory, and
  *    adds any label to the label table.
  *  - **Extern directive line**: validates extern labels and records them
  *    in the label table.
  *  - **Entry directive line**: skipped during first pass (handled in second pass).
  *  - **Invalid or unknown line**: reports an error and continues.
  *
  * Errors such as invalid syntax, label redefinition, or memory overflows
  * are flagged, but processing continues to allow multiple error reports.
  *
  * @param asmContext Pointer to the assembler context, containing state,
  *        counters (IC/DC), and memory structures to update.
  *
  * @return `true` if the first pass completed without errors,
  *         `false` if any errors were encountered.
  */
boolean execute_first_pass(assembler_context *asmContext);


#endif
