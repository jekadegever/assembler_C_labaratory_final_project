
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "lines_map.h"
#include "context.h"
#include "sys_memory.h"


/**
 * @file errors.h
 *
 * @brief Error handling definitions for the assembler.
 *
 * This module centralizes all error-related definitions and structures
 * used by the assembler. It defines error categories (system, internal,
 * external), the stages in which errors can occur (preprocessor, first pass,
 * second pass, file operations), and the data structures that represent
 * error codes and messages.
 *
 * Errors are categorized into three main types:
 *  - **System errors**: Issues caused by the environment (e.g., memory allocation failure).
 *  - **Internal errors**: Bugs or invalid states inside the assembler itself.
 *  - **External errors**: User-facing errors in the input assembly source file.
 *
 * Each error is mapped to a human-readable description to provide
 * meaningful feedback to the user or developer.
 *
 * This header is included in most assembler modules that need to
 * report or classify errors.
 *
 * @author Ivgeny Tokarzhevsky
 * @date 01/09/2025
 */


/*assembler context pointer*/
static assembler_context* asmContext = NULL;

/* ---------------- External Error Codes Table ---------------- */
error external_errors[] = {
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_100 }, "opcode name not found." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_101 }, "Invalid opcode name." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_102 }, "Opcode received invalid number of operands." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_103 }, "Out of memory: program reached to the max available memory." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_104 }, "Encoding process failed." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_105 }, "No operands found for used opcode." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_106 }, "Found more operands than required for used opcode." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_107 }, "Operands format error." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_108 }, "Operand parsing failed." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_109 }, "Invalid matrix operand format, unexpected token after [][] brackets." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_110 }, "Matrix column index is not a register number." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_111 }, "Matrix row index is not a register number." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_112 }, "Invalid matrix label name." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_113 }, "Invalid source operand type." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_114 }, "Invalid destination operand." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_115 }, "Invalid source operand type for used opcode." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_116 }, "Invalid destination operand type for used opcode." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_117 }, "Not enough operands for used opcode." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_118 }, "Format error: unexpected comma sign ',' near the operands." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_119 }, "Format error: missing comma sign ',' between operands." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_120 }, "Numeric operand has too many digits." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_121 }, "Line exceeds the maximum allowed length of " TOSTRING(MAX_LINE_LEN) " characters." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_122 }, "Directive name not found." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_123 }, "The provided number is greater than the maximum storable value in memory." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_124 }, "String declaration error: expected opening quotes (\") at the beginning of string." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_125 }, "String declaration error: illegal char found in the string." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_126 }, "String declaration error: expected closing quotes (\") at the end of string." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_127 }, "String declaration error: unexpected token found after string declaration." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_128 }, "String declaration error: string not found." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_129 }, "Data declaration error: no numeric value provided." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_130 }, "Data declaration error: no numeric value provided after + or - sign." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_131 }, "Data declaration error: unexpected token while expecting for numeric value." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_132 }, "Data declaration error: missing comma sign ',' after number." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_133 }, "Data declaration error: unnecessary comma sign ',' at end of line." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_134 }, "Data declaration error: matrix size definition is missing, format: [rows][cols]." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_135 }, "Data declaration error: invalid matrix size definition, expected format [rows][cols]." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_136 }, "Data declaration error: matrix size row and column cannot be zero." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_137 }, "Data declaration error: too many values provided for the declared matrix dimensions." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_138 }, "Entry directive error: label not found." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_139 }, "Entry directive error: illegal label name." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_140 }, "Entry directive error: unexpected token after label name." },
    { SECOND_PASS_SET_LINE_AND_FILE_NAME,  { ERROR_CODE_141 }, "Entry directive error: can't define the label as entry, label doesn't exist." },
    { SECOND_PASS_SET_LINE_AND_FILE_NAME,  { ERROR_CODE_142 }, "Entry directive error: can't define external label as entry." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_143 }, "The immediate operand value exceeds the allowed bit-field size." },
    { FILE_NAME_NO_LINE,   { ERROR_CODE_144 }, "Can't open the file: file does not exist or is in use." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_145 }, "Invalid label name." },
    { SECOND_PASS_SET_LINE_AND_FILE_NAME,  { ERROR_CODE_146 }, "Attempted to use an undeclared label." },
    { AS_FILE_LINE_AND_FILE_NAME,{ ERROR_CODE_147 }, "Macro declaration error: name already in use." },
    { AS_FILE_LINE_AND_FILE_NAME,{ ERROR_CODE_148 }, "Macro declaration error: invalid name provided." },
    { AS_FILE_LINE_AND_FILE_NAME,{ ERROR_CODE_149 }, "Macro declaration error: unexpected token found after macro name." },
    { AS_FILE_LINE_AND_FILE_NAME,{ ERROR_CODE_150 }, "Macro declaration error: unexpected token found after macroend directive." },
    { AS_FILE_LINE_AND_FILE_NAME,{ ERROR_CODE_151 }, "Macro declaration error: macro content is missing." },
    { AS_FILE_LINE_AND_FILE_NAME,{ ERROR_CODE_152 }, "Macro declaration error: macroend is missing." },
    { AS_FILE_LINE_AND_FILE_NAME,{ ERROR_CODE_153 }, "Error in macro invocation: unexpected token after macro name while expanding a defined macro." },
    { FILE_NAME_NO_LINE,{ ERROR_CODE_154 }, "Invalid file name provided." },
    { FILE_NAME_NO_LINE,{ ERROR_CODE_155 }, "File name length is not acceptable." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_156 }, "Unknown line type." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_157 }, "Extern directive error: external label name not found." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_158 }, "Extern directive error: unexpected token after label name." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_159 }, "Extern directive error: invalid label name." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_160 }, "Extern directive error: label already exists." },
    { FILE_NAME_NO_LINE,{ ERROR_CODE_161 }, "Can't open file: file does not exist." },
    { FILE_NAME_NO_LINE,{ ERROR_CODE_162 }, "Can't open file: file is in use, close it and try again." },
    { FILE_NAME_NO_LINE,{ ERROR_CODE_163 }, "Can't open file: permission denied." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_164 }, "Invalid label name, can't define a label with already used name." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_165 }, "Unknown directive name." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_166 }, "Unknown type of operation." },
    { FILE_NAME_NO_LINE,{ ERROR_CODE_167 }, "Unknown file type, the provided file is not assembly source file (.as)." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_168 }, "Unexpected comma sign before the first operand." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_169 }, "Unexpected comma sign after the last operand." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_170 }, "Unexpected comma sign between the operands." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_171 }, "Number missing after # sign." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_172 }, "Unexpected char found at immediate operand, only numeric values are allowed." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_173 }, "Data declaration error: floating point numbers are not allowed." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_174 }, "Data declaration error: characters are not allowed, only numeric values." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_175 }, "Data declaration error: unexpected comma sign before a number." },
    { AS_FILE_LINE_AND_FILE_NAME,{ ERROR_CODE_176 }, "Macro declaration error: macro name not found." },
    { AS_FILE_LINE_AND_FILE_NAME,{ ERROR_CODE_177 }, "mcroend command found without valid macro declaration before." },
    { AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_178 }, "floating point are not allowed at immediate operand" },
    { NO_FILE_NAME_NO_LINE,{ ERROR_CODE_179 }, "Input file is missing." },
{ AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_180 }, "can't define label on empty line;" },
{ AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_181 }, "Data declaration error: unnecessary comma sign ',' before numbers." },
{ AM_FILE_LINE_AND_FILE_NAME,   { ERROR_CODE_182 }, "Data declaration error: number missing after comma sign ','." },

};

/* ---------------- Internal Error Codes Table ---------------- */
error internal_errors[] = {
    { AM_FILE_LINE_AND_FILE_NAME, { ERROR_CODE_25 }, "Function called with NULL argument." },
    { AM_FILE_LINE_AND_FILE_NAME, { ERROR_CODE_26 }, "No handling function for this amount of operands." },
    { AM_FILE_LINE_AND_FILE_NAME, { ERROR_CODE_27 }, "No encoding operation for given operand type." },
    { AM_FILE_LINE_AND_FILE_NAME, { ERROR_CODE_28 }, "Can't relocate while operand is not a label." },
    { AM_FILE_LINE_AND_FILE_NAME, { ERROR_CODE_29 }, "Relocation error: target instruction address for update not found in instruction memory." },
    { AM_FILE_LINE_AND_FILE_NAME, { ERROR_CODE_30 }, "Operand not provided for address update request." }
};

/* ---------------- System Error Codes Table ---------------- */
error system_errors[] = {
    { AM_FILE_LINE_AND_FILE_NAME, { ERROR_CODE_1 },  "Memory allocation failed" },
    { AM_FILE_LINE_AND_FILE_NAME, { ERROR_CODE_2 },  "Encoding error: opcode value exceeds the allowed bit-field size." },
    { AM_FILE_LINE_AND_FILE_NAME, { ERROR_CODE_3 },  "Encoding error: source addressing mode value exceeds the allowed bit-field size." },
    { AM_FILE_LINE_AND_FILE_NAME, { ERROR_CODE_4 },  "Encoding error: destination addressing mode value exceeds the allowed bit-field size." },
    { AM_FILE_LINE_AND_FILE_NAME, { ERROR_CODE_5 },  "Encoding error: opcode ERA field value exceeds the allowed bit-field size." },
    { AM_FILE_LINE_AND_FILE_NAME, { ERROR_CODE_6 },  "Encoding error: destination register number exceeds the allowed bit-field size." },
    { AM_FILE_LINE_AND_FILE_NAME, { ERROR_CODE_7 },  "Encoding error: source register number exceeds the allowed bit-field size." },
    { AM_FILE_LINE_AND_FILE_NAME, { ERROR_CODE_8 },  "Encoding error: matrix register index number exceeds the allowed bit-field size." },
    { AM_FILE_LINE_AND_FILE_NAME, { ERROR_CODE_9 },  "Encoding error: operand ERA field value exceeds the allowed bit-field size." },
    { AM_FILE_LINE_AND_FILE_NAME, { ERROR_CODE_10 }, "Encoding error: label address value exceeds the allowed bit-field size." },
    { AM_FILE_LINE_AND_FILE_NAME, { ERROR_CODE_11 }, "Memory reallocation failed" }
};

/* ---------------- Error Printing Functions ---------------- */

void print_internal_error(internal_error_code code, char* func_name) {
    printf("\nINTERNAL ERROR: %s in function: %s.", internal_errors[code].description, func_name);

}

void print_system_error(system_error_code code) {
    printf("\nSYSTEM ERROR: %s \n\nProgram stopped !\n\n", system_errors[code].description);
    free_all_memory(asmContext);
    exit(1);
}

void print_external_error(external_error_code code) {
    char* file_name = asmContext->as_file_name;
    unsigned int line = get_origin_file_line(asmContext->am_file_line, asmContext->lines_maper);

    if (external_errors[code].params == SECOND_PASS_SET_LINE_AND_FILE_NAME) {
        printf("\n%s::%d: ERROR: %s \n\n", file_name,
               get_origin_file_line(asmContext->second_pass_error_line, asmContext->lines_maper),
               external_errors[code].description);
    }
    else if (external_errors[code].params == AS_FILE_LINE_AND_FILE_NAME) {
        printf("\n%s::%d: ERROR: %s \n\n", file_name, asmContext->as_file_line,external_errors[code].description);
    }
    else if (external_errors[code].params == NO_FILE_NAME_NO_LINE) {
        printf("\n ERROR: %s \n\n",  external_errors[code].description);
    }
    else if (external_errors[code].params == FILE_NAME_NO_LINE) {
        printf("\n%s: ERROR: %s \n\n", file_name,external_errors[code].description);
    }
    else {
        printf("\n%s::%d: ERROR: %s \n\n", file_name, line, external_errors[code].description);
    }
}

void set_error_context(assembler_context* Context) {
    asmContext = Context;
}
