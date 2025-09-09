
#include "files.h"
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sys_memory.h"
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
    file = open_file(file_name, WRITE);
    if (file == NULL) {

        return false;
    }

    /*write the content into the file*/
    fputs(file_content, file);

    /*close the file*/
    fclose(file);

    return true;
}

FILE* open_file(const char* file_name, file_mode file_mode) {
    FILE* file;

    /*validate that the file name exist*/
    if (!file_name) {
        print_internal_error(ERROR_CODE_25,"open_file");
        return NULL;
    }

    /*open the file with the provided access mode*/
    file = fopen(file_name, file_mode_str[file_mode]);


    /*if file didn't open, print an error*/
    if (file == NULL) {
        if (errno == ENOENT) {
            print_external_error(ERROR_CODE_161);
        }
        else if (errno == EBUSY) {
            print_external_error(ERROR_CODE_162);
        }
        else if (errno == EACCES) {
            print_external_error(ERROR_CODE_163);
        }
        return NULL;
    }
    return file;

}

boolean create_ent_file(assembler_context *asmContext) {


    FILE* ent_file;
    char* ent_file_name = NULL;
    char* base_4_str = NULL;
    char* ent_full_name = NULL;
    char* file_path = NULL;
    label_ptr label_tmp;

    /*verify that assembler_context pointer exist*/
    if (asmContext == NULL) {
        print_internal_error(ERROR_CODE_25,"create_ent_file");
        return false;
    }

    /*extract the labels list from the context*/
    label_tmp = asmContext->labels;

    /*allocate memory for temp string that will hold the base 4 letters address*/
    base_4_str = (char*)handle_malloc(sizeof(char)*OBJ_FILE_ADDRESS_PRINT_LENGTH+1);

    /*prepare the full file path*/
    ent_file_name = change_file_extension(ENTRY_FILE, asmContext->as_file_name);
    file_path =(asmContext->file_path == NULL ? EMPTY_STRING : asmContext->file_path);
    ent_full_name = str_concat(file_path, ent_file_name);

    /*create the .ent file*/
    if ((ent_file = open_file(ent_full_name,WRITE)) == NULL) {
        safe_free((void**)&ent_file_name);
        safe_free((void**)&base_4_str);
        safe_free((void**)&ent_full_name);
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
    safe_free((void**)&ent_full_name);
    /*set the new created ent file name*/
    asmContext->ent_file_name = ent_file_name;

    return true;
}

boolean create_ext_file(assembler_context *asmContext) {

    FILE* ext_file;
    char* ext_file_name = NULL;
    char* ext_full_name  = NULL;
    char* file_path = NULL;
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
    base_4_str = (char*)handle_malloc(sizeof(char)*OBJ_FILE_ADDRESS_PRINT_LENGTH+1);


    /*prepare the full file path*/
    ext_file_name = change_file_extension(EXTERNAL_FILE, asmContext->as_file_name);
    file_path = (asmContext->file_path == NULL) ? EMPTY_STRING : asmContext->file_path;
    ext_full_name = str_concat(file_path, ext_file_name);

    /*create and open the .ext file*/
    if ((ext_file = open_file(ext_full_name,WRITE)) == NULL) {
        safe_free((void**)&ext_file_name);
        safe_free((void**)&base_4_str);
        safe_free((void**)&ext_full_name);
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
    safe_free((void**)&ext_full_name);
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
    char* file_path = NULL;
    char* obj_full_name = NULL;

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
    base_4_str = (char*)handle_malloc(sizeof(char)* (OBJ_FILE_ADDRESS_PRINT_LENGTH > OBJ_FILE_DATA_PRINT_LENGTH ?  OBJ_FILE_ADDRESS_PRINT_LENGTH : OBJ_FILE_DATA_PRINT_LENGTH)+1);


    /*prepare the full file path*/
    obj_file_name = change_file_extension(OBJECT_FILE, asmContext->as_file_name);
    file_path = (asmContext->file_path == NULL) ? EMPTY_STRING : asmContext->file_path;
    obj_full_name = str_concat(file_path, obj_file_name);

    /*create and open the .obj file*/
    if ((obj_file = open_file(obj_full_name,WRITE)) == NULL) {
        safe_free((void**)&obj_file_name);
        safe_free((void**)&base_4_str);
        safe_free((void**)&obj_full_name);
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
    safe_free((void**)&obj_full_name);


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


    /*get reallocated address*/
    new_file_name = p;

    /*concat the new file extension with the file name*/
    strcat(new_file_name, obj_extension);

    /*return the new file name */
    return new_file_name;

}

boolean is_file_name_valid(char* file_name) {
    char* p = file_name;
    char* file_extension = NULL;
    unsigned int dot_counter = 0;/*only one dot acceptable for file extension*/

    /*if the file name doesn't exist*/
    if (!file_name) {
        print_external_error(ERROR_CODE_179);
        /*continue to find more errors*/
    }

    /*get the file extension*/
    file_extension = get_file_extension(file_name);/*free in the end*/


    /*verify that the file is an assembly file (.as)*/
    if ( !file_extension || strcmp(file_extension,ASSEMBLY_FILE_EXTENSION) != 0) {
        print_external_error(ERROR_CODE_167);
        safe_free((void**)&file_extension);
        return false;
    }

    /*verify that the file name contains only allowed letters*/
    while (*p) {
        if (*p == '.' || *p == '-' || *p == '_' || isspace(*p) || isalnum(*p)) {
            if (*p == '.') {
                dot_counter++;
            }
            p++;
            continue;
        }
        print_external_error(ERROR_CODE_154);
        safe_free((void**)&file_extension);
        return false;

    }

    /*only one dot acceptable for file extension, return error if found more then 1 dot*/
    if (dot_counter > 1) {
        print_external_error(ERROR_CODE_154);
        safe_free((void**)&file_extension);
        return false;
    }

    /*validate the file name length*/
    if (strlen(file_name) > FILE_NAME_MAX_LEN+ strlen(ASSEMBLY_FILE_EXTENSION)) {
        print_external_error(ERROR_CODE_155);
        safe_free((void**)&file_extension);
        return false;
    }

    /*file name is ok*/
    safe_free((void**)&file_extension);
    return true;
}

char* get_file_extension(char* file_name) {
    char* p;
    unsigned long len;
    char* file_ext;

    if (!file_name) {
        print_internal_error(ERROR_CODE_25, "get_file_extension");
        return NULL;
    }

    len = strlen(file_name);
    if (len == 0) return NULL;

    /*if the dot is the first or last char(no file name or no file extension
     *return NULL because no valid file extension found.*/
    if (*file_name == '.'  || *(file_name+len-1) == '.') {
        return NULL;
    }

    /*point to the end of the string*/
    p = file_name + (len - 1);

    /* move backward to the last '.' or till the start of the string */
    while (p > file_name && *p != '.') {
        p--;
    }

    /*dot found, allocate memory for file extension*/
    if (*p == '.') {
        file_ext = (char*)handle_malloc(strlen(p) + 1);

        /*copy the file ext to allocated memory*/
        strcpy(file_ext, p);
        trim_edge_white_space(file_ext);

        return file_ext;
    }

    return NULL;/*no extension*/
}

void remove_old_files(assembler_context *asmContext) {

    char* file_path = asmContext->file_path == NULL ? EMPTY_STRING : asmContext->file_path;
    /*remove all previous used files*/


    /*get the available file names*/
    char* obj_file_name = change_file_extension(OBJECT_FILE,asmContext->as_file_name);
    char* am_file_name = change_file_extension(AM_FILE,asmContext->as_file_name);
    char* ent_file_name  = change_file_extension(ENTRY_FILE,asmContext->as_file_name);
    char* ext_file_name  = change_file_extension(EXTERNAL_FILE,asmContext->as_file_name);

    /*get the files full name with directory*/
    char* obj_full_name = str_concat(file_path, obj_file_name);
    char* am_full_name = str_concat(file_path, am_file_name);
    char* ent_full_name = str_concat(file_path, ent_file_name);
    char* ext_full_name = str_concat(file_path, ext_file_name);

    /*remove the files*/
    remove(obj_full_name);
    remove(am_full_name);
    remove(ent_full_name);
    remove(ext_full_name);

    /*free files names*/
    safe_free((void**)&obj_file_name);
    safe_free((void**)&am_file_name);
    safe_free((void**)&ent_file_name);
    safe_free((void**)&ext_file_name);

    /*free full file name with directory*/
    safe_free((void**)&obj_full_name);
    safe_free((void**)&am_full_name);
    safe_free((void**)&ent_full_name);
    safe_free((void**)&ext_full_name);


}


/**
 * @brief Split a full path into directory part and filename part.
 *
 * Does not modify @p full_file_path_in. Allocates new strings for outputs.
 * Caller is responsible for freeing @p *name_out and @p *path_out.
 *
 * Rules:
 *  - Supports both '/' and '\\' as path separators.
 *  - If there is no directory component → *path_out = NULL.
 *  - If there is no filename (path ends with a separator) → *name_out = NULL.
 *  - Empty input string → both outputs NULL.
 *
 * @param full_file_path_in [in]  Full path string.
 * @param name_out          [out] Allocated filename or NULL.
 * @param path_out          [out] Allocated directory (without trailing sep) or NULL.
 * @return true on success, false on invalid arguments or allocation failure.
 */
boolean split_name_and_path(const char* full_file_path_in,char** name_out,char** path_out)
{
    const char *p;
    const char *last_sep = NULL;
    unsigned long path_len;
    char *directory = NULL;


    /* Validate pointers */
    if (!full_file_path_in || !name_out || !path_out) {
        print_internal_error(ERROR_CODE_25, "split_name_and_path");
        return false;
    }

    /*reset input parameters*/
    *name_out = NULL;
    *path_out = NULL;



    /*remove backslash in start of line (if exist)*/
    if (*full_file_path_in == '\\' || *full_file_path_in == '/')
        full_file_path_in++;



    /* the string is empty */
    if (*full_file_path_in == '\0') {
        return true;
    }


    /* Find last separator*/
    for (p = full_file_path_in; *p != '\0'; ++p) {
        if (*p == '/' || *p == '\\') {
            last_sep = p;
        }
    }

    if (last_sep == NULL) {
        /* no separator found, the entire input string is the file name */
        *name_out = copy_string(full_file_path_in);

        return true;
    }

    /* separator found */
    if (*(last_sep + 1) == '\0') {

       /*if the next char after separator is '\0', no file name in the string,
        * return only the file directory*/

        path_len = (last_sep - full_file_path_in)+1;

        if (path_len > 0) {
            /*allocate memory and copy the file patch*/
            directory = (char*)handle_malloc(path_len + 1);

            memcpy(directory, full_file_path_in, path_len);
            directory[path_len] = '\0';

            /*file directory found*/
            /*set the path_out parameter*/
            *path_out = directory;
        } else {
            /*file directory not found*/
            *path_out = NULL;
        }

        return true;
    }

    /*  - - - - - -file name found, check if directory exist  - - - - - -*/

    /*get the directory len*/
    path_len = (last_sep - full_file_path_in)+1;

    /*directory exist*/
    if (path_len > 0) {
        /*allocate memory and copy the directory*/
        directory = (char*)handle_malloc(path_len + 1);

        memcpy(directory, full_file_path_in, path_len);
        directory[path_len] = '\0';

        /*set directory out result*/
        *path_out = directory;
    } else {
        /* only file name found like text.txt */
        *path_out = NULL;
    }

    /*copy the file name part, everything after the '\'*/
    *name_out = copy_string(last_sep + 1);

    if (!*name_out) {
        /* In case of failure, free the allocated memory for directory (if the directory exist) */
        safe_free((void**)path_out);


        return false;
    }

    return true;
}

