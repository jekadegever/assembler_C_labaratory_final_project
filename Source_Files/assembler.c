
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
#include "context.h"
#include "sys_memory.h"



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
    if (argc > 1 && strcmp(argv[argc-1],"debug") == 0) {
        argc --;
        DEBUG = true;
    }


    /*verify that at least one file exist*/
    if (argc < 2) {
        /*exit if the source files missing*/
        printf("ERROR: Missing assembly source file input. Processing cannot continue.\nProgram stopped.");
        return false;
    }


    /*save the amount of the source files*/
    files= argc-1;


    /*source file/s found, execute the assembler*/
    printf("\n================ Assembler started ================\n\n");


    /*iterate through every source file*/
    while (--argc > 0) {


        /*========================================= initialize ==========================================-*/

        /*init assembler context*/
        init_assembler(&assembler_context);


        /*get the file name and file path*/
        /* allocated, required to release memory in the end*/
        split_name_and_path(argv[argc], &assembler_context.as_file_name,&assembler_context.file_path);


        /*if the file name provided without file extension, add the assembly source file's extension*/
        if (get_file_extension(assembler_context.as_file_name) == NULL) {
            assembler_context.as_file_name =  (char*)handle_realloc(assembler_context.as_file_name,(strlen(assembler_context.as_file_name)+strlen(ASSEMBLY_FILE_EXTENSION)+1) * sizeof(char));
            strcat(assembler_context.as_file_name,".as");
        }

        /* - - - - - - - - - - - - file name validation - - - - - - - - - - -*/

        if (!is_file_name_valid(assembler_context.as_file_name)) {
            safe_free((void**)&assembler_context.as_file_name);
            safe_free((void**)&assembler_context.file_path);

            /*continue to the next file*/
            continue;
        }


        /* - - - - - - full file path build for .as File - - - - - - -*/

                         /*.as file name already exist*/

        /*build and set the .am full file path*/
        assembler_context.as_full_file_name = str_concat((assembler_context.file_path == NULL ? EMPTY_STRING : assembler_context.file_path),assembler_context.as_file_name);

        /* - - - - - full file path build for .am File  -  - - - - -*/

        /*build and set the .am file name*/
        assembler_context.am_file_name = change_file_extension(AM_FILE, assembler_context.as_file_name);/*on error, memory allocation will stop the prog*/
        /*build and set the .am full file path*/
        assembler_context.am_full_file_name = str_concat((assembler_context.file_path == NULL ? EMPTY_STRING : assembler_context.file_path),assembler_context.am_file_name);


        /* - - - - - - remove old files  - - - - - - - -*/

        remove_old_files(&assembler_context);




        /* ========================================= start file assembly =====================================================*/


        printf("\n\n\n- - - Running assembler on file: <%s> - - -\n\n",assembler_context.as_file_name);



        /*-==============================PREPROCESSOR ===============================*/

        /*execute preprocessor*/
        if (!execute_preprocessor(&assembler_context)) {
            printf("\n%s: preprocessing failed\n\n",assembler_context.as_file_name);

            goto cleanup;
        }
        printf("Preprocessing stage completed.\n\n");



        /*=============================== FIRST PASS ===============================-*/


        /*execute first pass*/
        if (!execute_first_pass(&assembler_context)) {
            printf("First pass failed.\n\n");
            goto cleanup;
        }
        printf("First pass completed.\n\n");


        /*=============================== SECOND PASS ===============================-*/
        /*execute second pass*/
        if (!execute_second_pass(&assembler_context)) {
            printf("Second pass failed.\n\n");
            goto cleanup;
        }
        printf("Second pass completed.\n\n");



        /* ========================= OUTPUT FILES GENERATION ===========================-*/

        /*creat obj file*/
        if (!create_obj_file(&assembler_context)) {
            printf("Error while creating obj file\n\n");
            goto cleanup;
        }

        /*creat bin file*/
        if (!create_bin_file(&assembler_context)) {
            printf("Error while creating obj file\n\n");
            goto cleanup;
        }


        /*create ext file (if required)*/
        if (is_externals_usage_exist(assembler_context.external_labels)) {
            if (!create_ext_file(&assembler_context)) {
                printf("Error while creating ext file\n\n");
                goto cleanup;

            }
        }


        /*create ent file (if required)*/
        if (is_entry_label_exist(assembler_context.labels)) {
            if (!create_ent_file(&assembler_context)) {
                printf("Error while creating entry file\n\n");
                goto cleanup;
            }

        }

        /*print user messages, which files generated*/
        printf("Output files generated: ");
        if (assembler_context.obj_file_name) {
            printf("%s", assembler_context.obj_file_name);
        }
        if (assembler_context.ext_file_name) {
            printf(", %s", assembler_context.ext_file_name);
        }
        if (assembler_context.bin_file_name) {
            printf(", %s", assembler_context.bin_file_name);
        }
        if (assembler_context.ent_file_name) {
            printf(", %s", assembler_context.ent_file_name);
        }


        /*======================================= CLEAN-UP ======================================-*/
        cleanup:


        /*set the global error flag*/
        assembler_context.global_error = assembler_context.preproc_error ||assembler_context.first_pass_error ||assembler_context.second_pass_error;



        /*print the final assembly status of the file*/
        if (!assembler_context.global_error) {
            file_success++;/*inc success file counter*/
            printf("\n\nFile <%s> assembled successfully.\n\n\n",assembler_context.as_file_name);
        }
        else {
            printf("\n\nFile <%s> assembly failed.\n\n\n",assembler_context.as_file_name);
        }



        /*on debug mode, print saved assembler data*/
        if (DEBUG)
            debug_data_print(&assembler_context);



        /*free all allocated memory*/
        free_all_memory(&assembler_context);



        /* ============================================ end file assembly ==============================================*/


        /*======= continue to the next file ======*/
    }



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
    context->file_path = NULL;
    context->am_full_file_name = NULL;
    context->as_full_file_name = NULL;
    context->memory_usage = 0;
    context->DC = 0;
    context->IC = 0;
    context->preproc_error = false;
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
    context->bin_file_name = NULL;

    set_error_context(context);
    return true;
}
