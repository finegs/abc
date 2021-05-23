#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <zmq.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <time.h>
#endif

const size_t LEN_BUF = 8096;
const size_t  LEN_SVR_URL = 1024;

static int g_run = 1;

void *handle_recv(void* requester);
void *handle_message(void* requester);


int main(int argc, char **argv)
{

	char svr_protocol[8] = {"tcp"};
	char svr_addr[256] = {"localhost"};
	int svr_port = 5555;	
	char svr_url[LEN_SVR_URL] = {0};

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
			svr_port = atoi(argv[i+1]); i++; 
		} 
		if(argc>i+1&&(!strcmp(argv[i],"-pt") || !strcmp(argv[i],"--protocol"))) { 
			strncpy(svr_protocol, argv[i+1], strlen(argv[i+1])); i++;
		} 
		if(argc>i+1&&(!strcmp(argv[i],"-a") || !strcmp(argv[i],"--addr"))) { 
			strncpy(svr_addr, argv[i+1], strlen(argv[i+1])); i++;
		} 
	}

	snprintf(svr_url, LEN_SVR_URL, "%s://%s:%d", svr_protocol, svr_addr, svr_port);

	printf("INFO>>Connecting to echo server:%s \n", svr_url);
	void* context = zmq_ctx_new();
	void* requester = zmq_socket(context, ZMQ_REQ);
	zmq_connect(requester, svr_url);

	system("pause");

	pthread_t sender;
//	pthread_t receiver;		

	if(pthread_create(&sender, NULL, handle_message, requester)) { 
		perror("ERR>>Error creating thread : sender"); return 1; 
	}

//	if(pthread_create(&receiver, NULL, handle_recv, requester)) { 
//		perror("ERR>>Error creating thread : receiver"); return 1; 
//	}

	while(g_run) 
	{
		Sleep(1);
	}

	if(pthread_join(sender, NULL)) { perror("ERR>>Error joining thread:sender"); return 2; }

//	if(pthread_join(receiver, NULL)) { perror("ERR>>Error joining thread:receiver"); return 2; }

	if(requester) zmq_close(requester);
	if(context) zmq_ctx_destroy(context);
	
	return 0;
}

void *handle_recv(void* requester)
{
	char rcv_buf[LEN_BUF] = {0};
	while(g_run)
	{
		size_t rcv_size = zmq_recv(requester, rcv_buf, LEN_BUF, 0);
		if(rcv_size<0) return NULL;
		if(rcv_size>LEN_BUF-1) rcv_size = LEN_BUF-1;
		rcv_buf[rcv_size] = 0;
		printf("RCV>>[%llu]%s\n", rcv_size, rcv_buf);
		memset(rcv_buf, 0, rcv_size);
	}
	return NULL;
}

void *handle_message(void* requester) 
{
	char buf[LEN_BUF] = {0};
	char rcv_buf[LEN_BUF] = {0};
	while(g_run)
	{
		printf("USR<<"); fflush(stdout);
		scanf("%[^\n]%*c", buf);
		
		if(strstr(buf, "-exit") || strstr(buf, "-shutdown")) {
			printf("INFO>> exit by user\n");
			g_run = 0;

			if(strstr(buf, "-shutdown")) {
				zmq_send(requester, "-shutdown", strlen("-shutdown"), 0);
			}

			if(requester) zmq_close(requester);
			break;
		}
		if(strlen(buf)<1) {
			printf("INFO>> empty string with '\\n'\n");
			fflush(stdin);
			continue;
		}

		zmq_send(requester, buf, strlen(buf), 0);

		size_t rcv_size = zmq_recv(requester, rcv_buf, LEN_BUF, 0);
		if(rcv_size<0) { 
			// TODO : reconnect to server
			return NULL; g_run = 0;
		}

		if(rcv_size>LEN_BUF-1) rcv_size = LEN_BUF-1;
		rcv_buf[rcv_size] = 0;
		printf("RCV>>[%llu]%s\n", rcv_size, rcv_buf);

		memset(buf, 0, strlen(buf));
		memset(rcv_buf, 0, rcv_size);

	}
	return NULL;
}

