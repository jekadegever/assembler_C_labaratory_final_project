
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "lines_map.h"
#include "context.h"


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

/* ---------------- External Error Codes Table ---------------- */
error external_errors[] = {
    { FIRST_PASS,   { ERROR_CODE_100 }, "opcode name not found." },
    { FIRST_PASS,   { ERROR_CODE_101 }, "Invalid opcode name." },
    { FIRST_PASS,   { ERROR_CODE_102 }, "Opcode received invalid number of operands." },
    { FIRST_PASS,   { ERROR_CODE_103 }, "Out of memory: program reached to the max available memory." },
    { FIRST_PASS,   { ERROR_CODE_104 }, "Encoding process failed." },
    { FIRST_PASS,   { ERROR_CODE_105 }, "No operands found for used opcode." },
    { FIRST_PASS,   { ERROR_CODE_106 }, "Found more operands than required for used opcode." },
    { FIRST_PASS,   { ERROR_CODE_107 }, "Operands format error." },
    { FIRST_PASS,   { ERROR_CODE_108 }, "Operand parsing failed." },
    { FIRST_PASS,   { ERROR_CODE_109 }, "Invalid matrix operand format, unexpected token after [][] brackets." },
    { FIRST_PASS,   { ERROR_CODE_110 }, "Matrix column index is not a register number." },
    { FIRST_PASS,   { ERROR_CODE_111 }, "Matrix row index is not a register number." },
    { FIRST_PASS,   { ERROR_CODE_112 }, "Invalid matrix label name." },
    { FIRST_PASS,   { ERROR_CODE_113 }, "Invalid source operand type." },
    { FIRST_PASS,   { ERROR_CODE_114 }, "Invalid destination operand." },
    { FIRST_PASS,   { ERROR_CODE_115 }, "Invalid source operand type for used opcode." },
    { FIRST_PASS,   { ERROR_CODE_116 }, "Invalid destination operand type for used opcode." },
    { FIRST_PASS,   { ERROR_CODE_117 }, "Not enough operands for used opcode." },
    { FIRST_PASS,   { ERROR_CODE_118 }, "Format error: unexpected comma sign ',' near the operands." },
    { FIRST_PASS,   { ERROR_CODE_119 }, "Format error: missing comma sign ',' between operands." },
    { FIRST_PASS,   { ERROR_CODE_120 }, "Numeric operand has too many digits." },
    { FIRST_PASS,   { ERROR_CODE_121 }, "Line exceeds the maximum allowed length of " TOSTRING(MAX_LINE_LEN) " characters." },
    { FIRST_PASS,   { ERROR_CODE_122 }, "Directive name not found." },
    { FIRST_PASS,   { ERROR_CODE_123 }, "The provided number is greater than the maximum storable value in memory." },
    { FIRST_PASS,   { ERROR_CODE_124 }, "String declaration error: expected opening quotes (\") at the beginning of string." },
    { FIRST_PASS,   { ERROR_CODE_125 }, "String declaration error: illegal char found in the string." },
    { FIRST_PASS,   { ERROR_CODE_126 }, "String declaration error: expected closing quotes (\") at the end of string." },
    { FIRST_PASS,   { ERROR_CODE_127 }, "String declaration error: unexpected token found after string declaration." },
    { FIRST_PASS,   { ERROR_CODE_128 }, "String declaration error: string not found." },
    { FIRST_PASS,   { ERROR_CODE_129 }, "Data declaration error: no numeric value provided." },
    { FIRST_PASS,   { ERROR_CODE_130 }, "Data declaration error: no numeric value provided after + or - sign." },
    { FIRST_PASS,   { ERROR_CODE_131 }, "Data declaration error: unexpected token while expecting for numeric value." },
    { FIRST_PASS,   { ERROR_CODE_132 }, "Data declaration error: missing comma sign ',' after number." },
    { FIRST_PASS,   { ERROR_CODE_133 }, "Data declaration error: unnecessary comma sign ',' at end of line." },
    { FIRST_PASS,   { ERROR_CODE_134 }, "Data declaration error: matrix size definition is missing, format: [rows][cols]." },
    { FIRST_PASS,   { ERROR_CODE_135 }, "Data declaration error: invalid matrix size definition, expected format [rows][cols]." },
    { FIRST_PASS,   { ERROR_CODE_136 }, "Data declaration error: matrix size row and column cannot be zero." },
    { FIRST_PASS,   { ERROR_CODE_137 }, "Data declaration error: too many values provided for the declared matrix dimensions." },
    { FIRST_PASS,   { ERROR_CODE_138 }, "Entry directive error: label not found." },
    { FIRST_PASS,   { ERROR_CODE_139 }, "Entry directive error: illegal label name." },
    { FIRST_PASS,   { ERROR_CODE_140 }, "Entry directive error: unexpected token after label name." },
    { SECOND_PASS,  { ERROR_CODE_141 }, "Entry directive error: can't define the label as entry, label doesn't exist." },
    { SECOND_PASS,  { ERROR_CODE_142 }, "Entry directive error: can't define external label as entry." },
    { FIRST_PASS,   { ERROR_CODE_143 }, "The immediate operand value exceeds the allowed bit-field size." },
    { FIRST_PASS,   { ERROR_CODE_144 }, "Can't open the file: file does not exist or is in use." },
    { FIRST_PASS,   { ERROR_CODE_145 }, "Invalid label name." },
    { SECOND_PASS,  { ERROR_CODE_146 }, "Attempted to use an undeclared label." },
    { PRE_PROCESSOR,{ ERROR_CODE_147 }, "Macro declaration error: name already in use." },
    { PRE_PROCESSOR,{ ERROR_CODE_148 }, "Macro declaration error: invalid name provided." },
    { PRE_PROCESSOR,{ ERROR_CODE_149 }, "Macro declaration error: unexpected token found after macro name." },
    { PRE_PROCESSOR,{ ERROR_CODE_150 }, "Macro declaration error: unexpected token found after macroend directive." },
    { PRE_PROCESSOR,{ ERROR_CODE_151 }, "Macro declaration error: macro content is missing." },
    { PRE_PROCESSOR,{ ERROR_CODE_152 }, "Macro declaration error: macroend is missing." },
    { PRE_PROCESSOR,{ ERROR_CODE_153 }, "Error in macro invocation: unexpected token after macro name while expanding a defined macro." },
    { PRE_PROCESSOR,{ ERROR_CODE_154 }, "Invalid file name provided." },
    { PRE_PROCESSOR,{ ERROR_CODE_155 }, "File name length is not acceptable." },
    { FIRST_PASS,   { ERROR_CODE_156 }, "Unknown line type." },
    { FIRST_PASS,   { ERROR_CODE_157 }, "Extern directive error: external label name not found." },
    { FIRST_PASS,   { ERROR_CODE_158 }, "Extern directive error: unexpected token after label name." },
    { FIRST_PASS,   { ERROR_CODE_159 }, "Extern directive error: invalid label name." },
    { FIRST_PASS,   { ERROR_CODE_160 }, "Extern directive error: label already exists." },
    { PRE_PROCESSOR,{ ERROR_CODE_161 }, "Can't open file: file does not exist." },
    { PRE_PROCESSOR,{ ERROR_CODE_162 }, "Can't open file: file is in use, close it and try again." },
    { PRE_PROCESSOR,{ ERROR_CODE_163 }, "Can't open file: permission denied." },
    { FIRST_PASS,   { ERROR_CODE_164 }, "Invalid label name, can't define a label with already used name." },
    { FIRST_PASS,   { ERROR_CODE_165 }, "Unknown directive name." },
    { FIRST_PASS,   { ERROR_CODE_166 }, "Unknown type of operation." },
    { PRE_PROCESSOR,{ ERROR_CODE_167 }, "Unknown file type, the provided file is not assembly source file (.as)." },
    { FIRST_PASS,   { ERROR_CODE_168 }, "Unexpected comma sign before the first operand." },
    { FIRST_PASS,   { ERROR_CODE_169 }, "Unexpected comma sign after the last operand." },
    { FIRST_PASS,   { ERROR_CODE_170 }, "Unexpected comma sign between the operands." },
    { FIRST_PASS,   { ERROR_CODE_171 }, "Number missing after # sign." },
    { FIRST_PASS,   { ERROR_CODE_172 }, "Unexpected char found at immediate operand, only numeric values are allowed." },
    { FIRST_PASS,   { ERROR_CODE_173 }, "Data declaration error: floating point numbers are not allowed." },
    { FIRST_PASS,   { ERROR_CODE_174 }, "Data declaration error: characters are not allowed, only numeric values." },
    { FIRST_PASS,   { ERROR_CODE_175 }, "Data declaration error: unexpected comma sign before a number." },
    { PRE_PROCESSOR,{ ERROR_CODE_176 }, "Macro declaration error: macro name not found." },
    { PRE_PROCESSOR,{ ERROR_CODE_177 }, "mcroend command found without valid macro declaration before." },
    { FIRST_PASS,   { ERROR_CODE_178 }, "floating point are not allowed at immediate operand" }
};

/* ---------------- Internal Error Codes Table ---------------- */
error internal_errors[] = {
    { FIRST_PASS, { ERROR_CODE_25 }, "Function called with NULL argument." },
    { FIRST_PASS, { ERROR_CODE_26 }, "No handling function for this amount of operands." },
    { FIRST_PASS, { ERROR_CODE_27 }, "No encoding operation for given operand type." },
    { FIRST_PASS, { ERROR_CODE_28 }, "Can't relocate while operand is not a label." },
    { FIRST_PASS, { ERROR_CODE_29 }, "Relocation error: target instruction address for update not found in instruction memory." },
    { FIRST_PASS, { ERROR_CODE_30 }, "Operand not provided for address update request." }
};

/* ---------------- System Error Codes Table ---------------- */
error system_errors[] = {
    { FIRST_PASS, { ERROR_CODE_1 },  "Memory allocation failed" },
    { FIRST_PASS, { ERROR_CODE_2 },  "Encoding error: opcode value exceeds the allowed bit-field size." },
    { FIRST_PASS, { ERROR_CODE_3 },  "Encoding error: source addressing mode value exceeds the allowed bit-field size." },
    { FIRST_PASS, { ERROR_CODE_4 },  "Encoding error: destination addressing mode value exceeds the allowed bit-field size." },
    { FIRST_PASS, { ERROR_CODE_5 },  "Encoding error: opcode ERA field value exceeds the allowed bit-field size." },
    { FIRST_PASS, { ERROR_CODE_6 },  "Encoding error: destination register number exceeds the allowed bit-field size." },
    { FIRST_PASS, { ERROR_CODE_7 },  "Encoding error: source register number exceeds the allowed bit-field size." },
    { FIRST_PASS, { ERROR_CODE_8 },  "Encoding error: matrix register index number exceeds the allowed bit-field size." },
    { FIRST_PASS, { ERROR_CODE_9 },  "Encoding error: operand ERA field value exceeds the allowed bit-field size." },
    { FIRST_PASS, { ERROR_CODE_10 }, "Encoding error: label address value exceeds the allowed bit-field size." },
    { FIRST_PASS, { ERROR_CODE_11 }, "Memory reallocation failed" }
};

/* ---------------- Error Printing Functions ---------------- */
void print_internal_error(internal_error_code code, char* func_name) {
    printf("\nINTERNAL ERROR: %s in function: %s.\n\nProgram stopped ! ", internal_errors[code].description, func_name);
    exit(1);
}

void print_system_error(system_error_code code) {
    printf("\nSYSTEM ERROR: %s \n\nProgram stopped !\n\n", system_errors[code].description);
    exit(1);
}

void print_external_error(external_error_code code, const assembler_context *asmContext) {
    char* file_name = asmContext->as_file_name;
    unsigned int line = get_origin_file_line(asmContext->am_file_line, asmContext->lines_maper);

    if (external_errors[code].position == SECOND_PASS) {
        printf("\n%s::%d: ERROR: %s \n\n", file_name,
               get_origin_file_line(asmContext->second_pass_error_line, asmContext->lines_maper),
               external_errors[code].description);
    }
    else if (external_errors[code].position == PRE_PROCESSOR) {
        printf("\n%s: ERROR: %s \n\n", file_name, external_errors[code].description);
    }
    else {
        printf("\n%s::%d: ERROR: %s \n\n", file_name, line, external_errors[code].description);
    }
}
