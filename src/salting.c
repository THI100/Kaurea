#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint8_t* salt(uint8_t* input, size_t input_len, size_t salting_rounds) {
   if (input == NULL || input_len == 0) return NULL;

   size_t ilp = input_len + 1;
   uint8_t* temp_box = malloc(ilp);
   memcpy(temp_box, input, input_len);
    
   // Padding/Adjustment
   temp_box[input_len] = (ilp <= 2) ? (((temp_box[0] + (uint8_t)salting_rounds) % 255) + 1) : 0x00;

   uint8_t* salt_box = malloc(salting_rounds);
   size_t current_len = 0;

   // Generate Salt via ARX
   while (current_len < salting_rounds) {
      for (size_t i = 0; i < ilp - 1 && current_len < salting_rounds; i++) {
         salt_box[current_len++] = ARXL(temp_box[i], temp_box[i + 1]);
      }
   }

   // Allocate maximum possible size
   uint8_t* salted_input = malloc(salting_rounds + ilp);
   size_t actual_salted_len = 0;

   // Join while ignoring 0x00
   for (size_t i = 0; i < ilp; i++) {
      if (temp_box[i] != 0x00) {
         salted_input[actual_salted_len++] = temp_box[i];
      }
   }
   for (size_t i = 0; i < salting_rounds; i++) {
      if (salt_box[i] != 0x00) {
         salted_input[actual_salted_len++] = salt_box[i];
      }
   }

   // Simple Shuffle using actual_salted_len
   if (actual_salted_len > 0) {
      size_t seed = input_len * 32123u + actual_salted_len;
      for (size_t i = 0; i < actual_salted_len; i++) {
         seed = seed * 1103515245u + 12345u;
         size_t j = seed % (i + 1);

         uint8_t t = salted_input[i];
         salted_input[i] = salted_input[j];
         salted_input[j] = t;
      }
   }

   free(temp_box);
   free(salt_box);

   // Note: The caller needs to know the new length.
   return salted_input; 
}