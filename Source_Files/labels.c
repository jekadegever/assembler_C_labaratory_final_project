
#include "labels.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "util.h"
#include "sys_memory.h"


/**
 * @file labels.c
 * @brief Label management module for the assembler.
 *
 * This file implements all label-related operations in the assembler:
 *   - Detecting and extracting label definitions from source lines.
 *   - Managing a linked list of labels (add, search, print, free).
 *   - Differentiating between label types (code, data, extern, entry).
 *   - Providing utilities for label validation and retrieval during both passes.
 *
 * Labels are critical for memory addressing and data references.
 * They are collected in the first pass and resolved in the second pass.
 *
 * @author Ivgeny Tokarzhevsky
 * @date 01/09/2025
 */

char *find_label_definition(char *line, assembler_context *asmContext) {

    char *temp_line = NULL;
    char *token = NULL;
    char *name = NULL;
    unsigned long line_len;
    unsigned long token_len;

    /*verify that all input pointers exist*/
    if (!line || !asmContext) {print_internal_error(ERROR_CODE_25,"find_label_definition"); return NULL;}

    /*jump over white spaces*/
    while (isspace(*line)) line++;

    /*get the line's length*/
    line_len = strlen(line);

    /*get a copy of the provided assembly line*/
    temp_line = copy_string(line);

    /*get the first line token*/
    token = strtok(temp_line, "\t\n ");
    if (token == NULL) {
        safe_free((void**)&temp_line);
        return NULL;
    }

    /*get the len of word*/
    token_len = strlen(token);

    /*if the last char is ':', the token is a label*/
    if (token[strlen(token)-1] != ':') {
        safe_free((void**)&temp_line);
        return NULL;
    }



    /*remove the ':' from the label definition by '\0' insertion*/
    token[token_len-1] = '\0';
    token_len--;/*increase labels length*/


    /*verify that the label name is allowed*/
    if (is_name_valid(token)) {
        /*verify that the label name  not used yet*/
        if (!can_add_name(token, asmContext)) {
            print_external_error(ERROR_CODE_164);
            asmContext->first_pass_error = true;
        }
    }else {
        print_external_error(ERROR_CODE_145);
        asmContext->first_pass_error = true;
    }


    /*allocate memory for label name (even if the label name is invalid
     * if the name invalid, error will appear and program will execute the second pass */
    name = handle_malloc(sizeof(char)*token_len+1);
        strcpy(name, token);

    /*jump over the label name ,+ 2(because the end of the string is \0\0)*/
    token+= token_len+1;

    /*verify that the pointer will not jump over the end of the line*/
    if (line_len > token_len+1) {token++;}

    /*change the original line by removing the label definition from the line*/
    memmove(line,token,strlen(token)+1);


    safe_free((void**)&temp_line);
    return name;/*the caller should free the allocated memory for name*/
}




void remove_label_from_line(char *line) {

    char *temp_line = NULL;
    char *token = NULL;
    unsigned long len;

    /*verify the line is not NULL*/
    if (!line) { return;}

    /*get a copy of the line*/
    temp_line = copy_string(line);

    /* - - - - - - search for label definition - - - - - - - -*/


    /*extract the first token*/
    token = strtok(temp_line, "\t\n ");
    if (token == NULL) {/*no label definition to remove*/
        safe_free((void**)&temp_line);
        return;
    }

    /*get the length of word*/
    len = strlen(token);

    /*check that the last char is ':'*/
    if (token[strlen(token)-1] != ':') {
        safe_free((void**)&temp_line);
        return;
    }

   /* - - - - - - label definition found, remove it from the line - - - - - - - -*/


    /*jump over the label name ,+ 2(because the end of the string is \0\0)*/
    token+= len+2;
    /*change the original line by removing the label definition from the line*/
    memmove(line,token,strlen(token)+1);


    safe_free((void**)&temp_line);


}





boolean add_label(const char *new_name, unsigned int address, addr_type type, def_type definition, label_ptr *labels_list) {

    label_ptr new_label = NULL, temp = NULL;

    /*verify that all input pointers exist*/
    if (new_name == NULL || labels_list == NULL) {
        print_internal_error(ERROR_CODE_25,"add_label");
        return false;
    }

    /*allocate memory for new node*/
    new_label = (label_ptr)handle_malloc(sizeof(label));


    /*allocate memory for label name*/
    new_label->name = (char *) handle_malloc(strlen(new_name) + 1);


    /*insert values into the new allocated node*/
    strcpy(new_label->name, new_name);
    new_label->address = address;
    new_label->type = type;
    new_label->definition = definition;
    new_label->next = NULL;

    /*add the node in the end of the list*/
    if(*labels_list == NULL) {
        *labels_list = new_label;
    }
    else {
        temp = *labels_list;
        while(temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_label;
    }

    return true;


}

void free_label_list(label_ptr *labels_list) {
    label_ptr temp = NULL;

    /*free allocate node memory*/
    while(*labels_list != NULL) {
        temp = *labels_list;
        *labels_list = (*labels_list)->next;

        safe_free((void**)&temp->name);
        safe_free((void**)&temp);
    }
}

void print_labels(label_ptr labels_list) {

    /*print the label values*/
    while(labels_list != NULL) {
        printf("Label: %s  address: %d type: %s  define: %s\n", labels_list->name, labels_list->address,
            labels_list->type == CODE?"code":"data" ,(labels_list->is_entry?("entry"):(labels_list->definition == EXTERN?"external":"default")));
        labels_list = labels_list->next;
    }
}


 unsigned int get_label_address(const char* name, label_ptr labels_list) {

    /*verify label name pointer exist*/
    if (!name){print_internal_error(ERROR_CODE_25,"get_label_address"); return 0;}

    /*return the label address if the label defined*/
    while(labels_list != NULL) {
        if(strcmp(labels_list->name, name) == 0) {
            return labels_list->address;
        }
        labels_list = labels_list->next;
    }
    return 0;
}

boolean is_label_defined(const char *name, label_ptr labels_list) {

    /*verify name pointer exist*/
    if (!name) {print_internal_error(ERROR_CODE_25,"is_label_defined"); return false;}

    /*check if the label defined by search the label in the labels list*/
    while(labels_list != NULL) {
        if(strcmp(labels_list->name, name) == 0) {
            return true;
        }
        labels_list = labels_list->next;
    }
    return false;
}



label_ptr get_label(const char* name, label_ptr labels_list) {

    /*return the label list's node if it exist*/
    while(labels_list != NULL) {
        if(strcmp(labels_list->name, name) == 0) {
            return labels_list;
        }
        labels_list = labels_list->next;
    }
    return NULL;
}

void print_entry_labels(label_ptr labels_list) {

    /*print the entry labels*/
    while(labels_list != NULL) {
        if (labels_list->is_entry) {
            printf("entry label: %s -> address: \n", labels_list->name);
            print_binary(labels_list->address,10, STDOUT, NULL);
            printf("\n");
        }
        labels_list = labels_list->next;
    }
}

boolean is_entry_label_exist(label_ptr labels_list) {

    /*check if the label defined as entry*/
    while(labels_list != NULL) {
        if(labels_list->is_entry) {
            return true;
        }
        labels_list = labels_list->next;
    }
    return false;
}



