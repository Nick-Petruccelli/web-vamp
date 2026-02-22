#include <cstring>
#include <libwebsockets.h>
#include <queue>
#include <set>
#include <chrono>
#include <thread>
#include "game.cpp"
#include "server.hpp"

std::queue<ClientMessage> input_queue;
std::set<struct lws*> clients_connected;

auto tick_rate = std::chrono::milliseconds(1000 / 10);
auto next_tick = std::chrono::steady_clock::now();

GameState game_state = {};
unsigned char *data_buff = NULL;
unsigned char *data_to_send = NULL;
size_t data_to_send_len = 0;


static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
	switch(reason) {
	case LWS_CALLBACK_ESTABLISHED: {
		printf("CONNECTION ESTABLISHED\n");
		clients_connected.insert(wsi);
		int id = game_add_player(&game_state);
		if(id < 0) {
			printf("ERROR: game is full cant connect");
		}
		break;
	}
	case LWS_CALLBACK_RECEIVE: {
		ClientMessage cm = *(ClientMessage*)in;
		input_queue.push(cm);
		printf("MESSAGE RECIVED: player_id=%d, key_down=%d, key=%c\n", cm.player_id, cm.key_down, cm.key);
		break;
	}
	case LWS_CALLBACK_SERVER_WRITEABLE:
		lws_write(wsi, data_to_send, data_to_send_len, LWS_WRITE_BINARY);	
		break;
	case LWS_CALLBACK_CLOSED:
		printf("CONNECTION CLOSED\n");
		clients_connected.erase(wsi);
		break;
	default:
		break;
	}
	return 0;
}

static struct lws_protocols protocals[] {
	{"test-protocal", callback, 0, 10},
	{NULL, NULL, 0, 0}
};

void server_broadcast(unsigned char *data, size_t len) {
	data_to_send = data;
	data_to_send_len = len;
	for(struct lws *client : clients_connected) {
		lws_callback_on_writable(client);
	}
}

void server_broadcast_game_state(GameState *game_state) {
	size_t data_len = 1 + game_state->num_players * 8;
	if(data_buff)
		free(data_buff);
	data_buff = (unsigned char *)malloc(LWS_PRE + data_len);
	unsigned char *game_state_data = data_buff;
	memset(game_state_data, 0, data_len + LWS_PRE);
	int offset = LWS_PRE;
	game_state_data[offset] = game_state->num_players;
	offset++;
	for(int player=0; player<game_state->num_players; player++){
		memcpy(game_state_data + offset, &game_state->player_states[player].x, 4);
		offset += 4;
		memcpy(game_state_data + offset, &game_state->player_states[player].y, 4);
		offset += 4;
	}
	server_broadcast(game_state_data + LWS_PRE, data_len);
}

int server_start() {
	struct lws_context_creation_info context_info = {};
	context_info.port = 8080;
	context_info.protocols = protocals;
	game_init_state(&game_state, 10);

	struct lws_context *context = lws_create_context(&context_info);
	if(!context){
		printf("ERROR: failed to create server\n");
		return 1;
	}
	
	printf("SERVER STARTED\n");
	int i = 0;
	while(1){
		lws_service(context, 0);
		game_update(&game_state, input_queue);
		server_broadcast_game_state(&game_state);
		next_tick += tick_rate;
		std::this_thread::sleep_until(next_tick);
	}
	lws_context_destroy(context);
	return 0;
}
