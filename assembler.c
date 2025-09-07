
#include "assembler.h"
#include <string.h>
#include "data_memory.h"
#include "errors.h"
#include "first_pass.h"
#include "externals.h"
#include "files.h"
#include "pre_processor.h"
#include "second_pass.h"
#include "tables.h"


/**
 * @file assembler.c
 * @brief Main driver of the assembler program.
 *
 * This file serves as the entry point and the main work flow of the
 * assembly program for one or more input `.as` source files.
 *
 * Responsibilities:
 *  - Parse and validate command-line arguments.
 *  - For each input file:
 *      - Initialize the assembler context.
 *      - Remove any previously generated output files.
 *      - Run preprocessing stage (.am file generation).
 *      - Execute the first pass (symbols collection, IC/DC setup).
 *      - Execute the second pass (relocations, entries resolution).
 *      - Generate output files (.obj, .ext, .ent as applicable).
 *      - Free all allocated resources.
 *  - Provide user-facing messages, progress reporting, and a final summary.
 *
 * The workflow ensures robust error detection at each stage and avoids
 * producing partial or inconsistent output files.
 *
 * @author Ivgeny Tokarzhevsky
 * @date 01/09/2025
 */


int main(int argc, char* argv[]) {

    /*start the assembler program*/
    if (run_assembler(argc, argv))
        return 0;

    return 1;
}

boolean run_assembler(int argc, char *argv[]) {

    int files = 0;
    int file_success = 0;
    boolean DEBUG = false;
    assembler_context assembler_context;

    /*- - - for debug mode - - -*/
    if (strcmp(argv[argc-1],"debug") == 0) {
        argc --;
        DEBUG = true;
    }


    /*exit if the source files missing*/
    if (argc < 2) {
        printf("ERROR: Missing assembly source file input. Processing cannot continue.\nProgram stopped.");
        return false;
    }

    printf("\n================ Assembler started ================\n\n");

    /*save the amount of the source files*/
    files= argc-1;

    /*start running assembler on each file*/
    while (--argc > 0){

        /*- - - - - - - - - - - - - - - initialize - - - - - - - - - - - -*/

        /*init assembler context*/
        init_assembler(&assembler_context);
        assembler_context.as_file_name = argv[argc];/*save the source file name*/
        remove_old_files(&assembler_context);


        printf("\n\n\n- - - Running assembler on file: <%s> - - -\n\n",assembler_context.as_file_name);



        /* - - - - - - - - - - - - file name validation - - - - - - - - - - -*/

        if (!is_file_name_valid(assembler_context.as_file_name, &assembler_context)) {
            continue;
        }


        /*get the .am file name*/
        assembler_context.am_file_name = change_file_extension(AM_FILE, assembler_context.as_file_name);
        if (assembler_context.am_file_name == NULL) {goto cleanup;}



        /*- - - - - - - - - - - - - - PREPROCESSOR - - - - - - - - - - - - - -*/
        /*execute preprocessor*/
        if (!execute_preprocessor(&assembler_context)) {
            printf("\n%s: preprocessing failed\n\n",assembler_context.as_file_name);

            goto cleanup;
        }
        printf("Preprocessing stage completed.\n\n");


        /* - - - - - - - - - - - - - - FIRST PASS - - - - - - - - - - - - - - - */

        /*execute first pass*/
        if (!execute_first_pass(&assembler_context)) {
            printf("First pass failed.\n\n");
            goto cleanup;
        }
        printf("First pass completed.\n\n");


        /* - - - - - - - - - - - - - - SECOND PASS - - - - - - - - - - - - - - -*/
        /*execute second pass*/
        if (!execute_second_pass(&assembler_context)) {
            printf("Second pass failed.\n\n");
            goto cleanup;
        }
        printf("Second pass completed.\n\n");



        /* - - - - - -  - - - - - - OUTPUT FILES GENERATION - -  - - - - - - -- - -*/

        /*creat obj file*/
        if (!create_obj_file(&assembler_context)) {
            printf("Error while creating obj file\n\n");

            goto cleanup;
        }

        /*create ext file*/
        if (is_externals_usage_exist(assembler_context.external_labels)) {
            if (!create_ext_file(&assembler_context)) {
                printf("Error while creating ext file\n\n");
                goto cleanup;

            }
        }


        /*create ent file*/
        if (is_entry_label_exist(assembler_context.labels)) {
            if (!create_ent_file(&assembler_context)) {
                printf("Error while creating entry file\n\n");
                goto cleanup;
            }

        }

        /*print user messages*/
        printf("Output files generated: ");
        if (assembler_context.obj_file_name) {
            printf("%s", assembler_context.obj_file_name);
        }
        if (assembler_context.ext_file_name) {
            printf(", %s", assembler_context.ext_file_name);
        }
        if (assembler_context.ent_file_name) {
            printf(", %s", assembler_context.ent_file_name);
        }


        /* - - - - - - - - - - - - - - CLEAN-UP - - - - - - - - - - - - - -*/
        cleanup:

        if (DEBUG)
            debug_data_print(&assembler_context);

        free_all_memory(&assembler_context);
        safe_free((void**)&assembler_context.am_file_name);
        safe_free((void**)&assembler_context.ext_file_name);
        safe_free((void**)&assembler_context.obj_file_name);
        safe_free((void**)&assembler_context.ent_file_name);
        /*as_file name not allocated and not need to free*/


        /*set the global error flag*/
        assembler_context.global_error = assembler_context.preproccess_error ||assembler_context.first_pass_error ||assembler_context.second_pass_error;

        /*print the final status of the file*/
        if (!assembler_context.global_error) {
            file_success++;/*inc success file counter*/
            printf("\n\nFile <%s> assembled successfully.\n\n\n",assembler_context.as_file_name);
        }
        else {
            /*remove previous .obj and .am file (if exist)*/
            printf("\n\nFile <%s> assembly failed.\n\n\n",assembler_context.as_file_name);

        }


    }/*move to the next file (if exist)*/

    /*assembler finished, print user message*/
    printf("\n\n================ Assembler finished ================\n\nSummary: %d out of %d files assembled successfully.\n\n",file_success,files);
    return true;
}



int init_assembler(assembler_context* context) {

    /*verify that input context exist*/
    if ( context == NULL) {
        print_internal_error(ERROR_CODE_25,"init_assembler");
        return false;
    }
    /*Initialize the context variables*/
    context->as_file_name = NULL;
    context->am_file_name = NULL;
    context->ext_file_name = NULL;
    context->obj_file_name = NULL;
    context->ent_file_name = NULL;
    context->memory_usage = 0;
    context->DC = 0;
    context->IC = 0;
    context->preproccess_error = false;
    context->first_pass_error = false;
    context->second_pass_error = false;
    context->global_error = 0;
    context->as_file_line = 0;
    context->am_file_line = 0;
    context->data_memory = NULL;
    context->instruction_memory = NULL;
    context->external_labels = NULL;
    context->labels = NULL;
    context->macros = NULL;
    context->address_update_requests = NULL;
    context->lines_maper = NULL;
    context->second_pass_error_line = 0;
    context->opcode_table = get_opcode_table();
    context->data_directive_table = get_data_directives_table();
    context->attributes_directive_table = get_attributes_directives_table();
    context->registers = get_registers();
    context->macro_declaration_table = get_macro_declaration_table();



    return true;
}
