

#include "tables.h"


/**
 * @file tables.c
 * @brief Provides static lookup tables for assembler components.
 *
 * This module defines and exposes global tables used throughout the assembler:
 *   - Opcode table: Supported instructions with their operand rules and encodings.
 *   - Registers table: All valid register names.
 *   - Data directives table: Supported `.data`, `.string`, and `.mat` directives.
 *   - Attribute directives table: `.entry` and `.extern`.
 *   - Macro declaration keywords: Reserved words for macro handling.
 *
 * The functions in this file return pointers to the internal static tables,
 * ensuring consistency across the assembler while avoiding duplication.
 *
 * @author Ivgeny Tokarzhevsky
 * @date 01/09/2025
 */

static const opcode opcode_table[] ={
    {0,"mov",2,ALL,AM_1_2_3,ABSOLUTE},
    {1,"cmp",2,ALL,ALL,ABSOLUTE},
    {2,"add",2,ALL,AM_1_2_3,ABSOLUTE},
    {3,"sub",2,ALL,AM_1_2_3,ABSOLUTE},
    {4,"lea",2,AM_1_2,AM_1_2_3,ABSOLUTE},
    {5,"clr",1,NONE,AM_1_2_3,ABSOLUTE},
    {6,"not",1,NONE,AM_1_2_3,ABSOLUTE},
    {7,"inc",1,NONE,AM_1_2_3,ABSOLUTE},
    {8,"dec",1,NONE,AM_1_2_3,ABSOLUTE},
    {9,"jmp",1,NONE,AM_1_2_3,ABSOLUTE},
    {10,"bne",1,NONE,AM_1_2_3,ABSOLUTE},
    {11,"jsr",1,NONE,AM_1_2_3,ABSOLUTE},
    {12,"red",1,NONE,AM_1_2_3,ABSOLUTE},
    {13,"prn",1,NONE,ALL,ABSOLUTE},
    {14,"rts",0,NONE,NONE,ABSOLUTE},
    {15,"stop",0,NONE,NONE,ABSOLUTE}
};
static const char* registers[] = {"r0","r1","r2","r3","r4","r5","r6","r7"};

static const char *data_directives_table[] = {".data",".string",".mat"};

static const char *attributes_directives_table[] = {".entry",".extern"};

static const char* macro_declaration[] = {
    "mcro",
    "mcroend"
};

const opcode* get_opcode_table(void) {
    return opcode_table;
}

const char** get_registers(void) {
    return registers;
}

const char** get_data_directives_table(void) {
    return data_directives_table;
}

const char** get_attributes_directives_table(void) {
    return attributes_directives_table;
}

const char** get_macro_declaration_table(void) {
    return macro_declaration;
}
