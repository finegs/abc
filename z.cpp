#include
#include
#include
#include
#include

int main(int argc, char** argv)
{
	// Socket to talk to clients
	void* context = zmq_ctx_new();
	void* responder = zmq_socket(context, ZMQ_REP);
	int rc = zmq_bind(responder, "tcp://*:5555");
	assert(rc == 0);
	

	const size_t LEN_BUFFER = 8096;
	char rcv_buff[LEN_BUFFER];
	char snd_buff[LEN_BUFFER];
	const char* SND_MSG = "REQ:%s, REP:%s";
	while(1)
	{
		memset(rcv_buff, 0, LEN_BUFFER);
		zmq_recv(responder, rcv_buff, 10, 0);
		printf(">>%s\n", rcv_buff);
		//sleep(1); // Do some 'work'
		
		memset(snd_buff, 0, LEN_BUFFER);
		snprintf(snd_buf, LEN_BUFFER, SND_MSG, rcv_buff, "ACK");
		zmq_send(responder, snd_buf, strlen(snd_buf), NULL);
	}

	return 0;
}
