
#include "addresses.h"
#include <stdio.h>
#include "data_memory.h"
#include "encoder.h"
#include "errors.h"
#include "instructions.h"
#include "instruction_memory.h"
#include "labels.h"
#include "externals.h"
#include "sys_memory.h"



/**
 * @file addresses.c
 * @brief Handles memory relocation and deferred label resolution in the assembler.
 *
 * This module is responsible for adjusting addresses of instructions,
 * data, and labels after the final instruction and data memory size are determined.
 * It also manages address update requests generated during the first pass by the encoder
 * for operands that uses unresolved labels. These requests are processed
 * in the second pass to patch instruction memory with the correct relocated
 * label addresses.
 *
 * Main responsibilities:
 *  - Maintain and process a list of address update requests.
 *  - Relocate instruction and data memory by applying memory offsets.
 *  - Update label addresses according to their type (CODE or DATA).
 *  - Handle external label usages for generating `.ext` files.
 *
 * The functions in this file ensure that the final machine code has
 * consistent and correct addressing after relocation, enabling
 * the assembler to produce valid output files.
 *
 * @author Ivgeny Tokarzhevsky
 * @date 01/09/2025
 */



boolean add_addr_update_request(unsigned int IC_addr, operand *operand, address_update_request_ptr *request_list) {

    address_update_request_ptr temp = NULL;
    address_update_request_ptr new_addr_update_request = NULL;

    /*verify that all input pointers exist*/
    if (!operand || !request_list){print_internal_error(ERROR_CODE_25, "add_addr_update_request"); return false;}

    /*allocate memory for new request*/
    new_addr_update_request = (address_update_request_ptr)handle_malloc(sizeof(address_update_request));


    /*set the request node values*/
    new_addr_update_request->operand = operand;
    new_addr_update_request->address = IC_addr;
    new_addr_update_request->next = NULL;

    /*insert the new request in end of the list*/
    if(*request_list == NULL) {
        *request_list = new_addr_update_request;
        return true;
    }
    else {
        temp = *request_list;
        while(temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_addr_update_request;
        return true;}

}


void print_addr_update_requests(address_update_request_ptr request_list) {

    while (request_list != NULL) {

        /*access to the correct operand union field*/
        if (request_list->operand->type == MATRIX_ACCESS) {
            printf("\nrequest to update label addr: %s    ", request_list->operand->operand_val.matrix.label);
        }
        else if (request_list->operand->type == DIRECT_ACCESS) {
            printf("\nrequest to update label addr: %s    ", request_list->operand->operand_val.label);
        }
        else {
            return;
        }
        /*print the instruction memory address that need an address update*/
        printf("in instruction memory address: %d\n", request_list->address);
        request_list = request_list->next;
    }
}

boolean process_addr_update_requests(assembler_context *asmContext) {

    address_update_request_ptr address_update_request;
    instruction_ptr instructions_memory;
    const char *label_name;
    unsigned int new_label_addr;
    unsigned int encoded_label_addr;
    instruction_ptr instruction_node;
    unsigned int inst_addr_to_update;
    label_ptr label;

    /*verify that context exist*/
    if (!asmContext) {
        print_internal_error(ERROR_CODE_25, "update_relocated_addresses");
        return false;
    }

    /*get the data and instruction memories*/
    address_update_request = asmContext->address_update_requests;
    instructions_memory = asmContext->instruction_memory;

    /*update the request addresses with the new instruction addresses(after instruction memory relocated) */
    update_request_list_addresses(asmContext->address_update_requests);


    while (address_update_request != NULL) {
        if (!address_update_request->operand ) { print_internal_error(ERROR_CODE_30,"update_relocated_addresses"); return false; }

        /*get the label name that used in the instruction line */
        switch (address_update_request->operand->type) {

            case MATRIX_ACCESS: {
                label_name = address_update_request->operand->operand_val.matrix.label;
                break;
            }

            case DIRECT_ACCESS: {
                label_name = address_update_request->operand->operand_val.label;
                break;
            }

            default: {
                print_internal_error(ERROR_CODE_28, "update_relocated_addresses");
                return false;
            }

        }

        /*get the instruction memory address that need an update of the final label address*/
        inst_addr_to_update = address_update_request->address;

        /*verify the instruction memory not NULL*/
        if (!instructions_memory) {
            print_internal_error(ERROR_CODE_25, "update_relocated_addresses");
            return false;
        }

        /*get the instruction memory head*/
        instruction_node = instructions_memory;

        /*find the instruction memory node that must be updated with the resolved label address.*/
        while (instruction_node && instruction_node->address != inst_addr_to_update) {
            instruction_node = instruction_node->next;
        }
        /*if the instruction address in the request doesn't appear in the instruction memory
         * developing bug exist, stop and print error */
        if (!instruction_node){ print_internal_error(ERROR_CODE_29,"update_relocated_address"); return false;} /*instruction_node not found */

        /*verify that the label defined, otherwise print error
         * according to attempt using undefined label as operand */
        if (is_label_defined(label_name, asmContext->labels)) {


            /*get the label that used as operand in the instruction*/
            label = get_label(label_name, asmContext->labels);
            /*Get the label new final address*/
            new_label_addr = get_label_address(label_name, asmContext->labels);

            /* - - - - -update the label encoding mode - - - - -*/
            if (label->definition == EXTERN) {
                address_update_request->operand->encoding = EXTERNAL;
                add_external_usage(label_name, inst_addr_to_update,(external_ptr*)&asmContext->external_labels);
            }
            else
            address_update_request->operand->encoding = RELOCATABLE;
        }

        else {
            /*the label not found, an attempt to use undeclared label as operand*/
            asmContext->second_pass_error_line = address_update_request->operand->file_line;
            print_external_error(ERROR_CODE_146);
            return false;
        }

        /*encode the instruction operand with the resolved label address*/
        if (!encode_label_address(new_label_addr, address_update_request->operand->encoding, &encoded_label_addr)) {
            return false;
        }
        /*update the encoded operand value in the instruction memory*/
        instruction_node->value = (int)encoded_label_addr;



        /*get the next request*/
        address_update_request = address_update_request->next;
    }

    return true;
}


boolean update_labels_addresses(assembler_context *asmContext) {

    label_ptr temp;

    /*verify that the assembler context exist*/
    if (!asmContext) {
        print_internal_error(ERROR_CODE_25, "relocate_labels_addresses"); return false;
    }
    /*get the labels list*/
    temp = asmContext->labels;

    /*update the label in the next way :
     * code label -> add the memory offset to the label address.
     * data label -> add the memory offset and the Instruction count value to the label address
     */
    while(temp != NULL) {
        if (temp->type == DATA) {
            temp->address += (asmContext->IC + MEMORY_ADDRESS_OFFSET);
        }
        else if (temp->type == CODE) {
            temp->address += MEMORY_ADDRESS_OFFSET;
        }
        temp = temp->next;
    }
    return true;
}

boolean update_data_addresses(assembler_context *asm_context) {
    data_ptr temp;

    /*verify that the assembler context input param received*/
    if (!asm_context) {print_internal_error(ERROR_CODE_25, "update_data_addresses"); return false;}

    /*get the data memory*/
    temp = asm_context->data_memory;

    /* add to data memory the last instruction counter, then add the memory offset*/
    while(temp != NULL) {
        temp->address += (asm_context->IC+ MEMORY_ADDRESS_OFFSET);
        temp = temp->next;
    }
    /* now, the data memory located after the instruction memory */
    return true;
}

boolean update_instruction_addresses(assembler_context *asmContext) {

    instruction_ptr temp;

    /*verify that the instruction memory is  not null*/
    if (!asmContext) {print_internal_error(ERROR_CODE_25, "update_instruction_addresses"); return false;}

    temp = asmContext->instruction_memory;
    /*set instruction memory  after the memory offset address*/
    while(temp != NULL) {

        temp->address += MEMORY_ADDRESS_OFFSET;

        temp = temp->next;
    }

    return true;
}

void update_request_list_addresses(address_update_request_ptr request_list) {

    /*update the instruction address in the request list with the new relocated instruction address
     * by adding the memory offset*/
    while (request_list != NULL) {
        request_list->address += MEMORY_ADDRESS_OFFSET;
        request_list = request_list->next;
    }
}

void free_addr_update_req_list(address_update_request_ptr *request_list) {

    address_update_request_ptr temp = NULL;

    /*free the address update request list*/
    while (*request_list != NULL) {
        temp = *request_list;
        *request_list = (*request_list)->next;

        safe_free((void**)&temp->operand);
        safe_free((void**)&temp);
    }
}
