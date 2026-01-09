#ifndef SOLUTIONS_H
#define SOLUTIONS_H

uint8_t* salt(uint8_t* input, size_t input_len, size_t salting_rounds, size_t* out_len);
void cof (uint8_t* input, const size_t input_len, uint8_t* hash_box, const size_t hash_len);
void disassemble_blocks (uint8_t* input, const size_t input_len, uint32_t* blocks, size_t blocks_len);

#endif //SALTING_H