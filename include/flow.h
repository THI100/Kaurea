#ifndef FLOW_H
#define FLOW_H

void apply(uint32_t* original, const size_t original_len, const size_t rounds);
void shuffle(uint8_t* hash_box, uint8_t* hash_box_copy, const size_t hash_len);

#endif //FLOW_H