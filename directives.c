
#include "directives.h"
#include "config.h"
#include "data_memory.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "errors.h"


/**
 * @file directives.c
 * @brief Directive handling module of the assembler.
 *
 * This file implements all logic related to assembler directives:
 *  - Parsing and validating .data, .string, and .mat directives.
 *  - Handling attribute directives such as .entry and .extern.
 *  - Extracting values, strings,attribute directive labels from source lines.
 *  - Inserting directive results into the data memory.
 *  - Performing syntax and semantic checks and reporting errors.
 *
 * The functions here ensure that every directive line in the assembly source
 * is correctly interpreted, validated, and inserted into the data memory.
 *
 * @author Ivgeny Tokarzhevsky.
 * @date 01/09/2025
 */




boolean handle_data_directives_line(char *line, assembler_context *asmContext){

    char *directive_name;
    boolean success = false;
    int* data = NULL;
    unsigned int data_count;
    unsigned long line_len;
    const char** data_directives_table;
    int i;

    /*verify all necessary input exist*/
    if (!line || !asmContext) {
        print_internal_error(ERROR_CODE_25,"Handle_data_directives_line");
        return false;
    }

    /*get the directives table from context*/
    data_directives_table = asmContext->data_directive_table;

    /*clear space in start of line*/
    while (isspace(*line)) {
        line++;
    }

    line_len = strlen(line);

    /*get the first token, directive name is expected*/
    directive_name = strtok(line, " ");

    if (directive_name == NULL) {
        print_external_error(ERROR_CODE_122, asmContext);
        return false;
    }

    /*******************************  GET THE DIRECTIVE VALUES  ************************************/


    /********************  get data values  *******************************/

    /*if it's .data directive, parse the line according to data directive format*/
    if (strcmp(data_directives_table[DATA_DIRECTIVE], directive_name) == 0) {
        /*move the line pointer after the directive name*/
        line+= strlen(directive_name);
        /*if the line pointer not at the end of the line,
        *move the line pointer  to the character immediately after the '\0' terminator inserted by strtok()*/
        if (line_len > strlen(directive_name)) {line++;}
        success = parse_data_directive(line, &data, &data_count, asmContext);



    }
    /****************   get matrix values  *******************************/

    /*if it's .mat directive, parse the line according to mat directive format*/
    else if (strcmp(data_directives_table[MAT_DIRECTIVE], directive_name) == 0) {
        /*move the line pointer after the directive name*/
        line+= strlen(directive_name);
        /*if the line pointer not at the end of the line,
        *move the line pointer  to the character immediately after the '\0' terminator inserted by strtok()*/
        if (line_len > strlen(directive_name)) {line++;}

        /*update success flag according to the parsing status*/
        success = parse_mat_directive(line, &data, &data_count, asmContext);



    }
    /****************   get string values  *******************************/

    /*if it's .string directive, parse the line according to string directive format*/
    else if (strcmp(data_directives_table[STRING_DIRECTIVE], directive_name) == 0) {
        /*move the line pointer after the directive name*/
        line+= strlen(directive_name);
        /*if the line pointer not at the end of the line,
        *move the line pointer  to the character immediately after the '\0' terminator inserted by strtok()*/
        if (line_len > strlen(directive_name)) {line++;}

        /*update success flag according to the parsing status*/
        success = parse_string_directive(line, &data, &data_count, asmContext);


    }
    if (!success) {
        return false;
    }

    /*******************************  INSERT THE VALUES TO DATA MEMORY ************************************/

if (asmContext->memory_usage <= MEMORY_AVAILABLE_SPACE) {
    /*Insert data to data memory*/
    for ( i = 0; i < data_count; i++) {

        /*check number range*/
        if (data[i] > MAX_NUM(WORD_BIT_SIZE) || data[i] < MIN_NUM(WORD_BIT_SIZE)) {
            print_external_error(ERROR_CODE_123, asmContext);
            goto cleanup;
        }
        if (!add_data_to_memory(data[i], &asmContext->data_memory, &asmContext->DC, &asmContext->memory_usage)) {
            if (asmContext->memory_usage == MEMORY_AVAILABLE_SPACE) {
                print_external_error(ERROR_CODE_103, asmContext);

            }
            goto cleanup;
        }
    }
}

    safe_free((void**)&data);
    return true;

    cleanup:
    safe_free((void**)&data);
    return false;


}



boolean parse_string_directive(char *line, int **out_result, unsigned int *out_count, assembler_context *asmContext) {

    char *result = NULL;
    int i;
    unsigned int str_len = 0;
    int * int_result;

    /*verify that all necessary input variables exist*/
    if (!out_result || !out_count || !line || !asmContext) {
        print_internal_error(ERROR_CODE_25,"parse_string_directive");
        goto cleanup;
    }

    /*move the line pointer to the start of the first word*/
   while (isspace(*line)) line++;

    /*extract the declared string from  the line*/
    result = extract_directive_string(line, asmContext);

    /*if there is format error or no string found in the line*/
    if (result == NULL) {
        goto cleanup;
    }

    /*get the string length (+1 for \0)*/
    str_len = strlen(result)+1;

    /*allocate memory for the extracted string */
    int_result = (int*)handle_malloc(str_len*sizeof(int)+1);

    /*convert extracted string to ascii values*/
    for (i = 0; i < str_len; i++) {
        int_result[i] = (int)result[i];
    }

    /*set the extracted values to out pointer*/
    *out_count = str_len;
    *out_result = int_result;


    /*free allocated memory*/
    safe_free((void**)&result);
    return true;

    cleanup:
    {safe_free((void**)&result);}
    return false;
}


boolean parse_data_directive(char *line, int **out_result, unsigned int *out_count, assembler_context *asmContext) {

    int *result = NULL;
    unsigned int count;


    /*verify that all necessary input variables exist*/
    if (!out_result || !out_count || !line || !asmContext) {
        print_internal_error(ERROR_CODE_25,"Handle_data_directives_line");
        return false;
    }
    /*move the line pointer to the start of the first char in the line*/
    while (isspace(*line)) line++;



    /*extract the number of .data directive*/
    if (!extract_directive_numbers(line, &count, &result, asmContext)) {
            return false;
    }

    /*if no numbers found in the data directive*/
    if (count == 0) {
        print_external_error(ERROR_CODE_129, asmContext);
        return false;
    }

    /*set the extracted values to out pointer*/
    *out_result = result;
    *out_count = count;


    return true;


}

boolean parse_mat_directive(char *line, int **out_result, unsigned int *out_count, assembler_context *asmContext) {

    unsigned int i;
    unsigned int count;
    int* result = NULL;
    int row;
    int col;
    char *token;
    int *temp;
    int n;
    unsigned long line_len;

    /*verify that all necessary input variables exist*/
    if (!out_result || !out_count || !line || !asmContext) {
        print_internal_error(ERROR_CODE_25,"parse_mat_directive");
        return false;
    }

    /*move the line pointer to the start of the first char in the line*/
    while (isspace(*line)) line++;

    /*hold the length of the line*/
    line_len = strlen(line);

    /* Remove spaces inside [][] to ensure proper strtok(" ") parsing */
    trim_bracket_edge_spaces(line);

    /*get the next token , [][] expected */
    token = strtok(line, " ");

    /*if its end of line, report an error*/
    if (!token) {
        print_external_error(ERROR_CODE_134, asmContext);
        return false;
    }

    /*verify the matrix size declaration format, expect for [col size][row size]*/
    if (!( ( sscanf(token, "[%d][%d]%n", &row, &col,&n) == 2)  && ((token[n] == '\0' || isspace(token[n])) && token[n-1]  == ']'))) {
        if (n == -1) {
            print_external_error(ERROR_CODE_134, asmContext);
            return false;
        }else {
            print_external_error(ERROR_CODE_135, asmContext);
            return false;
        }

    }

    /*verify that the received matrix size is not 0*/
    if (row ==0 || col ==0) {
        print_external_error(ERROR_CODE_136, asmContext);
        return false;
    }

    /*move the line pointer after [][] format*/
    line += strlen(token);
    /*if the line pointer not at the end of the line,
       *move the line pointer  to the character immediately after the '\0' terminator inserted by strtok()*/
    if (line_len > strlen(token)) {line++;}

    /*move the line pointer the  start of the next token*/
    while (isspace(*line)) line++;

    /*extract the matrix numbers*/
    if (!extract_directive_numbers(line, &count, &result, asmContext)) {
            goto cleanup;
    }

    /*verify the amount of number doesn't exceed the matrix size*/
    if (count > mat_size) {
        print_external_error(ERROR_CODE_137, asmContext);
        goto cleanup;
    }

    /*if the matrix size is bigger then the declared numbers amount,
     *pad the entire matrix space with 0*/
    if (count < mat_size) {
        temp = handle_realloc(result, mat_size * sizeof(int));
        if (temp == NULL) {
            goto cleanup;

        }
        /*set the pointer to the  new relocated address*/
        result = temp;

        /*insert 0 in the undefined matrix cells*/
        for (i = count; i < mat_size; i++) {
            result[i] = 0;
        }

    }

    /*set the extracted values in the mat directive to the out pointers*/
    *out_result = result;
    *out_count = mat_size;


    return true;

    cleanup:
    safe_free((void**)&result);
    return false;


}





boolean extract_directive_numbers(const char *line, unsigned int *out_count, int **out_result, assembler_context *asmContext) {

    /*enum to hold the expect status*/
    typedef enum expect_for {
        EXPECT_NUMBER,/*expect for a valid digit, or optional sign (+/-)*/
        EXPECT_AFTER_NUMBER/*expect for after line, can be the end of line, white space or comma sign ','*/
    } expect_for;

    char buff[MAX_LINE_LEN];/*temp buffer for each digit*/
    int buff_pointer = 0;/*temp buff pointer*/
    int arr[MAX_LINE_LEN];/*the extracted numbers array*/
    int arr_pointer = 0;/*extracted numbers array pointer*/
    int *numbers;/*pointer to address with all extracted numbers (address returns to calling func*/
    int i;/*for loop index*/
    const char *p;/*temp line pointer*/
    expect_for expect;/*expect flag*/

    /*verify that all necessary input variables exist*/
    if (!line || !out_result || !out_count || !asmContext) {
        print_internal_error(ERROR_CODE_25,"extract_directive_numbers");
        return false;
    }

    /*point to the start of the line */
    p = line;
    /*set the expect for flag*/
    expect = EXPECT_NUMBER;

    /*run on the line till the first found token*/
    while (isspace((unsigned char)*p)) p++;
    if (*p == '\0') {
        *out_count = 0;
        return true;
        /*empty line -> no errors and no numbers*/

    }

    /*start running till the end of the line*/
    while (*p != '\0') {
        if (expect == EXPECT_NUMBER) {
            /*search for optional polarity sign*/
            if (*p == '+' || *p == '-') {
                char sign = *p;
                p++;

                /*verify that the next char after sign is a digit*/
                if (!isdigit((unsigned char)*p)) {
                    print_external_error(ERROR_CODE_130,asmContext);
                    *out_count = 0;
                    return false;
                }
                /*number found after optional sign, insert the sign to buffer*/
                buff[buff_pointer++] = sign;
            }

            /*if optional sign doesn't appear, the current char must be a digit*/
            if (!isdigit((unsigned char)*p)) {
                print_external_error(ERROR_CODE_131,asmContext);
                *out_count = 0;
                return false;
            }

            /*read all digits*/
            while (isdigit((unsigned char)*p)) {
                buff[buff_pointer++] = *p;
                p++;
            }
            /*if the loop stopped because '.' found in the number*/
            if (*p == '.') {
                print_external_error(ERROR_CODE_173,asmContext);
                *out_count = 0;
                return false;
            }
            /*if the loop stopped because alpha char found in the number*/
            if (isalpha(*p)) {
                print_external_error(ERROR_CODE_174,asmContext);
                *out_count = 0;
                return false;
            }
            /*set \0 in the end of the numer string*/
            buff[buff_pointer] = '\0';

            /*convert the number string into a number and insert it to the numbers array*/
            arr[arr_pointer++] = atoi(buff);
            buff_pointer = 0;
            expect = EXPECT_AFTER_NUMBER; /*update expect flag*/
            /*move to the next char in the line*/
            continue;
        }
        /*while expecting for "after number" chars (end of line, space or comma ',' sign*/
        else {
            /*run over the whitespaces*/
            while (isspace((unsigned char)*p)) p++;

            /*comma ',' sign found, run over the whitespaces and expect for number now*/
            if (*p == ',') {
                p++;
                while (isspace((unsigned char)*p)) p++;
                expect = EXPECT_NUMBER;
                continue;
            }
            /*reached to the end of the line, exit while loop*/
            else if (*p == '\0') {
                break;
            }
            /*invalid char found while expecting for "after number" -> end of line or comma ',' sign*/
            else {
                print_external_error(ERROR_CODE_132,asmContext);
                *out_count = 0;
                return false;
            }
        }
    }

    /*if the line end while expecting for number, exit and print error*/
    if (expect == EXPECT_NUMBER) {
        print_external_error(ERROR_CODE_133,asmContext);
        *out_count = 0;
        return false;
    }

    /*no format error, prepare the numbers and return the pointer of to the found numbers*/


    /*allocate memory for found numbers*/
    numbers = handle_malloc(sizeof(int) * arr_pointer);

    /*move the found numbers to the allocated memory*/
    for (i = 0; i < arr_pointer; i++) {
        numbers[i] = arr[i];
    }

    /*set the out result*/
    *out_count = arr_pointer;
    *out_result = numbers;
    return true;
}

/*receive entire line after .string directive*/
char *extract_directive_string(char * line, assembler_context *asmContext) {

    char *p = line;/*pointer to the start of the received line*/
    int buff_pointer = 0;/*buff index pointer*/
    char buff[MAX_LINE_LEN];/*temp buff of extracted string*/
    char *result;/*pointer to the extracted string*/


    /*verify that all necessary input variables exist*/
    if (!line || !asmContext) {
        print_internal_error(ERROR_CODE_25,"extract_directive_string");
        return NULL;
    }

    /*run over whitespaces*/
    while (isspace((unsigned char)*p)) p++;

    /*if reach end of line, no string found in string directive*/
    if (*p == '\0') {
        print_external_error(ERROR_CODE_128, asmContext);
        return NULL;
    }

    /*the first char should be quotes, according to .string directive format*/
    if (*p != '"') {
        print_external_error(ERROR_CODE_124, asmContext);
        return NULL;
    }
    /*first quotes found, move to the next char*/
    p++;

    /*run till the second quotes or till the end of the lineL*/
    while (*p != '"' && *p != '\0') {
        /*the string between the quotes can be whitespace or alpha char */
        if (isalnum((unsigned char)*p) || isspace((unsigned char)*p)) {
            /*insert each found char into the buffer*/
            buff[buff_pointer++] = *p;
        }
        else {
            print_external_error(ERROR_CODE_125, asmContext);
            return NULL;
        }
        p++;
    }


    /*if the while loop stopped because reach to the end of line, the second quotes not found, format error*/
    if (*p == '\0') {
        print_external_error(ERROR_CODE_126, asmContext);
        return NULL;
    }
    else {
        /*second quotes found, set the end of line with '\0'*/
        buff[buff_pointer] = '\0';
    }

    /*jump over second squats*/
    p++;
    /*second quotes found, run till EOL and make sure there is no characters(only spaces)*/
    while (*p != '\0') {
        if (!isspace((unsigned char)*p)) {
            print_external_error(ERROR_CODE_127, asmContext);
            return NULL;
        }
        p++;
    }

    /*allocate memory for the found string*/
    result = handle_malloc(sizeof(char) * strlen(buff)+1);

    /*insert the found string into the allocated memory*/
    strcpy(result, buff);

    /*return the pointer to the found string*/
    return result;

}

boolean is_directive(const char *name, assembler_context *asmContext) {

    /*verify that all necessary input variables exist*/
    if (!name || !asmContext) {
        print_internal_error(ERROR_CODE_25,"is_directive");
        return false;
    }

    /*check if the received string represent a directive name*/
    if (is_data_directive(name, asmContext) || is_attribute_directive(name, asmContext))
        return true;

    return false;
}

boolean is_data_directive(const char *name, assembler_context *asmContext) {
    const char** data_directives_table;
    int i;

    /*verify that all necessary input variables exist*/
    if (!name || !asmContext) {
        print_internal_error(ERROR_CODE_25,"is_data_directive");
        return false;
    }

    /*get the data directive table*/
    data_directives_table = asmContext->data_directive_table;

    /*run over the table and check if the received string represent a data directive name*/
    for (i = 0; i< DIRECTIVES_AMOUNT; i++) {
        if (strcmp(name,data_directives_table[i])== 0) {
            return true;
        }
    }

    return false;
}

boolean is_attribute_directive(const char *name, assembler_context *asmContext) {
    const char** attributes_directives_table;
    int i;

    /*verify that all necessary input variables exist*/
    if (!name || !asmContext) {
        print_internal_error(ERROR_CODE_25,"is_attribute_directive");
        return false;
    }

    /*get the attribute directive table*/
    attributes_directives_table = asmContext->attributes_directive_table;

    /*run over the table and check if the received string represent a data directive name*/
    for (i = 0; i< ATTRIBUTE_DIRECTIVES_AMOUNT; i++) {
        if (strcmp(name,attributes_directives_table[i])== 0) {
            return true;
        }
    }

    return false;

}

boolean is_entry_directive(const char * name, assembler_context *asmContext) {

    const char** attributes_directives_table;

    /*verify that all necessary input variables exist*/
    if (!name || !asmContext) {
        print_internal_error(ERROR_CODE_25,"is_entry_directive");
        return false;
    }

    /*get the attribute directive table*/
    attributes_directives_table = asmContext->attributes_directive_table;

    /*check if the name is .entry directive*/
    if (strcmp(attributes_directives_table[ENTRY_DIRECTIVE], name)== 0 ) {
        return true;
    }

    return false;
}

boolean is_extern_directive(const char * name, assembler_context *asmContext) {

    const char** attributes_directives_table;

    /*verify that all necessary input variables exist*/
    if (!name || !asmContext) {
        print_internal_error(ERROR_CODE_25,"is_extern_directive");
        return false;
    }

    /*get the attribute directive table*/
    attributes_directives_table = asmContext->attributes_directive_table;

    /*check if the name is .extern directive*/
    if (strcmp(attributes_directives_table[EXTERN_DIRECTIVE], name)== 0 ) {
        return true;
    }

    return false;
}

char* get_extern_label(char* line, assembler_context *asmContext) {

    char* label;
    char* token;
    const char** attributes_directives_table;

    /*verify that all necessary input variables exist*/
    if (!line || !asmContext) {
        print_internal_error(ERROR_CODE_25,"get_extern_label");
        return NULL;
    }

    /*get the attribute directive table*/
    attributes_directives_table = asmContext->attributes_directive_table;

    /*get first token, should be .extern directive*/
    token = strtok(line, " \n\t");

    /*check if .extern directive exist*/
    if (!token || strcmp(token, attributes_directives_table[EXTERN_DIRECTIVE]) != 0) {
        return NULL;
    }

    /*get second token, should be the external label name*/
    token = strtok(NULL, " \n\t");

    if (!token) {
        print_external_error(ERROR_CODE_157,asmContext);
        return NULL;
    }

    /*if the external label name invalid*/
    if (!is_name_valid(token)) {
        print_external_error(ERROR_CODE_159,asmContext);
        return NULL;
    }
    /*if the label name already used*/
    if (!can_add_name(token, asmContext)) {
        print_external_error(ERROR_CODE_160,asmContext);
        return NULL;
    }
    /*check that no token after external label name*/
    if (strtok(NULL, " \n\t")) {
        print_external_error(ERROR_CODE_158,asmContext);
        return NULL;
    }

    /*allocate memory for extern label name*/
    label = handle_malloc(strlen(token) + 1);

    /*insert the extern label name into the allocated memory*/
    strcpy(label, token);

    /*return the found external label name*/
    return label;
}


char* get_entry_label(char* line, assembler_context *asmContext) {

    const char** attributes_directives_table;
    char* label;
    char* token;

    /*verify that all necessary input variables exist*/
    if (!line || !asmContext) {
        print_internal_error(ERROR_CODE_25,"get_entry_label");
        return NULL;
    }

    /*get the attribute directive table*/
    attributes_directives_table = asmContext->attributes_directive_table;

    /*get first token, should be .entry directive*/
    token = strtok(line, " \n\t");

    /*check if .entry directive exist*/
    if (!token || strcmp(token, attributes_directives_table[ENTRY_DIRECTIVE]) != 0) {
        print_external_error(ERROR_CODE_122,asmContext);
        return NULL;
    }

    /*get second token, should be the entry label name*/
    token = strtok(NULL, " \n\t");

    /*if label name doesn't exist*/
    if (!token) {
        print_external_error(ERROR_CODE_138,asmContext);
        return NULL;
    }

    /*if the label name invalid*/
    if (  !is_name_valid(token)) {
        print_external_error(ERROR_CODE_139,asmContext);
        return NULL;
    }

    /*check that no token after external label name*/
    if (strtok(NULL, " \n\t")) {
        print_external_error(ERROR_CODE_140,asmContext);
        return NULL;
    }

    /*allocate memory for entry label name*/
    label = handle_malloc(strlen(token) + 1);

    /*insert the entry label name into the allocated memory*/
    strcpy(label, token);

    /*return the found entry label name*/
    return label;
}


