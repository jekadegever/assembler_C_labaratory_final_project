

#include "second_pass.h"
#include <stdlib.h>
#include "boolean.h"
#include "directives.h"
#include "errors.h"
#include "files.h"
#include "labels.h"
#include "addresses.h"
#include "util.h"
#include "sys_memory.h"


/**
 * @file second_pass.c
 * @brief Implementation of the assembler's second pass.
 *
 * The second pass is responsible for finalizing the assembly process:
 *   - Processes `.entry` directives, validating and marking entry labels.
 *   - Performs memory relocation for both instructions and data.
 *   - Updates label addresses after relocation.
 *   - Applies pending address update requests to instruction memory,
 *     resolving label references to final encoded addresses.
 *
 * This stage assumes that syntax and structure validation were already performed
 * in the first pass. The goal of the second pass is to produce a fully resolved
 * instruction and data memory layout that can be written to output files.
 *
 * @author Ivgeny Tokarzhevsky
 * @@date 01/09/2025
 */


boolean execute_second_pass(assembler_context *asmContext) {

    FILE* am_file;
    char* line = NULL;
    char* entry_label = NULL;
    line_type type;


    /*check that all input arguments exist*/
    if (!asmContext) {
        print_internal_error(ERROR_CODE_25, "execute_second_pass");
        return false;
    }
    /*reset line counter*/
    asmContext->am_file_line = 0;


    /*allocate memory for reading line*/
    line = (char*)handle_malloc(sizeof(char) * MAX_LINE_LEN + 3);

    if (!line) {return false;}

    /*open the am file for reading*/
    if ((am_file = open_file(asmContext->am_full_file_name, READ)) == NULL) {safe_free((void**)&line); return false;}
    
    /*read each line till reach end of file*/
    while (fgets(line, MAX_LINE_LEN + 2, am_file)) {

        /*increment the line counter*/
        asmContext->am_file_line++;

        /*clear white space and chars like '\n' */
        trim_edge_white_space(line);

        /*if the line is empty or comment line, ignore, continue to next line*/
        if (is_comment_or_empty_line(line)) { continue;}


        /*assembler is ignoring labels declaration  in second pass*/
        remove_label_from_line(line);

        /*get the line type*/
        type = get_line_type(line, asmContext);

        /*- - - - - - - - - ENTRY DIRECTIVE LINE HANDLING - - - - - - - - - -*/
        
        switch (type) {
            
            case ENTRY_DIRECTIVE_LINE: {

                /*get the entry label*/
                entry_label = get_entry_label(line, asmContext);
                if (!entry_label) {
                    asmContext->second_pass_error = true;
                    continue;
                }
                /*verify that the label is already defined as internal label*/
                if (!is_label_defined(entry_label, asmContext->labels)) {
                    asmContext->second_pass_error = true;
                    safe_free((void**)&entry_label);
                    asmContext->second_pass_error_line = asmContext->am_file_line;
                    print_external_error(ERROR_CODE_141);
                    continue;
                }

                /*get the label node*/
                if (get_label(entry_label, asmContext->labels)->definition == EXTERN) {
                    asmContext->second_pass_error_line = asmContext->am_file_line;
                    print_external_error(ERROR_CODE_142);
                    asmContext->second_pass_error = true;
                    safe_free((void**)&entry_label);
                    continue;
                }
                /*label found, update label's entry flag*/
                get_label(entry_label, asmContext->labels) -> is_entry = true;
                safe_free((void**)&entry_label);

                    break;
            }

            default:
                continue;
        }
    }

    /*relocate the instruction memory by moving all addresses after the memory offset*/
    if (!update_instruction_addresses(asmContext)) {
        print_external_error(ERROR_CODE_143);
        goto cleanup;
    }

    /*relocate the data memory by moving all addresses after the instruction memory*/
    if (!update_data_addresses(asmContext)) {
        goto cleanup;
    }

    /*update the  label addresses after data and instructions memory finally relocated*/
    if (!update_labels_addresses(asmContext)) {
        goto cleanup;
    }

    /* Update instruction memory with the resolved label addresses */
    if (!process_addr_update_requests(asmContext)) {
        goto cleanup;
    }


    /*if no error found, return true*/
    if (!asmContext->second_pass_error) {
        safe_free((void**)&line);
        fclose(am_file);
        return true;
    }



    cleanup:
    safe_free((void**)&line);
    asmContext->second_pass_error = true;
    fclose(am_file);
    return false;

}
