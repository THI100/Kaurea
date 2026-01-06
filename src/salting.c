#include <stdint.h>
#include <stdlib.h>

#include "../include/utils.h"
#include "../include/salting.h"

// Adds salt to the input data and stores the result in output
void salt (uint8_t* input, uint16_t salting_rounds) {

	if (strlen(input) > 1) {

		for (int i = 1; i < salting_rounds; i++) {
			uint8_t temp1 =
		}

	}
	else {

	}


}