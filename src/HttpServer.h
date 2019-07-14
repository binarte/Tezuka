/*
 * HttpServer.h
 *
 *  Created on: 6 de jul de 2019
 *      Author: jack
 */

#ifndef SRC_HTTPSERVER_H_
#define SRC_HTTPSERVER_H_
#include "Array.h"
#include "Utils.h"

struct _HttpHandler_;
struct _HttpConnection_;
struct _WebSocketConnection_;
typedef bool (*HttpRun)(struct _HttpConnection_*);
typedef bool (*WebSocketRun)(struct _WebSocketConnection_*);

typedef struct UploadFile {
	char* tempFile;
	char* filename;
	char* mimeType;
	size_t size;
};

typedef struct _WebSocketConnection_ {
	int foo;
} WebSocketConnection;

typedef struct _HttpConnection_ {
	struct _HttpHandler_* handler;
	int socket;
	Array headers;
	Array postData;
	Array uploadFiles;
	char readBuffer[1024];
	void* data;
	char* path;
	char* query;
} HttpConnection;

typedef struct _HttpHandler_ {
	char *path;
	char *webSocketProtocol;
	HttpRun *httpHandler;
	WebSocketRun *webSocketHandler;
} HttpHandler;

typedef struct {
	Array acceptQueue;
	Array websockets;
	Array httpconns;

} HttpServer;

#endif /* SRC_HTTPSERVER_H_ */
