#include <queue>
#include "server.hpp"

int x = 0;
int y = 0;

void game_update_state(std::queue<ClientMessage> input_queue) {
	while(!input_queue.empty()) {
		ClientMessage cm = input_queue.front();
		input_queue.pop();
		switch(cm.key) {
			case 'w':
				y -= 5;
				break;
			case 'a':
				x -= 5;
				break;
			case 's':
				y += 5;
				break;
			case 'd':
				x += 5;
				break;
		}
	}
	printf("Player: x=%d, y=%d\n", x, y);
}
