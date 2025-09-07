
#include "files.h"
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "externals.h"
#include "config.h"
#include "data_memory.h"
#include "errors.h"
#include "instruction_memory.h"
#include "labels.h"
#include "util.h"


/**
 * @file files.c
 * @brief Provides file handling utilities for the assembler.
 *
 * This module manages all file-related operations such as:
 *  - Creating and writing to output files (.obj, .ext, .ent, .am).
 *  - Opening files safely with error handling.
 *  - Validating file names.
 *  - Changing source file extensions to assembler-specific extensions.
 *
 * It ensures consistent error reporting and safe memory usage when dealing with files.
 *
 * @author jenya tokarzhevsky
 * @date 01/09/2025
 */



/*files extensions table*/
static const char* file_extensions[] = {
    ".obj",
    ".ext",
    ".ent",
    ".am",
    ""
};
/*file access mode*/
static const char *file_mode_str[] = {
    "r",/*read*/
    "w",/*write*/
    "r+"/*read + write*/
};


/*
 * file name must exist
 * content is optional
 */
boolean create_file(const char* file_name, char* content, const assembler_context *asmContext) {
    char* file_content;
    FILE* file;

    /*validate that the file name exist*/
    if (file_name == NULL) {
        print_internal_error(ERROR_CODE_25,"create_file");
        return false;
    }
    /*if the file content not provided, set the content as empty string*/
    if (content == NULL) {
        file_content = "";
    }else {
        file_content = content;
    }

    /*try to open th file*/
    file = open_file(file_name, WRITE, asmContext);
    if (file == NULL) {

        return false;
    }

    /*write the content into the file*/
    fputs(file_content, file);

    /*close the file*/
    fclose(file);

    return true;
}

FILE* open_file(const char* file_name, file_mode file_mode, const assembler_context *asmContext) {
    FILE* file;

    /*validate that the file name exist*/
    if (!file_name) {
        print_internal_error(ERROR_CODE_25,"open_file");
        return NULL;
    }

    /*open the file with the provided access mode*/
    file = fopen(file_name, file_mode_str[file_mode]);
    /**error 144
     *
     */

    /*if file didn't open, print an error*/
    if (file == NULL) {
        if (errno == ENOENT) {
            print_external_error(ERROR_CODE_161,asmContext);
        }
        else if (errno == EBUSY) {
            print_external_error(ERROR_CODE_162,asmContext);
        }
        else if (errno == EACCES) {
            print_external_error(ERROR_CODE_163,asmContext);
        }
        return NULL;
    }
    return file;

}

boolean create_ent_file(assembler_context *asmContext) {


    FILE* ent_file;
    char* ent_file_name = NULL;
    char* base_4_str = NULL;
    label_ptr label_tmp;

    /*verify that assembler_context pointer exist*/
    if (asmContext == NULL) {
        print_internal_error(ERROR_CODE_25,"create_ent_file");
        return false;
    }

    /*extract the labels list from the context*/
    label_tmp = asmContext->labels;

    /*allocate memory for temp string that will hold the base 4 letters address*/
    base_4_str = handle_malloc(sizeof(char)*OBJ_FILE_ADDRESS_PRINT_LENGTH+1);


    /*create the .ent file*/
    ent_file_name = change_file_extension(ENTRY_FILE, asmContext->as_file_name);

    if ((ent_file = open_file(ent_file_name,WRITE, asmContext)) == NULL) {
        safe_free((void**)&ent_file_name);
        safe_free((void**)&base_4_str);
        return false;
    }

    fprintf(ent_file,"\n\n");


    /*- - - print entry labels addresses - - - */
    /*iterate through labels list, find the entry label
     *and write their name and address into the file*/
    while (label_tmp != NULL) {
        if (label_tmp->is_entry) {
            fprintf(ent_file, "\t%s\t", label_tmp->name);
            to_base4_str(label_tmp->address, OBJ_FILE_ADDRESS_PRINT_LENGTH, base_4_str);
            fprintf(ent_file,"%s\t\t\n", base_4_str);
        }

        label_tmp = label_tmp->next;
    }

   /*close the file and free allocated unnecessary memory*/
    fclose(ent_file);
    safe_free((void**)&base_4_str);
    /*set the new created ent file name*/
    asmContext->ent_file_name = ent_file_name;

    return true;
}

boolean create_ext_file(assembler_context *asmContext) {

    FILE* ext_file;
    char* ext_file_name = NULL;
    char* base_4_str = NULL;
    external_ptr external_tmp;

    /*verify that assembler_context pointer exist*/
    if (asmContext == NULL) {
        print_internal_error(ERROR_CODE_25,"create_ent_file");
        return false;
    }

    /*extract the external usage labels list from the context*/
    external_tmp = asmContext->external_labels;

    /*allocate memory for temp string that will hold the base 4 letters address*/
    base_4_str = handle_malloc(sizeof(char)*OBJ_FILE_ADDRESS_PRINT_LENGTH+1);


    /*create and open the .ext file*/
    ext_file_name = change_file_extension(EXTERNAL_FILE, asmContext->as_file_name);

    if ((ext_file = open_file(ext_file_name,WRITE, asmContext)) == NULL) {
        safe_free((void**)&ext_file_name);
        safe_free((void**)&base_4_str);
        return false;
    }


    fprintf(ext_file,"\n\n");

    /*- - - print external label's usage addresses - - - */
    while (external_tmp != NULL) {
        /*- - - print external labels usage addresses - - - */
        /*iterate through external labels list, find the external label
         *and write their name and usage address into the file*/
        fprintf(ext_file, "\t%s\t", external_tmp->label_name);
        to_base4_str(external_tmp->mem_address, OBJ_FILE_ADDRESS_PRINT_LENGTH, base_4_str);
        fprintf(ext_file,"%s\t\t\n", base_4_str);

        external_tmp = external_tmp->next;
    }

    /*close the file and free unnecessary memory*/
    fclose(ext_file);
    safe_free((void**)&base_4_str);
    /*update the new created ext file name in the context*/
    asmContext->ext_file_name = ext_file_name;

    return true;
}

boolean create_obj_file(assembler_context *asmContext) {

    FILE* obj_file;
    char* obj_file_name = NULL;
    char* base_4_str = NULL;
    data_ptr data_memory = NULL;
    instruction_ptr instruction_memory = NULL;
    data_ptr data_tmp = NULL;
    instruction_ptr instruction_tmp = NULL;

    /*verify that assembler_context pointer exist*/
    if (!asmContext) {
        print_internal_error(ERROR_CODE_25,"create_obj_file");
        return false;
    }

    /*extract the data and instruction memory from the context*/
     data_memory = asmContext->data_memory;
     instruction_memory = asmContext->instruction_memory;

    /*assign the temp pointer*/
    data_tmp = data_memory;
    instruction_tmp = instruction_memory;

    /*allocate memory for temp string that will hold the base 4 letters address*/
    base_4_str = handle_malloc(sizeof(char)* (OBJ_FILE_ADDRESS_PRINT_LENGTH > OBJ_FILE_DATA_PRINT_LENGTH ?  OBJ_FILE_ADDRESS_PRINT_LENGTH : OBJ_FILE_DATA_PRINT_LENGTH)+1);


    /*create and open the .obj file*/
    obj_file_name = change_file_extension(OBJECT_FILE, asmContext->as_file_name);

    if ((obj_file = open_file(obj_file_name,WRITE, asmContext)) == NULL) {
        safe_free((void**)&obj_file_name);
        safe_free((void**)&base_4_str);
        return false;
    }

    fprintf(obj_file,"\n\n");

    /*write the first line, instructions and data words amount
     * in vase 4 (letters) using to_base4_str func.
     */
    to_base4_str(asmContext->IC, -1, base_4_str);
    fprintf(obj_file, "\t\t%-4s", base_4_str);
    to_base4_str(asmContext->DC, -1, base_4_str);
    fprintf(obj_file,"\t%-4s\t\t\n", base_4_str);

    /*- - - print instruction memory - - - */
    while (instruction_tmp != NULL) {
        to_base4_str(instruction_tmp->address, OBJ_FILE_ADDRESS_PRINT_LENGTH, base_4_str);
        fprintf(obj_file, "\t\t%s\t", base_4_str);
        to_base4_str(instruction_tmp->value, OBJ_FILE_DATA_PRINT_LENGTH, base_4_str);
        fprintf(obj_file,"%s\t\t\n", base_4_str);

        instruction_tmp = instruction_tmp->next;
    }

    /*- - - print data memory - - -*/
    while (data_tmp != NULL) {
        to_base4_str(data_tmp->address, OBJ_FILE_ADDRESS_PRINT_LENGTH, base_4_str);
        fprintf(obj_file, "\t\t%s\t", base_4_str);
        to_base4_str(data_tmp->value, OBJ_FILE_DATA_PRINT_LENGTH, base_4_str);
        fprintf(obj_file,"%s\t\t\n", base_4_str);

        data_tmp = data_tmp->next;
    }

    /*close the file and clear unnecessary allocated memory*/
    fclose(obj_file);
    asmContext->obj_file_name = obj_file_name;
    safe_free((void**)&base_4_str);

    return true;

}

char* change_file_extension(file_type type, const char *as_file_name) {

    char* new_file_name = NULL;
    char* p;
    const char* obj_extension = file_extensions[type];

    /*verify the file name exist*/
    if (as_file_name == NULL) {
        print_internal_error(ERROR_CODE_25,"change_file_extension");
        return NULL;
    }

    /*copy the file name into a new memory*/
    new_file_name = copy_string(as_file_name);
    if (!new_file_name) {
        return NULL;
    }

    p = new_file_name;

    /*search the dot before the old file extension*/
    while (*p && *p != '.') {
        p++;
    }

    /*remove the old file extension by inserting '\0'*/
    if (*p == '.') {
        *p = '\0';
    }

    /*reallocate the new file name string*/
    p = handle_realloc(new_file_name, strlen(new_file_name) + strlen(obj_extension)+1);
    if (!p) {
        safe_free((void**)&new_file_name);
        return NULL;
    }

    new_file_name = p;

    /*concat the new file extension with the file name*/
    strcat(new_file_name, obj_extension);

    return new_file_name;

}

boolean is_file_name_valid(char* file_name, const assembler_context *asmContext) {
    char* p = file_name;
    char* file_extension = NULL;

    /*get the file extension*/
    file_extension = get_file_extension(file_name);


    /*verify that the file is an assembly file (.as)*/
    if (strcmp(file_extension,ASSEMBLY_FILE_EXTENSION) != 0) {
        print_external_error(ERROR_CODE_167,asmContext);
        free((void*)file_extension);
        return false;
    }

    /*verify that the file name contains only allowed letters*/
    while (*p) {
        if (*p == '.' || *p == '-' || *p == '_' || isspace(*p) || isalnum(*p)) {
            p++;
            continue;
        }
        print_external_error(ERROR_CODE_154,asmContext);
        free((void*)file_extension);
        return false;

    }

    /*validate the file name length*/
    if (strlen(file_name) > FILE_NAME_MAX_LEN) {
        print_external_error(ERROR_CODE_155,asmContext);
        free((void*)file_extension);
        return false;
    }
    free((void*)file_extension);
    return true;
}

char* get_file_extension(char* file_name) {
    char* p;
    size_t len;
    char* file_ext;

    if (!file_name) {
        print_internal_error(ERROR_CODE_25, "get_file_extension");
        return NULL;
    }

    len = strlen(file_name);
    if (len == 0) return NULL;

    /*point to the end of the string*/
    p = file_name + (len - 1);

    /* move backward to the last '.' or till the start of the string */
    while (p > file_name && *p != '.') {
        p--;
    }

    /*dot found, allocate memory for file extension*/
    if (*p == '.') {
        file_ext = handle_malloc(strlen(p) + 1);

        /*copy the file ext to allocated memory*/
        strcpy(file_ext, p);
        trim_edge_white_space(file_ext);

        return file_ext;
    }

    return NULL;/*no extension*/
}

void remove_old_files(assembler_context *asmContext) {

    /*remove all previous used files*/
    remove(asmContext->obj_file_name = change_file_extension(OBJECT_FILE,asmContext->as_file_name));
    remove(asmContext->am_file_name = change_file_extension(AM_FILE,asmContext->as_file_name));
    remove(asmContext->ent_file_name = change_file_extension(ENTRY_FILE,asmContext->as_file_name));
    remove(asmContext->ext_file_name = change_file_extension(EXTERNAL_FILE,asmContext->as_file_name));

    /*free files names*/
    safe_free((void**)&asmContext->obj_file_name);
    safe_free((void**)&asmContext->am_file_name);
    safe_free((void**)&asmContext->ent_file_name);
    safe_free((void**)&asmContext->ext_file_name);


}

