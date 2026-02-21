#include <libwebsockets.h>
#include <queue>
#include "game.cpp"
#include "server.hpp"

std::queue<ClientMessage> input_queue;

static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
	switch(reason) {
	case LWS_CALLBACK_ESTABLISHED:
		printf("CONNECTION ESTABLISHED\n");
		break;
	case LWS_CALLBACK_RECEIVE: {
		ClientMessage cm = *(ClientMessage*)in;
		input_queue.push(cm);
		printf("MESSAGE RECIVED: player_id=%d, key_down=%d, key=%c\n", cm.player_id, cm.key_down, cm.key);
		break;
	}
	case LWS_CALLBACK_CLOSED:
		printf("CONNECTION CLOSED\n");
		break;
	default:
		break;
	}
	return 0;
}

static struct lws_protocols protocals[] {
	{"test-protocal", callback, 0, 256},
	{NULL, NULL, 0, 0}
};


int server_start() {
	struct lws_context_creation_info context_info = {};
	context_info.port = 8080;
	context_info.protocols = protocals;

	struct lws_context *context = lws_create_context(&context_info);
	if(!context){
		printf("ERROR: failed to create server\n");
		return 1;
	}
	
	printf("SERVER STARTED\n");
	int i = 0;
	while(1){
		lws_service(context, 0);
		game_update_state(input_queue);
	}
	lws_context_destroy(context);
	return 0;
}
