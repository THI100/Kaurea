#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Pass the length explicitly
uint8_t* salt(uint8_t* input, size_t input_len, size_t salting_rounds) {
    if (input == NULL || input_len == 0) return;

    size_t ilp = input_len + 1;
	// Initialize temp_box
    uint8_t* temp_box = malloc(ilp);
    memcpy(temp_box, input, input_len);

    // Initialize salt_box
    uint8_t* salt_box = malloc(salting_rounds);
    size_t current_len = 0;

    if (ilp <= 2) {
       temp_box[input_len] = ((temp_box[0] + (uint8_t)salting_rounds) % 255) + 1;
    } else {
       temp_box[input_len] = 0x00;
    }

    while (current_len < salting_rounds) {
       for (size_t i = 0; i < ilp - 1 && current_len < salting_rounds; i++) {
          uint8_t tempf = temp_box[i];
          uint8_t tempc = temp_box[i + 1];
          salt_box[current_len] = ARXL(tempf, tempc);
          current_len++;
       }
    }

    // NOTE: You need to decide what to do with salt_box here!
    // For example, copy it back to input if input is large enough:
    // memcpy(input, salt_box, salting_rounds);

    free(temp_box);
    free(salt_box);

	return salted_input;
}