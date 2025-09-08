

#include <addresses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "errors.h"
#include "instructions.h"
#include "util.h"
#include "context.h"
#include "sys_memory.h"


/**
 * @file Encoder.c
 *
 * @brief Instruction encoding module for the assembler.
 *
 * This module is responsible for translating parsed assembly instructions
 * (opcodes and operands) into their binary machine code representation.
 * It provides functionality to:
 *   - Encode the main instruction word (opcode, addressing modes, ERA).
 *   - Encode additional words for operands (immediate, direct, register, matrix).
 *   - Handle fix-up requests for label-based operands, so their addresses
 *     can be patched during the second pass.
 *   - Safely validate that encoded values fit into the defined bit-fields
 *     and report system or external errors if they do not.
 *
 * Key functions:
 *   - ::instruction_encoder — encodes a full instruction line into one or more words.
 *   - ::encode_label_address — encodes a resolved label address into an operand word.
 *
 * @note This module relies on definitions from:
 *   - Config.h (bit masks, shifts, word size, constants).
 *   - context.h (assembler context and fix-up list).
 *   - instructions.h (opcode and operand definitions).
 *   - addresses.h (fix-up table management).
 *   - errors.h / util.h (error handling and safe memory operations).
 *
 * @author Ivgeny Tokarzhevsky
 * @date 01/09/2025
 */




boolean instruction_encoder(const opcode *opcode, const operand *src_operand, const operand *dest_operand,assembler_context *asmContext, unsigned short **encoded_buff_out, unsigned short *words_amount_out) {

    /*operand address type*/
    typedef enum  {
        DESTINATION,
        SOURCE
    }operand_type;

    unsigned short machine_code_buff[MAX_ENCODED_WORDS_PER_INSTRUCTION];
    unsigned short machine_word = 0;
    unsigned short words_count = 0;
    unsigned short operands_amount;
    unsigned short mat_regs_machine_word =0;
    unsigned short i = 0;
    operand* op_temp = NULL;
    const  operand* operand;
    operand_type op_type;
    unsigned int IC;


    /*verify that all necessary input variables exist*/
    if (!asmContext ||!opcode ||!encoded_buff_out || !words_amount_out) {
        print_internal_error(ERROR_CODE_25,"encode_instruction_line");
        return false;
    }


    /*set input values*/
    IC = asmContext->IC;
    operands_amount = opcode->operands_amount;




    /*- - - - - - - - - - - - - - - - - - - - - - ENCODE INSTRUCTION WORD - - - - - - - - - - - - - - - - -*/


    /*- - - opcode bits field - - -*/

    /*reset to zero the opcode bits field*/
    machine_word = machine_word & (~OPCODE_BITS);

    /*validate that the value fits within the bit-field size (internal check)*/
    if (opcode->opcode > U_MAX_NUM(OPCODE_BITS) || opcode->opcode < U_MIN_NUM) {
        print_system_error(ERROR_CODE_2);
          return false;
    }
    /*insert the encoded opcode bits into the machine word*/
    machine_word = machine_word | (opcode->opcode << OPCODE_BITS_SHIFT);




     /*- - - source addressing mode bits field - - - - - - -*/

    /*reset to zero the source addressing mode bits field*/
    machine_word = machine_word & ~SRC_ADDR_MODE_BITS_MASK;
    if (src_operand != NULL) {
        /*validate that the value fits within the bit-field size (internal check)*/
        if (src_operand->type > U_MAX_NUM(SRC_ADDR_MODE_BITS) || src_operand->type < U_MIN_NUM) {
            print_system_error(ERROR_CODE_3);
            return 0;
        }
        /*insert the encoded source AM bits*/
        machine_word = machine_word | (src_operand->type << SRC_ADDR_MODE_BITS_SHIFT);
    }/*if Source operand NULL, leave it with reset 0 bits */




     /*- - - destination addressing mode bits field - - - - - - -*/

    /*reset to zero the dest addressing mode bits field*/
    machine_word = machine_word & ~DEST_ADDR_MODE_BITS_MASK;

    if (dest_operand != NULL) {
        /*validate that the value fits within the bit-field size (internal check)*/
        if (dest_operand->type > U_MAX_NUM(DEST_ADDR_MODE_BITS) || dest_operand->type < U_MIN_NUM) {
            print_system_error(ERROR_CODE_4);
            return 0;
        }
        /*insert the encoded destination AM bits*/
        machine_word = machine_word | (dest_operand->type << DEST_ADDR_MODE_BITS_SHIFT);
    }/*if dest operand NULL, leave it with bits 0*/




     /*- - - - ERA bits field - - - - -*/

    /*reset to zero the ERA bits field*/
    machine_word = machine_word & ~E_R_A_BITS_MASK;

    /*validate that the value fits within the bit-field size (internal check)*/
    if (opcode->encoding > U_MAX_NUM(E_R_A_BITS) || opcode->encoding < U_MIN_NUM) {
        print_system_error(ERROR_CODE_5);
        return false;
    }
    /*insert ERA bits to the machine word*/
    machine_word = (machine_word | (opcode->encoding << E_R_A_BITS_SHIFT));

    /*insert the fully encoded word into the buffer*/
    machine_code_buff[words_count++] = machine_word & WORD_BIT_MASK;
    /*increment the local IC:*/
    IC++;
    /*reset the machine word to zero for the next encoding*/
    machine_word =0;




    /*- - - - - - - - - - - - - - - - - - - - - - - ENCODE OPERANDS WORDS - - - - - - - - - - - - - - - - -*/

    /* - - - - - - operand data bits field- - - - - - - -*/

     /*- - - ONE OPERAND - - -*/
    while (i < operands_amount) {

        /*set the operand that will be encoded*/
        if (i == 1 || operands_amount == 1) {
            operand = dest_operand;
            op_type = DESTINATION;
        }
        else {
            operand = src_operand;
            op_type = SOURCE;
        }
        /*reset to zero the  word's data bits field*/
        machine_word = machine_word & ~OPERAND_DATA_BITS_MASK;

        /*if the operand is necessary and doesn't exist, stop the prog and print error*/
        if (operand == NULL) {
            print_internal_error(ERROR_CODE_25,"encode_instruction_line");
            return false;
        }
        /*encode dest operand by type*/
        switch (operand->type) {

            /*immediate access operand*/
            case IMMEDIATE_ACCESS: {
                /*validate that the value in the acceptable range and fits within the bit-field size */
                if (operand->operand_val.val > MAX_NUM(OPERAND_DATA_BITS) || operand->operand_val.val < MIN_NUM(OPERAND_DATA_BITS)) {
                    print_external_error(ERROR_CODE_143);
                    return 0;
                }
                /*insert immediate operand's encoded value bits into the machine word*/
                machine_word = machine_word | (operand->operand_val.val << OPERAND_DATA_BITS_SHIFT);
                break;
            }
            /*register access operand*/
            case REGISTER_ACCESS: {

                switch (op_type) {
                    /*destination register*/
                    case DESTINATION: {
                        /*validate that the value in the acceptable range and fits within the bit-field size */
                        if (operand->operand_val.reg > MAX_NUM(OPERAND_DATA_DEST_REG_BITS) || operand->operand_val.reg < MIN_NUM(OPERAND_DATA_DEST_REG_BITS)) {
                            print_system_error(ERROR_CODE_6);
                        }
                        /*insert the dest register encoded value into the word*/
                        machine_word = machine_word | (operand->operand_val.reg << OPERAND_DATA_DEST_REG_SHIFT);
                        break;
                    }
                    /*source register*/
                    case SOURCE: {
                        /*validate that the value in the acceptable range and fits within the bit-field size */
                        if (operand->operand_val.reg > MAX_NUM(OPERAND_DATA_SRC_REG_BITS) || operand->operand_val.reg < MIN_NUM(OPERAND_DATA_SRC_REG_BITS)) {
                            print_system_error(ERROR_CODE_7);
                        }
                        /*insert the source register encoded value into the word*/
                        machine_word = machine_word | (operand->operand_val.reg << OPERAND_DATA_SRC_REG_SHIFT);
                        break;
                    }
                }
                break;
            }

            /*direct access operand*/
            case DIRECT_ACCESS: {
                /* The operand's data bit-field in the machine word is already cleared (set to 0).
                 *Leave it as zero for now, and record the referenced label in the fixUp table.
                 *During the second pass, once all label addresses are resolved,
                 *the correct encoded value will be inserted. */

                /*allocate memory and copy the operand for fixup table*/
                op_temp = handle_malloc(sizeof(*operand));

                memcpy(op_temp, operand, sizeof(*operand));

                /*add the label to fixup table with the label name and the address to fix up*/
                if (!add_addr_update_request(IC, op_temp, &asmContext->address_update_requests)){
                    safe_free((void**)&op_temp);
                    return false;
                }

                break;
            }


            case MATRIX_ACCESS: {
                /*- - - - - - - - -  matrix first machine word (label) - - - -*/

                /* The operand's data bit-field in the machine word is already cleared (set to 0).
                *Leave it as zero for now, and record the referenced label in the fixUp table.
                *During the second pass, once all label addresses are resolved,
                *the correct encoded value will be inserted. */

                /*allocate memory and copy the operand for fixup table*/
                op_temp = handle_malloc(sizeof(*operand));

                memcpy(op_temp, operand, sizeof(*operand));

                /*add the label to fixup table with the label name and the address to fix up*/
                if (!add_addr_update_request(IC, op_temp, &asmContext->address_update_requests)){
                    safe_free((void**)&op_temp);
                    return false;
                }


                /*- - - - - - - - - matrix second machine word (the registers word -> matrix's index) - - - */

                /* - - - - - - encode matrix register additional word - - - - - - */

                /* - - - operand data bits field - - - -*/

                /*reset to zero the operand additional word data bits field*/
                mat_regs_machine_word = mat_regs_machine_word & ~OPERAND_DATA_BITS_MASK;

                /*validate that the value in the acceptable range and fits within the bit-field size */
                if (operand->operand_val.matrix.reg_1 > MAX_NUM(OPERAND_DATA_SRC_REG_BITS) || operand->operand_val.matrix.reg_1 < MIN_NUM(OPERAND_DATA_SRC_REG_BITS) || operand->operand_val.matrix.reg_2 > MAX_NUM(OPERAND_DATA_DEST_REG_BITS) || operand->operand_val.matrix.reg_2 < MIN_NUM(OPERAND_DATA_DEST_REG_BITS)) {
                    print_system_error(ERROR_CODE_8);
                    return false;
                }

                /*encode the source register value and insert into the word */
                mat_regs_machine_word = mat_regs_machine_word | (operand->operand_val.matrix.reg_1 << OPERAND_DATA_SRC_REG_SHIFT);

                /*encode the destination register value and insert into the word */
                mat_regs_machine_word = mat_regs_machine_word | (operand->operand_val.matrix.reg_2 << OPERAND_DATA_DEST_REG_SHIFT);

                break;
            }


            default:
                /*unknown operand type*/
                print_internal_error(ERROR_CODE_27,"instruction_encoder");
                    break;

        }

        /*- - - - - -  - operand ERA bits field - - - - -*/

        /*reset to zero the operand additional word ERA bits field*/
        machine_word = machine_word & (~E_R_A_BITS_MASK);

        /*validate that the value in the acceptable range and fits within the bit-field size */
        if (operand->encoding > U_MAX_NUM(E_R_A_BITS) || operand->encoding < U_MIN_NUM) {
            print_system_error(ERROR_CODE_9);
            return false;
        }

        /*insert ERA bits*/
        machine_word = machine_word | (operand->encoding << E_R_A_BITS_SHIFT);


        /*insert operand word to buff*/
        machine_code_buff[words_count++] = machine_word & WORD_BIT_MASK;
        /*increment the local IC*/
        IC++;

        /*if there is 2 operands and both are registers, merge the both register encoded words into one word*/
        if (i ==1 && operands_amount == 2) {
            if (src_operand != NULL && dest_operand != NULL) {/*unnecessary, to ignore warning*/
                if (src_operand->type==REGISTER_ACCESS && dest_operand->type==REGISTER_ACCESS) {
                    words_count--;/*decrement the machine words count*/
                    machine_code_buff[words_count-1] = machine_code_buff[words_count] | machine_code_buff[words_count-1];

                    /*decrement the local IC*/
                    IC--;
                }
            }
        }

        /*-only if the operand is matrix index, encode the ERA bits field of the mat's index registers word-*/
        if (operand->type == MATRIX_ACCESS) {

            /*reset to zero the mat second additional(registers) word ERA bits field*/
            mat_regs_machine_word = mat_regs_machine_word & (~E_R_A_BITS_MASK);

            /*insert the era bits*/
            mat_regs_machine_word = mat_regs_machine_word | (operand->operand_val.matrix.reg_encoding << E_R_A_BITS_SHIFT);

            /*insert mat's operand second word to buff*/
            machine_code_buff[words_count++] = mat_regs_machine_word & WORD_BIT_MASK;
            /*increment the local IC*/
            IC++;
        }
        i++;
    }

    /* - - - - - - - - - - - - - - - - -  RETURN THE ENCODED BUFFER TO CALLER - - - - - - - - - - - - - - - - - */

    /*allocate memory for encoded words array*/
    *encoded_buff_out = handle_malloc(words_count * sizeof(unsigned short));


    /*copy the encoded words into the new add*/
    memmove(*encoded_buff_out, machine_code_buff, words_count * sizeof(unsigned short));
    /*point the out pointer to the newly allocated memory with encoded words*/
    *words_amount_out = words_count;


    return true;
}

boolean encode_label_address(unsigned int new_label_addr, encoding_type encoding, unsigned int* encoded_val_out){

    unsigned short encoded_word = 0;

    /*check if label_address in bits field range */
    if (new_label_addr > U_MAX_NUM(OPERAND_DATA_BITS)) {
        print_system_error(ERROR_CODE_10);
        return false;
    }

    /*check if encoding val is in bits field range*/
    if (encoding > U_MAX_NUM(E_R_A_BITS)) {
        print_system_error(ERROR_CODE_9);
        return false;
    }


    /*- - - - - - - - - DATA BITS - - - - - - - - -*/

    /*reset to zero the operand additional word data bits field*/
    encoded_word = encoded_word & ~OPERAND_DATA_BITS_MASK;

    /*insert data bits -> the label new address*/
    encoded_word=encoded_word | (new_label_addr << OPERAND_DATA_BITS_SHIFT);


    /*- - - - - - - - - ERA BITS - - - - - - - - -*/

    /*reset to zero the operand additional word ERA bits field*/
    encoded_word = encoded_word & ~E_R_A_BITS_MASK;

    /*insert ERA bits*/
    encoded_word = encoded_word | (encoding << E_R_A_BITS_SHIFT);


    /*- - - - - - - SET OUT RESULT - - - - - - -*/
    *encoded_val_out = encoded_word;
    return true;
}














