/**
 * @file context.h
 * @brief Defines the central assembler context structure.
 *
 * The `assembler_context` structure acts as a container for all
 * state information required during the assembly process.
 * It centralizes file handling, memory management, error flags,
 * symbol/macro tables, and linked lists used across the assembler passes.
 *
 * This context is initialized at the start of processing each source file
 * and is passed between functions to maintain consistent assembler state.
 *
 * Main responsibilities:
 *  - Hold input/output file names and current line counters.
 *  - Track memory usage, instruction counter (IC), and data counter (DC).
 *  - Store global and stage-specific error flags.
 *  - Manage all dynamic data structures: labels, macros, memory images,
 *    relocation requests, externals, and line mapping.
 *  - Provide access to static tables (opcodes, directives, registers).
 *
 * @author Ivgeny Tokarzhevsky
 * @date 25/08/2025
 */

#ifndef CONTEXT_H
#define CONTEXT_H

#include <config.h>

#include "typedef.h"
#include "boolean.h"

/**
 * @struct assembler_context
 * @brief Centralized state of the assembler during a single assembly process.
 *
 * Each source file being assembled gets its own `assembler_context` instance.
 * This structure stores file metadata, counters, memory usage, error status,
 * and all dynamically allocated linked lists and lookup tables.
 */
typedef struct assembler_context {

    /* ---------- File names ---------- */
    char* as_file_name;   /**< Input assembly source file (.as). */
    char* am_file_name;   /**< Preprocessed assembly file (.am). */
    char* ent_file_name;  /**< Entry symbols file (.ent). */
    char* ext_file_name;  /**< Externals usage file (.ext). */
    char* obj_file_name;  /**< Object file (.ob). */

    char* file_path; /**< file directory. */

    char* am_full_file_name; /**< full .am file path (name and directory). */
    char* as_full_file_name; /**< full .as file path (name and directory). */

    /* ---------- File line tracking ---------- */
    int as_file_line;         /**< Current line number in the .as file. */
    int am_file_line;         /**< Current line number in the .am file. */
    int second_pass_error_line; /**< Line number of error detected in second pass. */

    /* ---------- Program counters ---------- */
    unsigned int DC;          /**< Data counter for .data/.string/.mat directives. */
    unsigned int IC;          /**< Instruction counter for instructions. */

    /* ---------- Memory usage ---------- */
    unsigned int memory_usage; /**< Tracks total memory words allocated. */

    /* ---------- Error flags ---------- */
    boolean preproc_error; /**< Set if preprocessing stage failed. */
    boolean first_pass_error;  /**< Set if first pass failed. */
    boolean second_pass_error; /**< Set if second pass failed. */
    boolean global_error;      /**< Aggregated global error flag. */

    /* ---------- Dynamic linked lists ---------- */
    data_ptr data_memory;                  /**< Linked list for data memory image. */
    instruction_ptr instruction_memory;    /**< Linked list for instruction memory image. */
    external_ptr external_labels;          /**< Linked list of external labels usage. */
    label_ptr labels;                      /**< Linked list of defined labels. */
    macro_ptr macros;                      /**< Linked list of defined macros. */
    address_update_request_ptr address_update_requests; /**< Linked list of relocation requests. */
    lines_map_ptr lines_maper;             /**< Line mapping table (.as ↔ .am). */

    /* ---------- constant tables ---------- */
   const char** data_directive_table;       /**< Table of .data/.string/.mat directives. */
   const char** attributes_directive_table; /**< Table of .entry/.extern directives. */
   const char** registers;                  /**< Table of valid register names. */
   const char** macro_declaration_table;    /**< Table of preprocessor declarations (mcro/mcroend). */
   const_opcode_ptr opcode_table;           /**< Opcode definitions table. */



} assembler_context;


#endif
