#include<stdlib.h>
#include<string.h>
#include <zmq.h>

static char* zmq_recv_buf(void* socket, char* buf, size_t buf_size) {
	int size = zmq_recv(socket, buf, buf_size, 0);
	if(size < 0) return NULL;
	if(size>buf_size-1) size = buf_size-1;
	buf[size] = 0;
	return buf;
}


