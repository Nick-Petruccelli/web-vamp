#include <queue>
#include <stdlib.h>
#include <cstring>
#include "server.hpp"

int x = 0;
int y = 0;

typedef struct PlayerState {
	int x;
	int y;
} PlayerState;

typedef struct GameState {
	uint8_t num_players = 0;
	PlayerState *player_states;
} GameState;

GameState *game_init_state(GameState *game_state, int num_players) {
	game_state->player_states = (PlayerState*)malloc(sizeof(PlayerState) * num_players);
	memset(game_state->player_states, 0, sizeof(PlayerState) * num_players);
	game_state->num_players = num_players;
	return game_state;
}

void game_process_input(GameState *game_state, ClientMessage cm) {
	printf("PlayerId: %d\n", cm.player_id);
	switch(cm.key) {
	case 'w':
		game_state->player_states[cm.player_id].y -= 5;
		break;
	case 'a':
		game_state->player_states[cm.player_id].x -= 5;
		break;
	case 's':
		game_state->player_states[cm.player_id].y += 5;
		break;
	case 'd':
		game_state->player_states[cm.player_id].x += 5;
		break;
	}
}

void game_update_state(GameState *game_state, std::queue<ClientMessage> input_queue) {
	while(!input_queue.empty()) {
		ClientMessage cm = input_queue.front();
		input_queue.pop();
		game_process_input(game_state, cm);
	}
}

