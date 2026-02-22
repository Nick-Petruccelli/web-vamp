#include <queue>
#include <stdlib.h>
#include <cstring>
#include "server.hpp"
#include <stdio.h>

int x = 0;
int y = 0;

typedef struct PlayerInputState {
	bool move_foward;
	bool move_backward;
	bool move_left;
	bool move_right;

} PlayerInputState;

typedef struct PlayerState {
	float x;
	float y;
	PlayerInputState input_state;
} PlayerState;

typedef struct GameState {
	uint8_t num_players = 0;
	uint8_t max_players = 255;
	PlayerState *player_states;
} GameState;

GameState *game_init_state(GameState *game_state, int max_players) {
	game_state->player_states = (PlayerState*)malloc(sizeof(PlayerState) * max_players);
	memset(game_state->player_states, 0, sizeof(PlayerState) * max_players);
	game_state->num_players = 0;
	game_state->max_players = max_players;
	return game_state;
}

int game_add_player(GameState *game_state) {
	if(game_state->num_players == game_state->max_players) {
		return -1;
	}
	memset(&game_state->player_states[game_state->num_players], 0, sizeof(PlayerState));
	game_state->num_players += 1;
	return game_state->num_players - 1;
}

void game_process_input(GameState *game_state, ClientMessage cm) {
	switch(cm.key) {
	case 'w':
		game_state->player_states[cm.player_id].input_state.move_foward = cm.key_down;
		break;
	case 'a':
		game_state->player_states[cm.player_id].input_state.move_left = cm.key_down;
		break;
	case 's':
		game_state->player_states[cm.player_id].input_state.move_backward = cm.key_down;
		break;
	case 'd':
		game_state->player_states[cm.player_id].input_state.move_right = cm.key_down;
		break;
	}
}

void game_update_state(GameState *game_state) {
	for(int i=0; i<game_state->num_players; i++) {
		PlayerState *player = &game_state->player_states[i];
		if(player->input_state.move_foward) {
			player->y -= 5;
		}
		if(player->input_state.move_backward) {
			player->y += 5;
		}
		if(player->input_state.move_left) {
			player->x -= 5;
		}
		if(player->input_state.move_right) {
			player->x += 5;
		}
		printf("Player:%d x=%f, y=%f\n", i, player->x, player->y);
	}
}

void game_update(GameState *game_state, std::queue<ClientMessage> input_queue) {
	while(!input_queue.empty()) {
		ClientMessage cm = input_queue.front();
		input_queue.pop();
		game_process_input(game_state, cm);
	}
	game_update_state(game_state);
}

