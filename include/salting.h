#ifndef SALTING_H
#define SALTING_H

uint8_t* salt(uint8_t* input, size_t input_len, size_t salting_rounds, size_t* out_len);

#endif //SALTING_H