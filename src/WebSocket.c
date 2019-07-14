/*
 * WebSocket.c
 *
 *  Created on: 30 de jun de 2019
 *      Author: jack
 */
#include "WebSocket.h"
#include "WebSocket_utils.h"
#include <string.h>
#include <strings.h>
#define WSGUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

/*
 WebSocket* WebSocket_open(unsigned short port){
 struct sockaddr aaa;
 struct sockaddr_in address;

 }*/

char __HTS_100[4][20] = { "Continue", //100
		"Switching Protocols", //101
		"Processing", //102
		"Early Hints", //103
		};
char __HTS_200[27][30] = { "OK", //200
		"Created", //201
		"Accepted", //202
		"Non-Authoritative Information", //203
		"No Content", //204
		"Reset Content", //205
		"Partial Content", //206
		"Multi-Status", //207
		"Already Reported", //208
		"", //209
		"", //210
		"", //211
		"", //212
		"", //213
		"", //214
		"", //215
		"", //216
		"", //217
		"", //218
		"", //219
		"", //220
		"", //221
		"", //222
		"", //223
		"", //224
		"", //225
		"IM Used", //226
		};
char __HTS_300[9][19] = { "Multiple Choices", //300
		"Moved Permanently", //301
		"Found", //302
		"See Other", //303
		"Not Modified", //304
		"Use Proxy", //305
		"Switch Proxy", //306
		"Temporary Redirect", //307
		"Permanent Redirect", //308
		};
char __HTS_400[52][32] = { "Bad Request", //400
		"Unauthorized", //401
		"Payment Required", //402
		"Forbidden", //403
		"Not Found", //404
		"Method Not Allowed", //405
		"Not Acceptable", //406
		"Proxy Authentication Required", //407
		"Request Timeout", //408
		"Conflict", //409
		"Gone", //410
		"Length Required", //411
		"Precondition Failed", //412
		"Payload Too Large", //413
		"URI Too Long", //414
		"Unsupported Media Type", //415
		"Range Not Satisfiable", //416
		"Expectation Failed", //417
		"I'm a teapot", //418
		"", //419
		"", //420
		"Misdirected Request", //421
		"Unprocessable Entity", //422
		"Locked", //423
		"Failed Dependency", //424
		"Too Early", //425
		"Upgrade Required", //426
		"", //427
		"Precondition Required", //428
		"Too Many Requests", //429
		"", //430
		"Request Header Fields Too Large", //431
		"", //432
		"", //433
		"", //434
		"", //435
		"", //436
		"", //437
		"", //438
		"", //439
		"", //440
		"", //441
		"", //442
		"", //443
		"", //444
		"", //445
		"", //446
		"", //447
		"", //448
		"", //449
		"", //450
		"Unavailable For Legal Reasons", //451
		};
char __HTS_500[11][27] = { "Internal Server Error", //500
		"Not Implemented", //501
		"Bad Gateway", //502
		"Service Unavailable", //503
		"Gateway Timeout", //504
		"HTTP Version Not Supported", //505
		"Variant Also Negotiates", //506
		"Insufficient Storage", //507
		"Loop Detected", //508
		"", //509
		"Not Extended", //510
		};
const char* HttpStatusName(const HttpStatus st) {
	int i = (int) st;
	if (i >= 600) {
		return NULL;
	}
#define STATUSI(NUM,MAX)\
	if (i >= NUM){\
		i -= NUM;\
		if (i > MAX || __HTS_##NUM[i][0] == '\0'){\
			return NULL;\
		}\
		return __HTS_##NUM[i];\
	}
	STATUSI(500, 10)
	STATUSI(400, 51)
	STATUSI(300, 8)
	STATUSI(200, 26)
	STATUSI(100, 3)
	return NULL;
#undef STATUSI
}

#define ERR(FUNC)\
	err=FUNC;\
	if (err != 0){\
		return err;\
	}

int WebSocket_open(WebSocket *ws) {
	int err, opt = 1;
	struct sockaddr_in address;
	ws->server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (ws->server_fd == 0) {
		return 1;
	}
	ERR(
			setsockopt(ws->server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))

	ws->address.sin_family = AF_INET;
	ws->address.sin_addr.s_addr = INADDR_ANY;
	ws->address.sin_port = htons(ws->port);

	ERR(bind(ws->server_fd, (struct sockaddr* ) &ws->address, sizeof(address)))
	ERR(listen(ws->server_fd, 3))
	return 0;
}

WebSocketConnection* WebSocket_accept(WebSocket *ws, int *error) {
#define SETERR(NUM) \
	if (error != NULL) {\
		*error = NUM;\
	}
#define SHDOWN\
	close(new_socket);\
	return NULL;

	int valread;
	char *c;
	char buffer[1024];
	char header[1024];
	socklen_t addrlen = sizeof(struct sockaddr_in);
	printf("Aceitando S%i\n", ws->server_fd);
	int new_socket = accept(ws->server_fd, (struct sockaddr*) &ws->address,
			&addrlen);
	if (new_socket < 0) {
		SETERR(new_socket)
		return NULL;
	}

	//Try to obtain headers
	printf("Aceito S%i\n", new_socket);
	valread = recv(new_socket, header, 1023, 0);
	printf("Lidos %i de S%i\n", valread, new_socket);
	if (valread <= 0) {
		SETERR(1)
		SHDOWN
	}
	header[valread] = '\0';
	c = strstr(header, "\n") - 1;
	memcpy(buffer, header, c - header);
	buffer[c - header] = '\0';

	regex_t regex;
	int reti;

#define HREGEX "([a-zA-Z]+)\\s+([^\\s]+)\\s+(HTTPS?)/([0-1]+(\\.[0-1]+)*)"
#define HREGEXCMP 5
	regmatch_t pmatch[HREGEXCMP];

	//compile regular expression
	reti = regcomp(&regex, HREGEX, REG_ICASE | REG_EXTENDED);
	if (reti) {
		SETERR(-2)
		fprintf(stderr, "Could not compile regex\n");
		SHDOWN
	}

	// Match regular expression to the first header line
	reti = regexec(&regex, buffer, HREGEXCMP, pmatch, 0);
	if (reti == REG_NOMATCH) {
		SETERR(2)
		SHDOWN
	} else if (reti) {
		SETERR(reti + 1024);
		regerror(reti, &regex, buffer, sizeof(buffer));
		fprintf(stderr, "Regex match failed: %s\n", buffer);
		SHDOWN
	}

	//Add terminators after each component for easy separation
	for (int i = 1; i < HREGEXCMP; i++) {
		buffer[pmatch[i].rm_eo] = '\0';
	}

	//Check for method
	HttpMethod method;
	if (strcasecmp(&buffer[pmatch[1].rm_so], "GET") == 0) {
		method = HTM_Get;
	} else if (strcasecmp(&buffer[pmatch[1].rm_so], "POST") == 0) {
		method = HTM_Post;
	} else if (strcasecmp(&buffer[pmatch[1].rm_so], "HEAD") == 0) {
		method = HTM_Head;
	} else if (strcasecmp(&buffer[pmatch[1].rm_so], "PUT") == 0) {
		method = HTM_Put;
	} else if (strcasecmp(&buffer[pmatch[1].rm_so], "DELETE") == 0) {
		method = HTM_Delete;
	} else if (strcasecmp(&buffer[pmatch[1].rm_so], "CONNECT") == 0) {
		method = HTM_Connect;
	} else if (strcasecmp(&buffer[pmatch[1].rm_so], "OPTIONS") == 0) {
		method = HTM_Options;
	} else if (strcasecmp(&buffer[pmatch[1].rm_so], "TRACE") == 0) {
		method = HTM_Trace;
	} else {
		fprintf(stderr, "Unknown HTTP method: %s\n", &buffer[pmatch[1].rm_so]);
		method = HTM_UNKNOWN;
	}

	//Get path
	char path[1024];
	strcpy(path, &buffer[pmatch[3].rm_so]);

	//Check for https
	bool isSecure = strcasecmp(&buffer[pmatch[3].rm_so], "HTTPS") == 0;

	//Get HTTP version
	char httpVersion[32];
	strcpy(httpVersion, &buffer[pmatch[4].rm_so]);

	//Get websocket headers
	char key[64];
	getHeader(header, "Sec-WebSocket-Key", key);
	char protocol[64];
	getHeader(header, "Sec-WebSocket-Protocol", protocol);
	unsigned short protocolVersion = (unsigned short) getHeaderLong(header,
			"Sec-WebSocket-Version");

	//In case of websocket request, send response
	if (key[0] != '\0') {
		//Generate accept key
		char rkey[64];
		unsigned char hash[SHA_DIGEST_LENGTH];
		sprintf(buffer, "%s" WSGUID, key);
		SHA1((unsigned char*) buffer, strlen(buffer), hash);
		base64_encode(hash, rkey, SHA_DIGEST_LENGTH);

		//Prepare response headers
		strcpy(header, "HTTP/2.0 101 Switching Protocols\n");
		c = header + strlen(header);
		c = addHeader(c, "Upgrade", "websocket");
		c = addHeader(c, "Connection", "upgrade");
		c = addHeader(c, "Sec-WebSocket-Accept", rkey);
		if (protocol[0] != '\0') {
			c = addHeader(c, "Sec-WebSocket-Protocol", protocol);
		}
		strcpy(c, "\n");

		//Send response
		valread = send(new_socket, header, strlen(header), 0);
		if (valread == 0) {
			SETERR(-3)
			fprintf(stderr, "Failed to send Websocket response\n");
			SHDOWN
		}
	}

	//Create connection object
	WebSocketConnection *out = (WebSocketConnection*) malloc(
			sizeof(WebSocketConnection));

	out->path = (char*) malloc(sizeof(char) * (strlen(path) + 1));
	strcpy(out->path, path);

	out->httpVersion = (char*) malloc(sizeof(char) * (strlen(httpVersion) + 1));
	strcpy(out->httpVersion, httpVersion);

	if (protocol[0] == '\0') {
		out->protocol = NULL;
	} else {
		out->protocol = (char*) malloc(sizeof(char) * (strlen(protocol) + 1));
		strcpy(out->protocol, protocol);
	}

	out->message = NULL;
	out->messageCursor = NULL;
	out->messageBufferLength = 0;
	out->messageLength = 0;

	out->response = NULL;
	out->responseLength = 0;
	out->responseBufferLength = 0;

	out->parent = ws;
	out->open = true;
	out->socketId = new_socket;
	out->secureHttp = isSecure;
	out->method = method;
	out->isWebSocket = key[0] != '\0';
	out->fin = false;
	out->mask.num = 0;
	out->opcode = 0;
	out->useMask = false;
	out->protocolVersion = protocolVersion;

	return out;
}

void WebSocketPayload_init(WebSocketPayload *pl) {
	pl->content = NULL;
	pl->fin = false;
	pl->useMask = false;
	pl->open = false;
	//pl->length = 0;
	//pl->length = 0;
	//pl->bufferLength = 0;
	pl->mask[0] = pl->mask[1] = pl->mask[2] = pl->mask[3] = '\0';
	pl->opcode = 0;
}

int WebSocketConnection_read(WebSocketConnection *wsc) {
	uint8_t c;
	read(wsc->socketId, &c, 1);
	wsc->fin = (c & 0x80) == 0x80;
	wsc->opcode = c & 0x07;
	read(wsc->socketId, &c, 1);
	wsc->useMask = (c & 0x80) == 0x80;
	wsc->responseLength = c & 0x7f;

	if (wsc->responseLength == 126) {
		uint16_t l;
		read(wsc->socketId, &l, 2);
		wsc->responseLength = c;
	} else if (wsc->responseLength == 127) {
		uint64_t l;
		read(wsc->socketId, &l, 8);
		wsc->responseLength = l;
	}

	if (wsc->useMask) {
		uint32_t mask;
		read(wsc->socketId, &mask, 4);
		for (unsigned char i = 0; i < 4; i++) {
			wsc->mask.num = mask;
		}
	} else {
		wsc->mask.num = 0;
	}

	if (wsc->responseLength > wsc->responseBufferLength) {
		if (wsc->response != NULL) {
			free(wsc->response);
		}
		wsc->responseBufferLength = ((wsc->responseLength / 256) + 1) * 256;
		wsc->response = (char*) malloc(wsc->responseBufferLength);
	}
	read(wsc->socketId, wsc->response, wsc->responseLength);
	if (wsc->responseLength > 0) {
		wsc->response[wsc->responseLength] = '\0';
	}

	if (wsc->useMask) {
		for (unsigned long int i = 0; i < wsc->responseLength; i++) {
			wsc->response[i] = wsc->response[i] ^ wsc->mask.chrs[i % 4];
		}
	}

	wsc->open = true;
	if (wsc->opcode == 0) {
		if (wsc->responseLength == 0 || strcmp("\3\xE9", wsc->response) == 0) {
			wsc->open = false;
		}
	} else if (wsc->opcode == 0x8) {
		wsc->open = false;
	}
	if (!wsc->open) {
		printf("Fechando soquete %i\n", wsc->socketId);
		if (close(wsc->socketId)) {
			return 2;
		}
	}

	return 0;
}

int WebSocketConnection_free(WebSocketConnection *ws) {
	if (ws == NULL) {
		return 1;
	}
	if (ws->open) {
		printf("Fechando soquete %i\n", ws->socketId);
		if (close(ws->socketId)) {
			return 2;
		}
	}

	free(ws->path);
	free(ws->httpVersion);

	if (ws->protocol) {
		free(ws->protocol);
	}
	if (ws->response) {
		free(ws->response);
	}
	free(ws);
	return 0;
}

void WebSocketConnection_messageSet(WebSocketConnection *ws,
		const char *message) {
	size_t len = strlen(message);
	if (len + 1 > ws->messageBufferLength) {
		if (ws->message != NULL) {
			free(ws->message);
		}
		ws->messageBufferLength = ((len / 256) + 1) * 256;
		ws->message = (char*) malloc(ws->messageBufferLength);
	}
	strcpy(ws->message, message);
	ws->messageLength = len;
	ws->messageCursor = ws->message + len;
}

void WebSocketConnection_messageAppend(WebSocketConnection *ws,
		const char *message) {
	if (ws->message == NULL || ws->message[0] == '\0') {
		WebSocketConnection_messageSet(ws, message);
		return;
	}

	size_t len = strlen(message);
	ws->messageLength += len;
	if (ws->messageLength + 1 > ws->messageBufferLength) {
		char *prev = ws->message;
		ws->messageBufferLength = ((ws->messageLength / 256) + 1) * 256;
		ws->message = (char*) malloc(ws->messageBufferLength);

		if (prev != NULL) {
			strcpy(ws->message, prev);
			free(prev);
		}
	}
	strcpy(ws->messageCursor, message);
	ws->messageLength += len;
	ws->messageCursor += len;
}
void WebSocketConnection_messageClear(WebSocketConnection *ws) {
	if (ws->message == NULL || ws->messageLength == 0) {
		return;
	}
	ws->messageLength = 0;
	ws->messageCursor = ws->message;
	ws->message[0] = '\0';
}

void WebSocketConnection_status(WebSocketConnection *ws, HttpStatus status) {
	char *c[256];
	sprintf(c, "HTTP/2.0 %i %s\n", (int) status, HttpStatusName(status));
	WebSocketConnection_messageSet(ws, c);
}

void WebSocketConnection_addHeader(WebSocketConnection *ws, const char *name,
		const char *value) {
	WebSocketConnection_messageAppend(ws, name);
	WebSocketConnection_messageAppend(ws, ": ");
	WebSocketConnection_messageAppend(ws, value);
	WebSocketConnection_messageAppend(ws, "\n");
}

int WebSocketConnection_sendNl(WebSocketConnection *ws) {
	if (ws->message == NULL || ws->message[0] == '\0') {
		return 1;
	}
	WebSocketConnection_messageAppend(ws, "\n");
	return WebSocketConnection_send(ws);
}

int WebSocketConnection_send(WebSocketConnection *ws) {
	if (ws->message == NULL || ws->message[0] == '\0') {
		return 1;
	}
	int valread = send(ws->socketId, ws->message, ws->messageLength, 0);
	if (valread == 0) {
		return 2;
	}
	WebSocketConnection_messageClear(ws);
	return 0;
}

struct __WebSocket_thread_data {
	pthread_t thread;
	WebSocket *ws;
	int socket;
};
#define WPTR struct __WebSocket_thread_data

void* __WebSocket_thread_(void *x_void_ptr) {
	WPTR* data = (WPTR*) x_void_ptr;
return NULL;
}

void WebSocket_run(WebSocket *ws) {
	ws->running = true;
	ws->connectionCount = 0;
	while (ws->running) {
		socklen_t addrlen = sizeof(struct sockaddr_in);
		int new_socket = accept(ws->server_fd, (struct sockaddr*) &ws->address,
				&addrlen);
		if (!ws->running){
			close(new_socket);
			break;
		}
		WPTR *data = (WPTR*) malloc(sizeof(WPTR));
		data->socket = new_socket;
		data->ws = ws;

		//pthread_create(&data->thread, NULL, inc_x, (void*) data);
	}
}
