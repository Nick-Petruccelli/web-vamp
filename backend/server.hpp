#pragma once
#include <stdint.h>

typedef struct __attribute__((packed)) client_message {
	uint8_t player_id;
	uint8_t key_down : 4;
	uint8_t padding: 4;
	char key;
} ClientMessage;
