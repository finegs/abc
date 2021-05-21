#include <iostream>
#include <cstring>
#include <zmq.h>
#include <cstdlib>
#include <assert.h>

static int g_run = 1;

int main(int argc, char** argv)
{
	// Socket to talk to clients
	void* context = zmq_ctx_new();
	void* responder = zmq_socket(context, ZMQ_REP);
	int rc = zmq_bind(responder, "tcp://*:5555");
	assert(rc == 0);
	

	const size_t LEN_BUFFER = 8096;
	char rcv_buf[LEN_BUFFER];
	char snd_buf[LEN_BUFFER];
	const char* SND_MSG = "REQ:%s, REP:%s";
	size_t rcv_len_limit = 10;
	while(g_run)
	{
		memset(rcv_buf, 0, LEN_BUFFER);
		zmq_recv(responder, rcv_buf, rcv_len_limit, 0);

		if (strlen(rcv_buf) > rcv_len_limit) {
			fprintf(stderr, "Warning : rcv_buf is overflow. len=%lld\n", rcv_len_limit);
		}

		printf(">>%s\n", rcv_buf);

		if(strstr(rcv_buf, "-shutdown")) {
			zmq_send(responder, "server going to shutdown", 14, 0);
			g_run = 0;
			continue;
		}
		//sleep(1); // Do some 'work'
		
		memset(snd_buf, 0, LEN_BUFFER);
		snprintf(snd_buf, LEN_BUFFER, SND_MSG, rcv_buf, "ACK");
		zmq_send(responder, snd_buf, strlen(snd_buf), 0);
	}

	return 0;
}
