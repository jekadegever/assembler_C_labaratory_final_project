
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "util.h"
#include <stdlib.h>
#include "lines_map.h"
#include "config.h"
#include "data_memory.h"
#include "directives.h"
#include "errors.h"
#include "instructions.h"
#include "instruction_memory.h"
#include "labels.h"
#include "addresses.h"
#include "pre_processor.h"
#include "context.h"
#include "sys_memory.h"



/**
 * @file util.c
 * @brief Utility helpers for the assembler: strings, names/identifiers validation,
 *        whitespace trimming, base-4 formatting, line typing,
 *        global cleanup and many other utility functions.
 *
 * Provides small, reusable routines used across preprocessing, first/second pass,
 * encoding, files, and error handling modules.
 *
 *
 * @author Ivgeny Tokarzhevsky
 * @date 01/09/2025
 */


char* copy_string(const char *string) {
    char* copied_string;

    /*verify the input string is not NULL*/
    if (!string){print_internal_error(ERROR_CODE_25, "copy_string"); return NULL;}

    /*allocate memory for new string with the size of original string*/
    copied_string = (char*)handle_malloc(sizeof(char) * (strlen(string) + 1));

    /*copy the original string into the allocated memory*/
    strcpy(copied_string, string);


    /*return the string copy*/
    return copied_string;

}



boolean is_name_valid(const char *str) {

    int index = 0;/*str index*/

    /*verify the char pointer is not NULL*/
    if (!str) {
        print_internal_error(ERROR_CODE_25, "is_name_valid"); return false;
    }

    /*validate the name length*/
    if (strlen(str) > NAME_MAX_LEN) {
        return false;
    }

    /*validate that the first string char start with alpha numeric char*/
    if (!isalpha(str[index++])) {
        return false;
    }

    /*iterate through the name, verify that there is only alphanumeric or '_' sign*/
    while (str[index] != '\0') {
        if (isalnum(str[index]) || str[index] == '_') {

            index++;
            continue;
        }
        return false;

    }

    return true;

}

boolean can_add_name(const char *str, assembler_context *asmContext) {

    /*verify string not NULL*/
    if (!str) {
        print_internal_error(ERROR_CODE_25, "can_add_name"); return false;
    }

    /*return false if the name is already used or*/
    if ( is_sys_saved_name(str, asmContext) || is_used_name(str, asmContext)) {return false;}


    return true;

}



void trim_edge_white_space(char *str) {
    char *start = str;
    char *end;

    if (!str){return;}
    /*jump over white spaces from the left*/
    while (isspace((unsigned char)*start)) {
        start++;
    }

    /*if the str is only white chars, insert \0 at the start */
    if (*start == '\0') {
        str[0] = '\0';
        return;
    }

   /*Point to the end of the string*/
    end = start + strlen(start) - 1;

    /*Run over the string from the right*/
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }

    /*Delete all white spaces by inserting \0 after the last char of the string*/
    *(end + 1) = '\0';

    /*If there is difference, uppdate to the original address*/
    if (start != str) {
        memmove(str, start, end - start + 2);
    }
}



void trim_bracket_edge_spaces(char *line) {
    char *p1 = line;
    char* p2;

    /*verify that the line pointer is not NULL*/
    if (line == NULL) {
        return;
    }


    while (*p1) {
        /*Removing spaces after the '[' bracket */
        if (*p1 == '[') {/*Move p1 forward until finding '['*/

            p2 = p1 + 1;/*point to the first char after '['*/
            while (*p2 && isspace(*p2)) p2++;/*jump over the white spaces*/
            if (p2 != p1 + 1) {/*Remove the spaces if they exist*/
                memmove(p1 + 1, p2, strlen(p2) + 1);/*change the original like*/
            }
            p1++;
            continue;
        }
        /*Removing spaces before the ']' bracket*/
        if (*p1 == ']') {
            /*point to the first char before ']' bracket*/
            p2 = p1 - 1;
            while (p2 >= line && isspace(*p2)) p2--;/*Jump to the left over the white spaces*/
            p2++;

            if (p2 < p1) {
                /*change the original line */
                memmove(p2, p1, strlen(p1) + 1);
                p1 = p2; /* ']' bracket moved to the left, update p1 pointer*/
            }

        }

        p1++;
    }
}

boolean is_single_word(const char *str) {

    const char *p = str;

    /*verify the string is not NULL*/
    if (str == NULL) {
        print_internal_error(ERROR_CODE_25, "is_single_word");
        return false;
    }

    /*Run over the white spaces till the first non-white space char*/
    while (*p && isspace((unsigned char)*p)) {
        p++;
    }


    if (*p == '\0') {/*the line is empty*/
        return false;
    }

    /*Run over the word*/
    while (*p && !isspace((unsigned char)*p)) {
        p++;
    }

    /*Verify that only white space appear after the word*/
    while (*p && isspace((unsigned char)*p)) {
        p++;
    }

    /*If the pointer reach the NULL terminator after the white spaces, the str is a single word */
    return *p == '\0';
}


void print_binary(unsigned int num, int bits) {
    int i;

    for (i = bits - 1; i >= 0; i--) {
        /*prepare the bit mask*/
        unsigned int mask = 1u << i;
        /*get and print the bit value*/
        putchar((num & mask) ? '1' : '0');
    }


}

/* 0 -error
 * 1 - instruction
 * 2 - data directive
 * 3 - entry directive
 * 4- extern directive
 * 5 - empty
 */
line_type get_line_type(const char *line, assembler_context *asmContext) {

    char *copy_line;
    char *token;
    line_type type = UNKNOWN_LINE;

    /*verify that all input pointers exist*/
    if (!line || !asmContext)  {
        print_internal_error(ERROR_CODE_25, "get_line_type");
    }



 /* Verify that the line is not empty.
 *
 * This check is relevant only in the special case where an empty line
 * appears immediately after a label declaration.
 *
 * For a regular empty line, the assembler should never enter this function;
 * instead, it should detect the line as empty earlier and simply skip it.*/
    if (is_comment_or_empty_line(line)){return EMPTY_LINE;}


    /*get a line copy*/
    copy_line = copy_string(line);


    /*get the first token*/
    token = strtok(copy_line, " ");
    if (token == NULL) {
        goto cleanup;
    }

    /*check if the token is an opcode name*/
    if (is_opcode(token, asmContext)) {
        type = INSTRUCTION_LINE;
    }
    /*check if the token is an opcode name*/
    else if (is_directive(token, asmContext)) {
        /*check if the token is directive name*/
        if (is_data_directive(token, asmContext)) {
            type = DATA_DIRECTIVE_LINE;
        }
        /*check if the token is an attribute directive name*/
        else if (is_attribute_directive(token, asmContext)) {
            /*check if the token is entry directive name*/
            if (is_entry_directive(token, asmContext)) {
                type = ENTRY_DIRECTIVE_LINE;
            }
            /*check if the token is external directive name*/
            else if (is_extern_directive(token, asmContext)) {
                type = EXTERN_DIRECTIVE_LINE;
            }
        }

    }
    /*free copied line and return the line type if found,otherwise return NULL*/
    safe_free((void**)&copy_line);
    return type;


    cleanup:
    safe_free((void**)&copy_line);
    return type;

}


boolean is_comment_or_empty_line(const char *line) {
    const char *p = line;

    /*verify that the line pointer is not NULL*/
    if (p == NULL) {
        return false;
    }

    /*jump over the whitespaces*/
    while (*p && isspace((unsigned char)*p)) {
        p++;

    }
    /*return true if the lime empty or first char is ';' */
    if (*p == '\0' || *p == ';') {
        return true;
    }

    return false;
}

boolean is_sys_saved_name(const char *name, assembler_context *asmContext) {

    char* directive_temp_name;

    /*verify that all input pointers exist*/
    if (!name || !asmContext) {
        print_internal_error(ERROR_CODE_25, "is_sys_saved_name");
        return false;
    }
    /*allocate memory for temp directive name*/
    directive_temp_name = handle_malloc(strlen(name) + 2);/*for '\0' and to be able concat '.' to start of the name*/


    /* concat '.' to the start of the provided name.
     * this process done to be able using "is_directive" func */
    directive_temp_name[0] = '.';
    strcpy(directive_temp_name+1, name);

    /*check if the provided name is directive name*/
    if (is_directive(directive_temp_name, asmContext)) {
        return true;
    }
    /*check if the name is an opcode*/
    if (is_opcode(name, asmContext)) {
        return true;
    }
    /*check if the name is register name*/
    if (is_register(name, asmContext)) {
        return true;
    }
    /*check if the name is pre-processor saved name*/
    if (is_PP_saved_name(name, asmContext)) {
        return true;
    }

    /*free temp allocated memory*/
    safe_free((void**)&directive_temp_name);
    return false;
}


boolean is_used_name(const char *name, assembler_context *asmContext) {

    /*verify that all input arguments exist*/
    if (!name || !asmContext) {
        print_internal_error(ERROR_CODE_25, "is_used_name");
        return false;
    }
    /*check if the provided name already defined as label or macro name*/
    if (is_label_defined(name, asmContext->labels) || is_macro_defined(name, asmContext->macros)) {
        return true;
    }
    return false;
}


void to_base4_str(unsigned int num, int word_len, char *result_out) {

    static const char digits[] = {'a', 'b', 'c', 'd'};
    int i;
    int len;

    /*if the word_len argument provided pad with 'a' otherwise
     *the len of the word will be set automatically*/
    if (word_len == -1) {
        /*word len not provided, calculate the word len*/
        unsigned int tmp = num;
        len = 1;
        while (tmp >= 4) {
            tmp /= 4;
            len++;
        }
    } else {
        len = word_len;
    }

    /* calculate the base 4 and save the value in result_out */
    for (i = len - 1; i >= 0; i--) {
        result_out[i] = digits[num % 4];
        num /= 4;
    }
    /*end the string with null terminator*/
    result_out[len] = '\0';
}



void debug_data_print(const assembler_context *asmContext) {
    /*print data in debug mode*/
    printf("\n\nmemory usage : %d\n\n", asmContext->memory_usage);
    printf("\n\nIC = %d  DC = %d\n\n",asmContext->IC,asmContext->DC);
    printf("data memory :\n\n");
    print_data_memory(asmContext->data_memory);
    printf("\ninstruction memory :\n\n");
    print_instruction_memory(asmContext->instruction_memory);
    printf("\ninstructions address update requests:\n\n");
    print_addr_update_requests(asmContext->address_update_requests);
    printf("\nlabels :\n\n");
    print_labels(asmContext->labels);
    printf("\nmacros :\n\n");
    print_macro_names(asmContext->macros);
    print_lines_map(asmContext->lines_maper);

}

char* str_concat(const char *s1, const char *s2) {
    char *result;
    size_t len1, len2;

    /*verify that no null input*/
    if (!s1 || !s2) return NULL;

    /*calculate the length of each string*/
    len1 = strlen(s1);
    len2 = strlen(s2);

    /*allocate memory for new string*/
    result = handle_malloc(len1 + len2 + 1 * sizeof(char));


    /*insert the first string to allocated memory*/
    strcpy(result, s1);
    /*add the second string to the end of the first*/
    strcat(result, s2);

    return result;/*user responsible to free */
}
