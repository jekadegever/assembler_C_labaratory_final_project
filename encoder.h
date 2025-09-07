

#ifndef ENCODER_H
#define ENCODER_H

#include "instructions.h"




/**
 * @brief Encodes a single instruction (opcode + operands) into machine words.
 *
 * Encoding the given instruction word (opcode, addressing modes, ERA) and the
 * required operand words (operand data, ERA) and all operand's additional word(if needed)
 * into a 10-bit machine code. For DIRECT/MATRIX operands, writes a zero data field
 * and makes a fix-up record so the second pass can resolve and update the final label address.
 * If both operands are registers, their fields are packed into a
 * single word (9-6 bits for source operand and 5-2 for dest operand).
 * The encoded words are returned in a newly allocated buffer.
 *
 * @param[in]  opcode            Pointer to opcode metadata (mnemonic, operands count, encoding).
 * @param[in]  src_operand       Source operand descriptor (or NULL if none).
 * @param[in]  dest_operand      Destination operand descriptor (or NULL if none).
 * @param[in]  asmContext        Assembler context (IC, fix-up list, errors, etc.).
 * @param[out] encoded_buff_out  On success, points to a malloced array of encoded words.
 * @param[out] words_amount_out  Number of encoded machine words written to  encoded_buff_out.
 * @note the caller func own the out params and must be free.
 *
 * @return true on success; false on any validation/encoding error (bit-field overflow,
 *         bad operand, allocation failure, or fix-up enqueue failure).
 *
 * @note Increments a local IC while encoding; the caller should advance the global IC
 *       by words_amount_out on success.
 * @note For MATRIX operands, an extra registers word exist.
 */
boolean instruction_encoder(const opcode *opcode, const operand *src_operand, const operand *dest_operand, assembler_context *asmContext,unsigned short **encoded_buff_out, unsigned short *words_amount_out);






/**
* @brief Encodes a resolved label address + ERA into an operand word.
*
* Fills the operand data bit-field with new_label_addr param and the ERA bit-field
* with encoding param, after validating both fit their respective bit sizes.
*
* @param[in]  new_label_addr   The (relocated) address of the label to encode into the data field.
* @param[in]  encoding         ERA value to encode (ABSOLUTE/RELOCATABLE/EXTERNAL).
* @param[out] encoded_val_out  Receives the fully encoded 10-bit word.
*
* @return true on success; false if @p new_label_addr or @p encoding exceed their bit ranges.
* */
boolean encode_label_address(unsigned int new_label_addr, encoding_type encoding, unsigned int* encoded_val_out);

#endif
