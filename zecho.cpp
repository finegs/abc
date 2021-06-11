#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <zmq.h>
#include "mzhelper.h"
#include "Util.h"

const size_t LEN_BIND_URL = 256;
const size_t LEN_BUF = 8096;

static int g_run = 1;
void *handle_response(void* responder);

int main(int argc, char* argv[])
{
	char bind_url[256] = {0};
	char bind_addr[128] = {"*"};
	char bind_protocol[8] = {"tcp"};
	int bind_port = 5555;

	for(int i = 1;i<argc;i++) {
		if(argc>i+1&&(!strcmp(argv[i],"-v") 
					|| !strcmp(argv[i],"--help") 
					|| !strcmp(argv[i],"--version"))) { 
			int major,minor,match;
			zmq_version(&major,&minor,&match);
			printf("0MQ Version:%d.%d.%d\n", major, minor, match);
			return 0;
		} 
		if(argc>i+1&&(!strcmp(argv[i],"-p") || !strcmp(argv[i],"--port"))) { 
			bind_port = atoi(argv[i+1]); i++; 
		} 
		if(argc>i+1&&(!strcmp(argv[i],"-pt") || !strcmp(argv[i],"--protocol"))) { 
			strncpy(bind_protocol, argv[i+1], strlen(argv[i+1])); i++;
		} 
		if(argc>i+1&&(!strcmp(argv[i],"-a") || !strcmp(argv[i],"--addr"))) { 
			strncpy(bind_addr, argv[i+1], strlen(argv[i+1])); i++;
		} 
	}
	snprintf(bind_url, LEN_BIND_URL, "%s://%s:%d", bind_protocol, bind_addr, bind_port);
	printf("INF: Connecting to 0MQ : %s\n", bind_url);

	// Socket to talk to clients
	void* context = zmq_ctx_new();
	void* responder = zmq_socket(context, ZMQ_REP);
	int rc = zmq_bind(responder, bind_url);

	assert(rc == 0);
	
	pthread_t thr_responder;		

	if(pthread_create(&thr_responder, NULL, handle_response, responder)) { 
		perror("ERR>>Error creating thread : responder"); return 1; 
	}

	char buf[LEN_BUF] = {0};
	while(g_run) {
		printf("CMD<<"); fflush(stdout);
		scanf("%s", buf);
		
		if(strstr(buf, "exit") || strstr(buf, "quit")) {
			printf("INFO>> exit by user\n");

			if(responder) zmq_close(responder);
			if(context) zmq_ctx_destroy(context);
			
			g_run = 0;
			break;
		}
	}

	if(pthread_join(thr_responder, NULL)) { perror("ERR>>Error joining thread:responder"); return 2; }

	if(responder) zmq_close(responder);
	if(context) zmq_ctx_destroy(context);

	return 0;
}

void *handle_response(void* responder)
{
	char rcv_buf[LEN_BUF] = {0};
	char snd_buf[LEN_BUF] = {0};
	const char* SND_MSG = "REQ:%s, REP:%s";
	while(g_run)
	{
		int rcv_size = zmq_recv(responder, rcv_buf, LEN_BUF, 0);
		if(rcv_size<0) continue;
		if(rcv_size>LEN_BUF-1) rcv_size = LEN_BUF-1;
		rcv_buf[rcv_size] = 0;

		printf("DBG:RCV>>[%d]%s\n", rcv_size, rcv_buf);
		
		snprintf(snd_buf, LEN_BUF, SND_MSG, rcv_buf, "ACK");
		zmq_send(responder, snd_buf, strlen(snd_buf), 0);
		printf("DBG:REP>>%s\n", snd_buf);

		memset(rcv_buf, 0, LEN_BUF);
		memset(snd_buf, 0, LEN_BUF);
	}
	return responder;
}
