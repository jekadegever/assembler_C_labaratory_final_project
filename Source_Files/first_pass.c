

#include "first_pass.h"
#include <stdio.h>
#include <string.h>
#include "sys_memory.h"
#include "directives.h"
#include "errors.h"
#include "files.h"
#include "instructions.h"
#include "labels.h"
#include "util.h"

/**
 * @file first_pass.c
 *
 * @brief Implements the first pass of the assembler.
 *
 * This module performs the first pass over the preprocessed assembly source (.am file).
 * It scans the file line by line, validates syntax, identifies labels, and populates
 * the assembler’s intermediate data structures.
 *
 * Responsibilities:
 *  - Validate line length and format.
 *  - Identify and classify each line as instruction, directive, extern, entry, or comment.
 *  - Parse instructions and encode them into instruction memory.
 *  - Parse data directives and insert values into data memory.
 *  - Collect and store label definitions in the symbol table.
 *  - Record extern declarations.
 *  - Flag errors for invalid syntax, memory overflows, or invalid label usage.
 *
 * The results of the first pass are stored in the assembler context, and are later
 * refined and resolved during the second pass.
 *
 * @author Ivgeny Tokarzhevsky
 * @date 01/09/2025
 */


boolean execute_first_pass(assembler_context *asmContext) {

    char* line = NULL;
    FILE* am_file;
    unsigned int IC;
    unsigned int DC;
    char* label = NULL;
    boolean directive_processed = false;
    boolean instruction_processed = false;
    line_type type;




    /*check that all input arguments exist*/
    if (!asmContext) {
        print_internal_error(ERROR_CODE_25,"execute_first_pass");
        return false;
    }



    /*allocate memory for reading line*/
    line = (char*)handle_malloc(sizeof(char) * MAX_LINE_LEN + 3);

    /*open the am file for reading*/
    if ((am_file = open_file(asmContext->am_full_file_name, READ)) == NULL) { safe_free((void**)&line); return false;}

    /*read each line till reach end of file*/
    while (fgets(line, MAX_LINE_LEN + 2, am_file)) {

        /*increment the line counter*/
        asmContext->am_file_line++;

        /*verify line length not exceeds max allowed length*/
        if (strlen(line) > MAX_LINE_LEN) {
            asmContext->first_pass_error = true;
            print_external_error(ERROR_CODE_121);
            continue;
        }

        /*clear white space and chars like '\n' */
        trim_edge_white_space(line);

        /*if the line is empty or comment line, ignore and continue to the  next line*/
        if (is_comment_or_empty_line(line)) { continue;}


        /*if a label exist, get the label name*/
        label = find_label_definition(line, asmContext);

        /*get the current DC and IC (for labels)*/
        IC= asmContext->IC;/*for code labels*/
        DC = asmContext->DC;/*for data labels*/

        /*get the line type (directive, instruction, extern etc..)*/
        type = get_line_type(line, asmContext);


        /* - - - - - - - - data directive / instruction lines handling - - - - - - - - - -*/

        /* Data directive line -> parse the line and add the data to data memory.
         * Attribute directive(Extern only) -> add the extern label to the labels list.
         * Instruction line -> parse the line, encode the opcode and operands and add to instruction memory.
         * Unknown line -> print an error and move to the next line.
         */

        switch (type) {

            case INSTRUCTION_LINE: {
                /*try parse the instruction line*/
                instruction_processed = handle_instruction_line(line, asmContext);
                if (!instruction_processed) {
                    asmContext->first_pass_error = true;
                }

                if (label && instruction_processed) {
                    /*If label exist and instruction line parsed successfully -> add the label to the labels list (as code)*/
                    if (!add_label(label,IC,CODE,NORMAL, &asmContext->labels)) {
                        asmContext->first_pass_error = true;
                    }
                }

                break;
            }

            case DATA_DIRECTIVE_LINE: {
                /*try parse the data directive line*/
                directive_processed = handle_data_directives_line(line, asmContext);
                if (!directive_processed) {
                    asmContext->first_pass_error = true;
                }

                if (label && directive_processed) {
                    /*If label exist and data directive line parsed successfully -> add the label to the labels list (as data)*/
                    if (!add_label(label,DC,DATA,NORMAL, &asmContext->labels)) {
                        asmContext->first_pass_error = true;
                    }
                }

                break;
            }

            case EXTERN_DIRECTIVE_LINE:{
                /*try parse extern directive line*/
                label = get_extern_label(line, asmContext);/*get the extern label*/
                if (!label) {
                    asmContext->first_pass_error = true;
                }
                else {
                    if (!add_label(label, EXTERNAL_TEMP_ADDR,UNKNOWN_ADDR_TYPE,EXTERN, &asmContext->labels)) {
                        asmContext->first_pass_error = true;

                    }
                }

                break;
            }


            case ENTRY_DIRECTIVE_LINE:{
                /*skip entry directive line (handled in second pass)*/
                break;

            }

            case EMPTY_LINE: {
                 /*skip this line after error printed*/

                /* This case is relevant only in the special case where an empty line
                 * appears immediately after a label declaration.
                 *
                 * For a regular empty line, the assembler should never
                 * enter this case instead, ite should detect the line
                 * as empty earlier and continue to the next line.*/
                    print_external_error(ERROR_CODE_180);
                    asmContext->first_pass_error = true;

                break;
            }


            default: {
                /*Unknown line type found*/
                if (*line == '.') {
                    /*error, trying to use unknown directive name*/
                    print_external_error(ERROR_CODE_165);
                }else {
                    /*error, trying to use unknown opcode name*/
                    print_external_error(ERROR_CODE_166);
                }
                asmContext->first_pass_error = true;


                break;
            }

        }


        safe_free((void**)&label);

    }
    /*clean-up allocated memory and close the used file*/
    fclose(am_file);
    safe_free((void**)&line);

    /*If no error found -> return true*/
    if (!asmContext->first_pass_error) {

        return true;
    }

    return false;
}
