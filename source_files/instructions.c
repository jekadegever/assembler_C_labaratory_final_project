
#include "instructions.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "config.h"
#include "data_memory.h"
#include "util.h"
#include "instruction_memory.h"
#include "encoder.h"
#include "errors.h"
#include "context.h"
#include "sys_memory.h"


/**
 * @file instructions.c
 * @brief Instruction parsing and encoding module.
 *
 * This file contains all the logic for handling instruction lines in the assembler.
 * It provides functionality to:
 *   - Parse instruction mnemonics (opcodes).
 *   - Extract, validate, and classify operands.
 *   - Check addressing modes against opcode rules.
 *   - Encode instructions into machine words.
 *   - Insert encoded instructions into instruction memory.
 *
 * Error handling is performed via internal and external error reporting
 * to ensure correct assembler behavior.
 *
 * @author Ivgeny Tokarzhevsky
 * @date 01/09/2025
 */



boolean handle_instruction_line(char *line, assembler_context *asmContext) {
    char *token = NULL;
    opcode* opcode_info = NULL;
    operand* dest_operand = NULL;
    operand* src_operand = NULL;
    unsigned short* out = NULL;
    unsigned short count;
    int i;
    unsigned long line_len;
    unsigned long token_len;

    /*verify that all input pointer exist*/
    if (!line || !asmContext) {
        print_internal_error(ERROR_CODE_25,"handle_instruction_line");
        return false;
    }


    /*set the line length*/
    line_len = strlen(line);


    /*get the first token in the line, expect for the opcode name*/
    token = strtok(line," \t\n");
    if (!token) { print_external_error(ERROR_CODE_101);return  false;}




    /*get the opcode info (num of operands, valid addressing mode, opcode value etc..)*/
    opcode_info = get_opcode_info(token, asmContext);
    if (!opcode_info) {
        print_external_error(ERROR_CODE_100);
        goto cleanUp;
    }

    /*get the token len*/
    token_len = strlen(token);

    /*move the line pointer after the opcode name*/
    token += token_len;
    /*if the line pointer not at the end of the line,
      *move the line pointer  to the character immediately after the '\0' terminator inserted by strtok()*/
    if (line_len>token_len) {token++;}

    /*cut the opcode name from the line by moving the line backward*/
    memmove(line,token,strlen(token)+1);

    /*handle the instruction line according to the operands amount*/
    switch (opcode_info->operands_amount) {
        /*- - - - instruction without operands - - - */
        case 0 : if (strtok(NULL," ") != NULL){print_external_error(ERROR_CODE_106); return false;}
        break;




        /* - - - - instruction with 1 operand - - - -*/
        case 1 : {

            /*allocate memory for dest operand*/
            dest_operand = handle_malloc(sizeof(operand));

            /*handle the 1 operand instruction line*/
            if (!handle_one_operand_line(*opcode_info, line, dest_operand, asmContext)){goto cleanUp;}

            break;
        }


        /* - - - - instruction with 2 operands - - - -*/
        case 2 : {

            /*allocate memory for the dest and source operands*/
            dest_operand = handle_malloc(sizeof(operand));

            /*allocate memory for source operand*/
            src_operand = handle_malloc(sizeof(operand));

            /*handle the 2 operands instruction line*/
            if (!handle_two_operands_line(*opcode_info, line, src_operand, dest_operand, asmContext)){goto cleanUp;}

            break;


            /* - - - - invalid instruction operands amount - - - -*/
            default: print_internal_error(ERROR_CODE_26, "handle_instruction_line");
            goto cleanUp;
        }
    }



    /*start encoding instruction line into the machine code*/
    if (!instruction_encoder(opcode_info, src_operand, dest_operand,asmContext,&out,&count)) {
        goto cleanUp;

    }


    /* - - - insert the encoded machine words of the instruction line into the instruction memory - - -*/

        /*verify that the memory is not full*/
    if (asmContext->memory_usage <= MEMORY_AVAILABLE_SPACE) {
        /*- - - add instruction codes to memory - - - -*/
        for ( i =0; i<count; i++) {
            if (!add_instruction_to_memory(out[i], &(asmContext->instruction_memory), &(asmContext->IC), &asmContext->memory_usage)) {
                /*memory insertion interrupted because the memory is full*/
                if (asmContext->memory_usage == MEMORY_AVAILABLE_SPACE){
                    print_external_error(ERROR_CODE_103);
                    /* Increment memory counter past the limit to ensure the overflow error is reported only once */
                    asmContext->memory_usage ++;
                }



                goto cleanUp;
            }

        }
    }


    /*free allocated memory*/
    safe_free((void**)&opcode_info);
    safe_free((void**)&src_operand);
    safe_free((void**)&dest_operand);
    safe_free((void**)&out);

    return true;

    cleanUp:
    safe_free((void**)&opcode_info);
    safe_free((void**)&dest_operand);
    safe_free((void**)&src_operand);
    safe_free((void**)&out);

    return false;

}


boolean handle_one_operand_line(opcode opcode_info, char *entire_line, operand *dest_operand, assembler_context *asmContext) {

    char* dest_operand_str = NULL;/*will hold the operand string  in the line*/
    int operands_found;/*will hold the amount of found operands in the line*/


    /*verify all input pointers exist*/
    if (!dest_operand || !entire_line) {
        print_internal_error(ERROR_CODE_25,"handle_one_operand_line");
        return false;
    }

    /*Extract operand token from line and hold it at 'dest_operand_str' and the found operands amount
     * at 'operands_found'.
     */
    operands_found = extract_operands(entire_line, NULL, &dest_operand_str, asmContext);


    /*verify that the amount of found operands fits to opcode operands amount */
    if (operands_found == 0) {
        print_external_error(ERROR_CODE_105);
        return false;
    }
    if (operands_found > 1) {
        print_external_error(ERROR_CODE_106);
        return false;
    }
    if (operands_found == -1) {
        return false;
    }


    /*parse the operand word and build the 'dest_operand' struct*/
    if (!parse_operand(dest_operand_str, dest_operand, asmContext)) {
        print_external_error(ERROR_CODE_114);
        return false;
    }


    /*verify that the operand addressing mode acceptable for current opcode*/
    if (!(AM_BIT(dest_operand->type) & opcode_info.dest)) {
        print_external_error(ERROR_CODE_116);
        return false;
    }

    return true;
}


boolean handle_two_operands_line(opcode opcode_info, char *entire_line, operand *src_operand, operand *dest_operand, assembler_context *asmContext) {
    char* dest_operand_str = NULL;
    char* src_operand_str = NULL;
    int operands_found = 0;

    /*verify that all input pointers exist*/
    if (!src_operand || !dest_operand || !entire_line) {
        print_internal_error(ERROR_CODE_25,"handle_two_operands_line");
        return false;
    }

    /*Extract operands words from line and hold it at 'dest_operand_str' and 'src_operand_str' vars
     * and the amount of found operands at 'operands_found'. */
    operands_found = extract_operands(entire_line, &src_operand_str, &dest_operand_str, asmContext);


    /*verify that the amount of the found operands fits to opcode operands amount */
    if (operands_found == 0) {
        print_external_error(ERROR_CODE_105);
        return false;
    }
    if (operands_found  == 1) {
        print_external_error(ERROR_CODE_117);
        return false;
    }
    if (operands_found > 2) {
        print_external_error(ERROR_CODE_106);
        return false;
    }
    if (operands_found == -1) {
        return false;
    }


    /*parse the operand word and build the 'src_operand' struct*/
    if (!parse_operand(src_operand_str, src_operand, asmContext)) {
        print_external_error(ERROR_CODE_113);
        return false;
    }

    /*parse the operand word and build the 'dest_operand' struct*/
    if (!parse_operand(dest_operand_str, dest_operand, asmContext)) {
        print_external_error(ERROR_CODE_114);
        return false;
    }


    /*check if the src operand can be used in this opcode*/
    if (!(AM_BIT(src_operand->type) & opcode_info.source)) {
        print_external_error(ERROR_CODE_115);
        return false;
    }

    /*check if the src operand can be used in this opcode*/
    if (!(AM_BIT(dest_operand->type) & opcode_info.dest)) {
        print_external_error(ERROR_CODE_116);
        return false;
    }


    return true;
}



/*if 1 operand found, it will be set at dest operand
if 2 operands found, first will set as src and second as dest
otherwise both will be NULL
returns the num of found operands
if  format error like double comma (",,") return -1*/
int extract_operands(char *line, char **src_out, char **dest_out, assembler_context *asmContext) {
    char* p1 = NULL;
    char* p2 = NULL;
    int operands_count = 0;
    int comma_count = 0;
    int i =0;


    /*Init out pointers*/
    if (src_out)  *src_out  = NULL;
    if (dest_out) *dest_out = NULL;
    operands_count = 0;

    /*verify that the line isn't empty*/
    if (!line || !asmContext) {
        print_internal_error(ERROR_CODE_25,"extract_operands");
        safe_free((void**)&line);
        return -1;
    }

    /*remove white spaces from left and right sides of whole line*/
    trim_edge_white_space(line);

    if (strlen(line) == 0) {return 0;}


    /*Count the existing comma signs ',' in the line.
     used later to validate that there is no unnecessary comma signs*/
   for (i = 0; line[i] != '\0'; i++) {
       if (line[i] == ',') {
           comma_count++;
       }
   }


    /*In case of matrix operand , whitespace characters inside the [][] will be removed.
    this is important in order to recognize this operand as a continuous word rather than two or more words separated by spaces*/
    trim_bracket_edge_spaces(line);



    /*Extract the operands split by comma sign, p1 will point to strat of first operands str, p2 will point to start of second operand str*/

    /* - - - FIRST OPERAND - - -*/
    p1 = strtok(line, ",");
    if (p1) {/*if exist*/
        trim_edge_white_space(p1);
        if (strlen(p1) > 0) {
            operands_count++;
        }
        /*- - - SECOND OPERAND - - -*/
        p2 = strtok(NULL, ",");
        if (p2) {/*if exist*/
            trim_edge_white_space(p2);
            if (strlen(p2) > 0) {
                operands_count++;
            }

            /*check if the line contains more unnecessary operands */
            while (strtok(NULL, ",") != NULL) {
                operands_count++;
            }
        }
    } else {
        /*No operands found, return 0*/
        return 0;
    }


 /* - - - - -  format check  - - -- - -*/

    /* Verify that each operand is a single token.
     * If an operand spans multiple tokens, it indicates a missing comma.
     * Example: "r1 r2,r4" → the first operand ("r1 r2") is invalid due to the missing comma. */
    if (!is_single_word(p1) || (p2 != NULL && !is_single_word(p2))) {
        print_external_error(ERROR_CODE_119);
        return -1;
    }

    /*verify that there is not double comma,comma at the beginning or at the end
     *the comma sign amount should fit to the operands amount */
    if (comma_count != operands_count-1) {
        if (*line == ',') {
            print_external_error(ERROR_CODE_168);
        }
        if (line[strlen(line) - 1] == ',') {
            print_external_error(ERROR_CODE_169);
        }
        if (*line != ',' && line[strlen(line) - 1] != ',') {
            print_external_error(ERROR_CODE_170);
        }
        return -1;
    }


    /*Set the results (found operands and operands num into the received input pointers)*/
    if (operands_count == 1) {
        if (dest_out) *dest_out = p1;
    } else if (operands_count == 2) {
        if (src_out)  *src_out  = p1;
        if (dest_out) *dest_out = p2;
    }


    return operands_count;
}


/*parse_operand function, gets string that represent an operand like #-7, mat[r1][r2], R4 etc..
 * finding the type of the operand (mat, label, immediate, register)
 * by calling other function.
 * param operands_str -> the operand's string from line.
 * param operand -> operand  struct
*returns 1 if the operand type exist and it parsed successfully, otherwise return 0*/
boolean parse_operand(char *operand_str, operand *operand, assembler_context *asmContext) {


    /*verify all input pointers exist*/
    if (!operand_str || !operand) {
        print_internal_error(ERROR_CODE_25,"parse_operand");
        return false;
    }

    /*check if the operand is a register, if it does, parse it and insert values into the operands struct*/
    if (try_parse_reg_operand(operand_str,operand, asmContext)) {
            return true;
    }

    /*check if the operand is an immediate(constant number), if it does, parse it and insert values into the operands struct*/
    if (try_parse_immediate_operand(operand_str,operand, asmContext)) {
            return true;

    }

    /*check if the operand is a matrix number, if it does, parse it and insert values into the operands struct*/
    if (try_parse_matrix_operand(operand_str,operand, asmContext)) {
        return true;
    }

    /*check if the operand is a direct operand (label), if it does, parse it and insert values into the operands struct*/
    if (try_parse_direct_operand(operand_str,operand, asmContext)) {
        return true;
    }

    else {
        return false;
    }



}

/* func is_register, check if the given string represent a name of existing register.
 * param str - the string that should be checked if it's register name or not
 * returns the register number if the register exist, otherwise return -1. */
int get_reg_num(const char* str, assembler_context *asmContext) {

    int i;
    const char** registers = NULL;

    /*verify that all input pointers exist and they are not NULL*/
    if (!str || !asmContext){print_internal_error(ERROR_CODE_25,"get_reg_num");return false;}

    /*get the registers table from the context*/
    registers  = asmContext->registers;

    /*check if the given string represent a valid register,
     * if it does, return the register int value. */
    for (i=0; i < REGISTERS_AMOUNT; i++) {
        if (strcmp(str,registers[i]) == 0) {
            return i;
        }
    }
    /*the string is not a register, return -1*/
    return -1;
}


boolean is_register(const char *str, assembler_context *asmContext) {

    /*verify that the input string is not NULL*/
    if (!str) {print_internal_error(ERROR_CODE_25,"is_register");return false;}

    /*check if the provided string is a register name*/
    if (get_reg_num(str, asmContext) == -1) {
        return false;
    }

    return true;
}

boolean try_parse_matrix_operand(const char *operand_str, operand *operand, assembler_context *asmContext) {


    char* label_name = NULL;
    char* temp1 = NULL;
    char* temp2 = NULL;
    char* p = NULL;
    int reg_1;
    int reg_2;
    boolean format_error = false;


    /*verify that all input pointers exist*/
    if (operand_str == NULL || operand == NULL) {
        print_internal_error(ERROR_CODE_25,"try_parse_matrix_operand");
        return 0;
    }

    /*copy the operand's string to not change the original one due the process*/
    p = copy_string(operand_str);
    if (!p) {return false;}

    /*point to the start of the string, the first token
     *in the string should be a label name*/
    label_name = p;




    /*Start iterate over the given string.*/
    /*the string till the first '[' sign should be a label name.*/
    while (*p && *p != '[') {
        if (isspace(*p)) {
            goto not_matrix_found;;
        }
        p++;
    }
    /*if the previous while loop stopped because '[' found, cut the line by
     *NULL terminator insertion and move the pointer forward*/
    if (*p == '[') {
        *p = '\0';
        p++;
        /*now, 'temp1' points to the first char after '[' sign*/
        temp1 = p;

        /*run till the ']' sign or till the end of the line*/
        while (*p && *p != ']') {
            if (isspace(*p)) {
                goto not_matrix_found;;

            }
            p++;
        }
        /*if the previous while stopped because ']' found and if the next char is '[', cut the line by
         * NULL terminator insertion and move the pointer forward (temp1 contains the string inside the first
         * square brackets. */
        if (*p == ']' && *(p+1) == '[' ) {
            *p = '\0';
            p+=2;
            /*now 'temp2' points to the first char after the '[' sign (inside the
             * second square brackets)
             */
            temp2 = p;

            /*run till the ']' sign or till the end of the line*/
            while (*p && *p != ']') {
                if (isspace(*p)) {
                    goto not_matrix_found;
                }
                p++;
            }
            /*if the previous while loop stopped because ']' sign found, cut the line by
             *NULL terminator insertion and move the pointer forward*/
            if (*p == ']') {
                *p = '\0';
                p++;

                /*now 'label_name' contains the first string before [][],
                 * 'temp1' contains the string inside the first [], 'temp2' contains
                 * the string inside the second []*/

                /*according to matrix format, after [][] only whitespace or end of line
                 *can exist*/
                while (isspace(*p) || *p != '\0') {
                    p++;
                }

                /*if the previous while loop stopped because it reached the end of the line
                 *so the format is valid*/
                if (*p == '\0') {

                /* - - - - - - - - - - - - - - matrix operand found -> verify the foramt  - - - - - - - - - - - - - -*/

                    /*parse the 'label_name', 'temp1', 'temp2'*/

                     /* verify that the 'label_name' string represent a valid label name,
                     * 'temp1' and 'temp2' represent a valid register names*/
                    if (!is_register(temp1, asmContext)) {
                        print_external_error(ERROR_CODE_110);
                        format_error = true;
                    }
                    if (!is_register(temp2, asmContext)) {
                        print_external_error(ERROR_CODE_111);
                        format_error = true;
                    }

                    if (!is_name_valid(label_name)) {
                        print_external_error(ERROR_CODE_112);
                        format_error = true;
                    }

                    /*able the previous functions print all found errors*/
                    if (format_error) {
                        /*matrix operand found with format error*/
                        goto matrix_format_error;
                    }

                    /*- - - - - - - - - matrix operand found, format is ok -> insert values into operand struct - - - - - - - - - -*/

                    /*get registers numeric values*/
                    reg_1 = get_reg_num(temp1, asmContext);
                    reg_2 = get_reg_num(temp2, asmContext);


                    /*If the name of label is valid, and the register exists, set the result by filling the operand structure*/
                    operand->type = MATRIX_ACCESS;
                    strcpy(operand->operand_val.matrix.label,label_name);
                    operand->operand_val.matrix.reg_1 = reg_1;
                    operand->operand_val.matrix.reg_2 = reg_2;
                    operand->operand_val.matrix.reg_encoding = ABSOLUTE;
                    operand->encoding = UNKNOWN;
                    operand->file_line = asmContext->am_file_line;


                    safe_free((void**)&label_name);
                    return true;
                }
                else {
                    print_external_error(ERROR_CODE_109);
                }

            }

        }
    }
    /*the string doesn't represent a matrix  operand*/
    not_matrix_found:
    safe_free((void**)&label_name);
    return false;

    matrix_format_error:
    safe_free((void**)&label_name);
    asmContext->first_pass_error = true;
    /* return true if the operand is identified as an matrix operand even if format error found,
     * a true result signals the caller to stop checking for other operand types. */
    return true;
}


boolean try_parse_reg_operand(const char *operand_str, operand *operand, assembler_context *asmContext) {

    int reg_num;

    /*verify that all input pointers exist*/
    if (operand_str == NULL || operand == NULL) {
        print_internal_error(ERROR_CODE_25,"try_parse_reg_operand");
        return false;
    }

    /*get the reg num( -1 will be returned if register doesn't exist*/
    reg_num = get_reg_num(operand_str, asmContext);

    /*if the string is a valid register name*/
    if (reg_num != -1) {
        /*Fill operand struct values*/
        operand-> type = REGISTER_ACCESS;
        operand->operand_val.reg = reg_num;
        operand->encoding = ABSOLUTE;

        /*the string is a register operand*/
        return true;
    }
    /*the string is not a register operand*/
    return false;


}


boolean try_parse_immediate_operand(const char *operand_str, operand *operand, assembler_context *asmContext) {

    char buff[MAX_NUM_DIGITS+1];/*for found number, +1 for \0 */
    int buff_pointer = 0;/*buff index pointer*/
    char* p = NULL;/*temp pointer*/
    char* orig_address = NULL;/*to free the whole memory*/

    /*verify that all input pointers exist*/
    if (operand_str == NULL || operand == NULL) {
        print_internal_error(ERROR_CODE_25,"try_parse_immediate_operand");
        return false;
    }



    /*copy the operand string to not change the original one*/
    p = copy_string(operand_str);
    if (p == NULL) {
        return false;
    }
    /*fixup the allocated address to free the memory in the end*/
    orig_address = p;


    /*immediate operand should start with '#' sign*/
    if (*p == '#') {
        p++;
        /*Check for optional sign*/
        if (*p == '-' || *p == '+') {
            buff[buff_pointer++] = *p;
            p++;
        }

        /*a digit must appear now according to immediate operand format*/
        if (*p == '\0') {
            /*if the first char in the string or the first char
             * after the optional sign (+ or -) is not a digit*/
            print_external_error(ERROR_CODE_171);
            goto immediate_format_error;
        }

        /*verify thst only digit exist till the end of the line*/
        while (*p != '\0' && buff_pointer < sizeof(buff)) {
            if (isdigit(*p)) {
                /*insert the digit into the temp buff*/
                buff[buff_pointer++] = *p;
            }

            else if (*p == '.') {
                /*loop stopped because '.' found*/
                print_external_error(ERROR_CODE_178);
                goto immediate_format_error;
            }
            else {
                /*loop stopped because char found*/
                print_external_error(ERROR_CODE_172);
                goto immediate_format_error;

            }
            p++;
        }
        /*the previous while loop stopped bcause buffer is full and exceed the max number length*/
        if (*p != '\0') {
            print_external_error(ERROR_CODE_120);
            goto immediate_format_error;
        }



        /*Immediate operand found, the format is ok, fill the operand structure's values*/
        buff[buff_pointer] = '\0';
        operand->type = IMMEDIATE_ACCESS;
        operand->operand_val.val = atoi(buff);
        operand->encoding = ABSOLUTE;

        safe_free((void**)&orig_address);

        return true;
    }

    safe_free((void**)&orig_address);
    /*the operand is not an immediate operand*/
    return false;

    immediate_format_error:
    asmContext->first_pass_error = true;
    safe_free((void**)&orig_address);
    /* return true if the operand is identified as an immediate (#), even if format error found.
     * a true result signals the caller to stop checking for other operand types. */
    return true;
}


boolean try_parse_direct_operand(const char *operand_str, operand *operand, const assembler_context *asmContext) {

    /*verify that all input pointers exist*/
    if (operand_str == NULL || operand == NULL) {
        print_internal_error(ERROR_CODE_25,"try_parse_direct_operand");
        return false;
    }

    /*If the string doesn't represent a valid label name*/
    if (!is_name_valid(operand_str))
    {   /*the provided string is not a direct access operand*/
        return false;

}

    /*The string is a label name (may not declared at this time)*/

    /*Fill the operand struct values*/
    strcpy(operand->operand_val.label, operand_str);
    operand->type = DIRECT_ACCESS;
    operand->encoding = UNKNOWN;
    operand->file_line = asmContext->am_file_line;


return true;

}


opcode* get_opcode_info(const char *opcode_name, assembler_context *asmContext) {

    int i;/*For loop iterator*/
    opcode* opcode_info;
    const opcode* opcode_table;

    /*verify that all input pointers exist*/
    if (!opcode_name || !asmContext)  {
        print_internal_error(ERROR_CODE_25,"get_opcode_info");
        return NULL;
    }
    /*get the opcode table from context*/
    opcode_table = asmContext->opcode_table;

    /*Look for given opcode name by running over the opcode table*/
    for (i =0; i < INSTRUCTIONS_AMOUNT; i++) {

        if (strcmp(opcode_table[i].name, opcode_name) == 0) {

            /*opcode table is const, allocate memory and return a copy of an opcode struct cell*/
            opcode_info = handle_malloc(sizeof(opcode));

            /*copy the struct*/
            memcpy(opcode_info, &opcode_table[i], sizeof(opcode));

            return opcode_info;/*return a copy of the opcode struct*/
        }
    }
    /*opcode not found*/
    return NULL;
}

boolean is_opcode(const char *name, assembler_context *asmContext) {
    const opcode* opcode_table;
    int i;/*loop iterator*/

    /*verify that all input pointers exist*/
    if (!name || !asmContext) {
        print_internal_error(ERROR_CODE_25,"is_opcode");
        return false;
    }

    /*get the opcode table*/
    opcode_table = asmContext->opcode_table;

    /*check if the provided string is an opcode name by running over the table*/
    for ( i = 0; i < INSTRUCTIONS_AMOUNT; i++) {
        if (strcmp(name, opcode_table[i].name) == 0) {
            return true;
        }
    }

    /*string is not an opcode name*/
    return false;
}









