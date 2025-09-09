
#include "pre_processor.h"
#include <stdio.h>
#include <string.h>
#include <string.h>
#include "config.h"
#include "files.h"
#include "boolean.h"
#include "errors.h"
#include "lines_map.h"
#include "sys_memory.h"

/**
 * @file Pre_processor.c
 * @brief Assembler preprocessor implementation.
 *
 * This file contains the implementation of the assembler's preprocessing stage.
 * Its main responsibilities are:
 *   - Reading the raw assembly source file (.as).
 *   - Detecting, validating, and storing macro definitions (`mcro` ... `mcroend`).
 *   - Expanding macro calls into their defined content.
 *   - Writing the processed result into a new expanded file (.am).
 *   - Maintaining a line mapping table to preserve accurate error reporting.
 *
 * The preprocessor ensures that the assembler only receives pure assembly code,
 * with no macro directives, simplifying the parsing and encoding process.
 *
 *
 * @author Ivgeny Tokarzhevsky
 * @date 01/09/2025
 */

boolean execute_preprocessor(assembler_context* asmContext) {
    FILE* as_file;
    char line[MAX_LINE_LEN+2];/*+2 to cath line bigger then maximum allowed size*/
    char *macro_name = NULL;
    char* am_file_content = NULL;
    char *macro_content = NULL;
    macro_ptr macro;
    char* tmp = NULL;
    int origin_line_num =0;/*.as file line, used to build the line LUT*/
    int new_line_num = 0;/*.a file line, used to build the line LUT*/
    int macro_lines_count = 0;/*number of lines of macro content*/
    int i;
    int macro_line;/*macro content line, used to build line LUT*/



    /*verify assembler Context pointer exist*/
    if ( !asmContext) {
        print_internal_error(ERROR_CODE_25,"execute_preprocessor");
        return false;
    }



    /*allocate initial memory for file content buff*/
    am_file_content = handle_malloc(1);
    if ( !am_file_content) {
        safe_free((void**)&am_file_content);
        return false;
    }
    /*set '\0' at index 0 to not access unknown memory while using string functions*/
    am_file_content[0] = '\0';

    /* open the .as  file*/
    if ((as_file = open_file(asmContext->as_full_file_name, READ)) == NULL) {
        goto cleanUp;

    }

    /*read a line from file and search for macros*/
    while (fgets(line, MAX_LINE_LEN, as_file)) {
        /*calculate the line numbers for lines_map*/
        asmContext->as_file_line++;
        new_line_num++;
        origin_line_num ++;

        /*check if the line contains start of macro declare*/
        if ((macro_name = is_start_of_macro(line, asmContext)) != NULL) {

            /*check if macro_already_exist*/
            if (!is_name_valid(macro_name)) {
                print_external_error(ERROR_CODE_148);
            }

            /*verify that the macro name not used */
            if (!can_add_name(macro_name,asmContext)) {
                print_external_error(ERROR_CODE_147);
            }

            /*macro  exists get the macro content*/
            if (!read_macro_content(as_file, &macro_content, &macro_lines_count, asmContext)) {
                goto cleanUp;
            }

            /*add the macro to list*/
            if (!add_macro( macro_name, macro_content, macro_lines_count, &asmContext->macros, asmContext->as_file_line-macro_lines_count)) {
                goto cleanUp;
            }

            /*jump over the macro*/
            origin_line_num += macro_lines_count ;

            /* Temporarily adjust the .am file line counter back by one.
            * Reason: in the .as file there is a "mcroend" line, but this line does not exist
            * in the .am file. Without this adjustment, the line counters of the .as file
            * and the .am file would become misaligned.
            * On the next iteration both counters will be incremented,
            * and the .as file line will correctly match its corresponding .am file line.
            */
            new_line_num--;
        }



        /*if it's a macro call, find the content of the macro and add it to a temp buff*/
        else if ((macro = is_macro_call(line, asmContext->macros, asmContext)) != NULL) {

            /*reallocate memory for the line*/
            tmp = handle_realloc(am_file_content, strlen(am_file_content)+1 +strlen(get_macro(macro->name, asmContext->macros)->content) + 1);
            if (!tmp) {
                goto cleanUp;
            }
            else {
                am_file_content = tmp;
            }
            /*add the macro content to the whole file content*/
            strcat(am_file_content,get_macro(macro->name,asmContext->macros)->content);



            /*while macro called, add to lines look up table the line of macro content,
             * so when an error found in macro body, the error will print the num of the
             * invalid line that defined inside the macro declaration and not the line where
             * the macro called, because the issue is in macro content and not in the macro call.
             */
            i=1;
            macro_line = macro->define_line+1;
            while (i < macro->lines) {
                add_lines_to_map(macro_line, new_line_num, &asmContext->lines_maper);

                new_line_num++; i++; macro_line++;

            }
            new_line_num--;

        }

        else {/*the line is not a macro , print the original line from .as file .*/
            tmp = handle_realloc(am_file_content, strlen(line) + 1 + strlen(am_file_content)+1);
            if (!tmp) {
                goto cleanUp;
            }
            else {
                am_file_content = tmp;
            }
            /*add the line to the temp buffer*/
            strcat(am_file_content,line);
            add_lines_to_map(origin_line_num, new_line_num, &asmContext->lines_maper);
        }


    }





    /* create am file and return true if errors not found*/
    if (!asmContext->preproc_error) {
        /*create am_file and write to am file content*/
        if (!create_file(asmContext->am_full_file_name,am_file_content, asmContext)) {
            goto cleanUp;
        }

        safe_free((void**)&am_file_content);/*free allocated temp memory*/
        fclose(as_file);/*close the file*/

        return true;
    }




cleanUp:
    /*turn on pre-processing stage error flag*/
    asmContext->preproc_error = true;
    fclose(as_file);/*close the file*/
    safe_free((void**)&am_file_content);/*free allocated temp memory*/
    return false;
}


boolean add_macro(char *name, char *content, int lines_amount, macro_ptr *macro_list, int define_line) {

    macro_ptr temp = NULL;
    macro_ptr p1 = NULL;

    /*verify that all input pointer exist*/
    if (!name || !content ) {
        print_internal_error(ERROR_CODE_25,"add_macro");
        return false;
    }

    /*allocate memory for new node*/
    temp = (macro_ptr)handle_malloc(sizeof(macro));


    /*fill the new node values*/
    temp->name = name;
    temp->content = content;
    temp->lines = lines_amount;
    temp->define_line = define_line;
    temp->next = NULL;

    /*add the node at the end of the list*/
    if (*macro_list == NULL) {
        *macro_list = temp;
    }
    else {
        p1 = *macro_list;
        while (p1->next != NULL) {
            p1 = p1->next;
        }
        p1->next = temp;

    }

    return true;
}

void print_macro_names(macro_ptr macro_list) {

/*print the macro name and the macro content*/
    while (macro_list != NULL) {
        printf("macro name :%s\n", macro_list->name);
        printf("macro content :%s\n", macro_list->content);
        macro_list = macro_list->next;
    }
}


void free_macro_list(macro_ptr *macro_list) {
    macro_ptr temp = NULL;

    /*free the macro */
    while (*macro_list != NULL) {
        temp = *macro_list;
        *macro_list = (*macro_list)->next;

        safe_free((void**)&temp->name);
        safe_free((void**)&temp->content);
        safe_free((void**)&temp);
    }
}


char *is_start_of_macro(char *line, assembler_context *asmContext) {

    char *temp_line = NULL;
    char *token = NULL;
    char *macro_name = NULL;

    /*verify the line pointer is not NULL*/
    if (!line) {print_internal_error(ERROR_CODE_25, "is_start_of_macro"); return NULL;}

    /*copy the line to not change the original one*/
    temp_line = copy_string(line);


    /*get the first token and check if the token is maco definition command (mcro)*/
    token = strtok(temp_line, " \t\n");
    if (token == NULL || strcmp(token, asmContext->macro_declaration_table[MACRO_START]) != 0) {
        if (token != NULL && strcmp(token, asmContext->macro_declaration_table[MACRO_END]) == 0){
            /*mcroend found without macro declaration before*/
            print_external_error(ERROR_CODE_177);
            /*update the error flag because function return NULL*/
            asmContext->preproc_error = true;}
        safe_free((void**)&temp_line);
        return NULL;
    }

    /*if the previous token is macro definition command,
     *the current token should be the macro name*/
    token = strtok(NULL, " \t\n");
    if (token == NULL) {
        print_external_error(ERROR_CODE_176);
        asmContext->preproc_error = true;
        safe_free((void**)&temp_line);
        return NULL;
    }

    /*verify that line ends after macro name and that there is no unnecessary tokens*/
    if (strtok(NULL, " \t\n") != NULL) {
        safe_free((void**)&temp_line);
        print_external_error(ERROR_CODE_149);
        asmContext->preproc_error = true;
        return NULL;
    }

   /*copy the found macro name */
    macro_name = copy_string(token);


    /*return macro name if found*/
    safe_free((void**)&temp_line);
    return macro_name;
}




boolean is_end_of_macro(char *line, assembler_context *asmContext) {
    char *token = NULL;

    /*verify all input pointers exist*/
    if (!line){print_internal_error(ERROR_CODE_25, "is_end_of_macro"); return false;}

    /*get the first token and check if the token is a macro end command*/
    token = strtok(line, " \t \n");
    if (token != NULL && strcmp(token, asmContext->macro_declaration_table[MACRO_END]) == 0) {

        /*verify that the line ends after macro end command and there is no unnecessary tokens*/
        if ( strtok(NULL,"\n\t ") == NULL )
            return true;
        else {
            print_external_error(ERROR_CODE_150);
            asmContext->preproc_error = true;
            return true;
        }
    }
    return false;
}


boolean read_macro_content(FILE *fp_in, char** content_out, int *lines_count_out, assembler_context *asmContext) {
char temp_line[MAX_LINE_LEN + 2];
    char safe_line[MAX_LINE_LEN + 2];
    char *macro_content;
    unsigned long len;
    int lines_count = 0;
    char *temp = NULL;

    /*verify that all input pointers exist*/
    if (!content_out || !lines_count_out || !fp_in ) {
        print_internal_error(ERROR_CODE_25, "read_macro_content");
        return false;
    }

    /*allocate memory for macro content line*/
    macro_content = handle_malloc(sizeof(char));

    /*set '\0' at index 0 to not access unknown memory while using string functions*/
    macro_content[0] = '\0';

    /*read each line and add it to macro content*/
    while (fgets(temp_line, MAX_LINE_LEN + 2, fp_in) != NULL ) {
        asmContext->as_file_line++;
        lines_count++;
        strcpy(safe_line, temp_line);


            /*if end of macro command found, verify the macro is not empty
             *and end the read macro content operation*/
            if (is_end_of_macro(temp_line, asmContext) == true) {
                /*verify that the macro is not empty*/
                if (strlen(macro_content) == 0) {
                    print_external_error(ERROR_CODE_151);
                    asmContext->preproc_error = true;
                    goto cleanup;
                }
                /*set results at out pointers*/
                *content_out = macro_content;
                *lines_count_out = lines_count;
                return true;

            }
            /*macro end command not found yet*/
            else {

                /*calculate the new len of macro content with the new line*/
                len = strlen(macro_content) + strlen( safe_line) +1;

                temp = handle_realloc(macro_content, len);

                if (temp == NULL) {return false;}
                macro_content = temp;

                /*add the new line to macro the existing macro content*/
                strcat(macro_content, safe_line);

            }

    }/*reached end of line, mcroend command not found, print an error*/
    print_external_error(ERROR_CODE_152);
    asmContext->preproc_error = true;
    cleanup:
    safe_free((void**)&macro_content);
    return false;
}

macro_ptr is_macro_call(char *line, macro_ptr macro_list, assembler_context *asmContext) {
    char *token = NULL;
    char *line_copy = NULL;
    macro_ptr macro_node = NULL;

    /*verify that all input pointers exist*/
    if (!line || !asmContext){print_internal_error(ERROR_CODE_25, "is_macro_call"); return NULL;}

    /*get copy of the line to not change the original*/
    if ((line_copy = copy_string(line)) == NULL) {
        return NULL;
    }

    /*get the first token*/
    token = strtok(line_copy, " \t\n");

    /*if the token exist and the line is not empty*/
    if (token != NULL) {
        /*check if the token is a a macro name*/
        macro_node = get_macro(token, macro_list);
        if (macro_node != NULL) {
            /*macro name found, verify that no unnecessary tokens after macro name*/
            if (strtok(NULL, " \t\n") == NULL) {
                safe_free((void**)&line_copy);
                /*the line is macro call, return the full macro list's node*/
                return macro_node;
            }
            else {
                /*unnecessary token found after macro call*/
                print_external_error(ERROR_CODE_153);
                asmContext->preproc_error = true;
            }
        }
    }

    /*the line is not a macro call*/
    safe_free((void**)&line_copy);
    return NULL;


}

macro_ptr get_macro(char *name, macro_ptr macro_list) {

    /*verify that all input pointers exist*/
    if (!name){print_internal_error(ERROR_CODE_25, "get_macro"); return NULL;}

    /*iterate through the macro list*/
    while (macro_list != NULL) {
        /*check if the provided name is a macro name*/
        if (strcmp(macro_list->name, name) == 0) {
            /*if the name found, return the macro list's node*/
            return macro_list;
        }
        macro_list = macro_list->next;
    }
    return NULL;

}

boolean is_macro_defined(const char *name, macro_ptr macro_list) {

    /*verify that all input pointers exist*/
    if (!name){print_internal_error(ERROR_CODE_25, "is_macro_defined"); return false;}

    while (macro_list != NULL) {
        if (strcmp(macro_list->name, name) == 0) {
            return true;
        }
        macro_list = macro_list->next;
    }
    return false;
}

boolean is_PP_saved_name(const char* name, const assembler_context *asmContext) {

    int i;/*for loop iterator*/

    /*verify that all input pointers exist*/
    if (!name){print_internal_error(ERROR_CODE_25,"is_PP_saved_name"); return false;}

    /*check if the provided string is a saved pre processor name like mcro and mcroend*/
    for (i =0; i< MACRO_DECLARATION_WORDS; i++) {
        if (strcmp(name,  asmContext->macro_declaration_table[i]) == 0) {
            return true;
        }
    }

    return false;
}




