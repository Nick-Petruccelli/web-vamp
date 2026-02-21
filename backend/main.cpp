#include "server.cpp"

int main() {
	int res = server_start();	
	if(res) {
		return 1;
	}
}
