/*
 * main.c
 *
 *  Created on: 29 de jun de 2019
 *      Author: jack
 */

#include "WebSocket.h"
#include <pthread.h>

#include "ValuePair.h"

typedef struct {
	int id;
	pthread_t tid;
	WebSocketConnection *conn;
} WScon;

void* inc_x(void *x_void_ptr) {
	WScon *wsc = (WScon*) x_void_ptr;
	WebSocketConnection *conn = wsc->conn;

	while (true) {
		WebSocketConnection_read(conn);
		if (!conn->open) {
			break;
		}
		//system("xdotool key space");
		printf("S%i I%i %s\n", wsc->conn->socketId, wsc->id, conn->response);
	}
	printf("Fechando conexão %i...\n", wsc->id);
	WebSocketConnection_free(conn);
	printf("Fechada %i\n", wsc->id);
	//free(wsc);

	return NULL;

}

int mmmain(void) {
	/*HttpHeaderSet h;
	HttpHeaderSet_init(&h);
	HttpHeaderSet_add(&h,"Foo","Bar");
	HttpHeaderSet_clear(&h);*/
	return 0;
}

int mmain(void) {
	WebSocket ws;
	ws.port = 2345;
	WebSocket_open(&ws);
	printf("Porta %i aberta\n", ws.port);

	int i = 0;
	while (i < 100) {
		printf("++++++++++++++++\n");
		printf("Tentando conexão %i\n", i);
		WebSocketConnection *conn = WebSocket_accept(&ws, NULL);
		printf("++++++++++++++++\n");
		if (conn != NULL) {
			if (conn->isWebSocket) {
				printf("////////////////\n");
				WScon *c = malloc(sizeof(WScon));
				c->conn = conn;
				c->id = i;
				printf("Conexão %i %i\n", i, conn->socketId);
				pthread_t thread;
				pthread_create(&thread, NULL, inc_x, (void*) c);
				printf("////////////////\n");
			} else {
				printf("=================\n");
				/*WebSocketConnection_status(conn,HTS_SeeOther);
				 WebSocketConnection_addHeader(conn,"Location","http://google.com");
				 WebSocketConnection_sendNl(conn);*/
				WebSocketConnection_free(conn);
				printf("=================\n");
			}
		}
		i++;
	}
	/*while (conn != NULL){
	 WebSocketConnection_read(conn);
	 if (!conn->open){
	 break;
	 }
	 //system("xdotool key space");
	 printf("%s\n",conn->response);
	 }
	 WebSocketConnection_free(conn);*/

	printf("Terminado\n");
	return 0;
}
