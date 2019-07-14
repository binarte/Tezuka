/*
 * WebSocket.h
 *
 *  Created on: 29 de jun de 2019
 *      Author: jack
 */

#ifndef SRC_WEBSOCKET_H_
#define SRC_WEBSOCKET_H_

#include "WebSocket_utils.h"
#include <pthread.h>
#include "Utils.h"
#include "ValuePair.h"


struct __WSCHandlerAlias_;
struct __WSConnection_;

typedef struct {
	unsigned short port;
	struct sockaddr_in address;
	int server_fd;

	struct __WSCHandlerAlias_ *aliases;
	size_t aliasCount;
	size_t aliasBufferCount;

	struct __WSConnection_ *connections;
	struct __WSConnection_ *connectionCursor;
	size_t connectionBufferCount;

	size_t connectionCount;
	bool running;
} WebSocket;

typedef enum {
	HTM_UNKNOWN = 0,
	HTM_Get,
	HTM_Post,
	HTM_Head,
	HTM_Put,
	HTM_Delete,
	HTM_Connect,
	HTM_Options,
	HTM_Trace,
} HttpMethod;

typedef enum {

	/**
	 * Continue
	 *
	 * The server has received the request headers and the client should proceed to send the request
	 * body (in the case of a request for which a body needs to be sent; for example, a POST request).
	 * Sending a large request body to a server after a request has been rejected for inappropriate headers
	 * would be inefficient. To have a server check the request's headers, a client must send Expect:
	 * 100-continue as a header in its initial request and receive a 100 Continue status code in response
	 * before sending the body. If the client receives an error code such as 403 (Forbidden) or 405 (Method
	 * Not Allowed) then it shouldn't send the request's body. The response 417 Expectation Failed indicates
	 * that the request should be repeated without the Expect header as it indicates that the server
	 * doesn't support expectations (this is the case, for example, of HTTP/1.0 servers).
	 */
	HTS_Continue = 100,
	/**
	 * Switching Protocols
	 *
	 * The requester has asked the server to switch protocols and the server has agreed to do so.
	 */
	HTS_SwitchingProtocols = 101,
	/**
	 * Processing
	 *
	 * A WebDAV request may contain many sub-requests involving file operations, requiring a long time
	 * to complete the request. This code indicates that the server has received and is processing the
	 * request, but no response is available yet. This prevents the client from timing out and assuming
	 * the request was lost.
	 */
	HTS_Processing = 102,
	/**
	 * Early Hints
	 *
	 * Used to return some response headers before final HTTP message.
	 */
	HTS_EarlyHints = 103,
	/**
	 * OK
	 *
	 * Standard response for successful HTTP requests. The actual response will depend on the request
	 * method used. In a GET request, the response will contain an entity corresponding to the requested
	 * resource. In a POST request, the response will contain an entity describing or containing the
	 * result of the action.
	 */
	HTS_Ok = 200,
	/**
	 * Created
	 *
	 * The request has been fulfilled, resulting in the creation of a new resource.
	 */
	HTS_Created = 201,
	/**
	 * Accepted
	 *
	 * The request has been accepted for processing, but the processing has not been completed. The request
	 * might or might not be eventually acted upon, and may be disallowed when processing occurs.
	 */
	HTS_Accepted = 202,
	/**
	 * Non-Authoritative Information
	 *
	 * The server is a transforming proxy (e.g. a Web accelerator) that received a 200 OK from its origin,
	 * but is returning a modified version of the origin's response.
	 */
	HTS_NonAuthoritativeInformation = 203,
	/**
	 * No Content
	 *
	 * The server successfully processed the request and is not returning any content.
	 */
	HTS_NoContent = 204,
	/**
	 * Reset Content
	 *
	 * The server successfully processed the request, but is not returning any content. Unlike a 204
	 * response, this response requires that the requester reset the document view.
	 */
	HTS_ResetContent = 205,
	/**
	 * Partial Content
	 *
	 * The server is delivering only part of the resource (byte serving) due to a range header sent by
	 * the client. The range header is used by HTTP clients to enable resuming of interrupted downloads,
	 * or split a download into multiple simultaneous streams.
	 */
	HTS_PartialContent = 206,
	/**
	 * Multi-Status
	 *
	 * The message body that follows is by default an XML message and can contain a number of separate
	 * response codes, depending on how many sub-requests were made.
	 */
	HTS_MultiStatus = 207,
	/**
	 * Already Reported
	 *
	 * The members of a DAV binding have already been enumerated in a preceding part of the (multistatus)
	 * response, and are not being included again.
	 */
	HTS_AlreadyReported = 208,
	/**
	 * IM Used
	 *
	 * The server has fulfilled a request for the resource, and the response is a representation of the
	 * result of one or more instance-manipulations applied to the current instance.
	 */
	HTS_ImUsed = 226,
	/**
	 * Multiple Choices
	 *
	 * Indicates multiple options for the resource from which the client may choose (via agent-driven
	 * content negotiation). For example, this code could be used to present multiple video format options,
	 * to list files with different filename extensions, or to suggest word-sense disambiguation.
	 */
	HTS_MultipleChoices = 300,
	/**
	 * Moved Permanently
	 *
	 * This and all future requests should be directed to the given URI.
	 */
	HTS_MovedPermanently = 301,
	/**
	 * Found
	 *
	 * Tells the client to look at (browse to) another URL. 302 has been superseded by 303 and 307. This
	 * is an example of industry practice contradicting the standard. The HTTP/1.0 specification (RFC
	 * 1945) required the client to perform a temporary redirect (the original describing phrase was
	 * "Moved Temporarily"),
	 */
	HTS_Found = 302,
	/**
	 * See Other
	 *
	 * The response to the request can be found under another URI using the GET method. When received
	 * in response to a POST (or PUT/DELETE), the client should presume that the server has received
	 * the data and should issue a new GET request to the given URI.
	 */
	HTS_SeeOther = 303,
	/**
	 * Not Modified
	 *
	 * Indicates that the resource has not been modified since the version specified by the request headers
	 * If-Modified-Since or If-None-Match. In such case, there is no need to retransmit the resource
	 * since the client still has a previously-downloaded copy.
	 */
	HTS_NotModified = 304,
	/**
	 * Use Proxy
	 *
	 * The requested resource is available only through a proxy, the address for which is provided in
	 * the response. For security reasons, many HTTP clients (such as Mozilla Firefox and Internet Explorer)
	 * do not obey this status code.
	 */
	HTS_UseProxy = 305,
	/**
	 * Switch Proxy
	 *
	 * No longer used. Originally meant "Subsequent requests should use the specified proxy."
	 */
	HTS_SwitchProxy = 306,
	/**
	 * Temporary Redirect
	 *
	 * In this case, the request should be repeated with another URI; however, future requests should
	 * still use the original URI. In contrast to how 302 was historically implemented, the request method
	 * is not allowed to be changed when reissuing the original request. For example, a POST request
	 * should be repeated using another POST request.
	 */
	HTS_TemporaryRedirect = 307,
	/**
	 * Permanent Redirect
	 *
	 * The request and all future requests should be repeated using another URI. 307 and 308 parallel
	 * the behaviors of 302 and 301, but do not allow the HTTP method to change. So, for example, submitting
	 * a form to a permanently redirected resource may continue smoothly.
	 */
	HTS_PermanentRedirect = 308,
	/**
	 * Bad Request
	 *
	 * The server cannot or will not process the request due to an apparent client error (e.g., malformed
	 * request syntax, size too large, invalid request message framing, or deceptive request routing).
	 */
	HTS_BadRequest = 400,
	/**
	 * Unauthorized
	 *
	 * Similar to 403 Forbidden, but specifically for use when authentication is required and has failed
	 * or has not yet been provided. The response must include a WWW-Authenticate header field containing
	 * a challenge applicable to the requested resource. See Basic access authentication and Digest access
	 * authentication., the user does not have valid authentication credentials for the target resource.
	 */
	HTS_Unauthorized = 401,
	/**
	 * Payment Required
	 *
	 * Reserved for future use. The original intention was that this code might be used as part of some
	 * form of digital cash or micropayment scheme, as proposed, for example, by GNU Taler,
	 */
	HTS_PaymentRequired = 402,
	/**
	 * Forbidden
	 *
	 * The request was valid, but the server is refusing action. The user might not have the necessary
	 * permissions for a resource, or may need an account of some sort. This code is also typically used
	 * if the request provided authentication via the WWW-Authenticate header field, but the server did
	 * not accept that authentication.
	 */
	HTS_Forbidden = 403,
	/**
	 * Not Found
	 *
	 * The requested resource could not be found but may be available in the future. Subsequent requests
	 * by the client are permissible.
	 */
	HTS_NotFound = 404,
	/**
	 * Method Not Allowed
	 *
	 * A request method is not supported for the requested resource; for example, a GET request on a
	 * form that requires data to be presented via POST, or a PUT request on a read-only resource.
	 */
	HTS_MethodNotAllowed = 405,
	/**
	 * Not Acceptable
	 *
	 * The requested resource is capable of generating only content not acceptable according to the Accept
	 * headers sent in the request. See Content negotiation.
	 */
	HTS_NotAcceptable = 406,
	/**
	 * Proxy Authentication Required
	 *
	 * The client must first authenticate itself with the proxy.
	 */
	HTS_ProxyAuthenticationRequired = 407,
	/**
	 * Request Timeout
	 *
	 * The server timed out waiting for the request. According to HTTP specifications: "The client did
	 * not produce a request within the time that the server was prepared to wait. The client MAY repeat
	 * the request without modifications at any later time."
	 */
	HTS_RequestTimeout = 408,
	/**
	 * Conflict
	 *
	 * Indicates that the request could not be processed because of conflict in the current state of
	 * the resource, such as an edit conflict between multiple simultaneous updates.
	 */
	HTS_Conflict = 409,
	/**
	 * Gone
	 *
	 * Indicates that the resource requested is no longer available and will not be available again.
	 * This should be used when a resource has been intentionally removed and the resource should be
	 * purged. Upon receiving a 410 status code, the client should not request the resource in the future.
	 * Clients such as search engines should remove the resource from their indices. Most use cases do
	 * not require clients and search engines to purge the resource, and a "404 Not Found" may be used
	 * instead.
	 */
	HTS_Gone = 410,
	/**
	 * Length Required
	 *
	 * The request did not specify the length of its content, which is required by the requested resource.
	 */
	HTS_LengthRequired = 411,
	/**
	 * Precondition Failed
	 *
	 * The server does not meet one of the preconditions that the requester put on the request header
	 * fields.
	 */
	HTS_PreconditionFailed = 412,
	/**
	 * Payload Too Large
	 *
	 * The request is larger than the server is willing or able to process. Previously called "Request
	 * Entity Too Large".
	 */
	HTS_PayloadTooLarge = 413,
	/**
	 * URI Too Long
	 *
	 * The URI provided was too long for the server to process. Often the result of too much data being
	 * encoded as a query-string of a GET request, in which case it should be converted to a POST request.
	 */
	HTS_UriTooLong = 414,
	/**
	 * Unsupported Media Type
	 *
	 * The request entity has a media type which the server or resource does not support. For example,
	 * the client uploads an image as image/svg+xml, but the server requires that images use a different
	 * format.
	 */
	HTS_UnsupportedMediaType = 415,
	/**
	 * Range Not Satisfiable
	 *
	 * The client has asked for a portion of the file (byte serving), but the server cannot supply that
	 * portion. For example, if the client asked for a part of the file that lies beyond the end of the
	 * file.
	 */
	HTS_RangeNotSatisfiable = 416,
	/**
	 * Expectation Failed
	 *
	 * The server cannot meet the requirements of the Expect request-header field.
	 */
	HTS_ExpectationFailed = 417,
	/**
	 * I'm a teapot
	 *
	 * This code was defined in 1998 as one of the traditional IETF April Fools' jokes, in RFC 2324,
	 * Hyper Text Coffee Pot Control Protocol, and is not expected to be implemented by actual HTTP servers.
	 * The RFC specifies this code should be returned by teapots requested to brew coffee.
	 */
	HTS_ImATeapot = 418,
	/**
	 * Misdirected Request
	 *
	 * The request was directed at a server that is not able to produce a response
	 */
	HTS_MisdirectedRequest = 421,
	/**
	 * Unprocessable Entity
	 *
	 * The request was well-formed but was unable to be followed due to semantic errors.
	 */
	HTS_UnprocessableEntity = 422,
	/**
	 * Locked
	 *
	 * The resource that is being accessed is locked.
	 */
	HTS_Locked = 423,
	/**
	 * Failed Dependency
	 *
	 * The request failed because it depended on another request and that request failed (e.g., a PROPPATCH).
	 */
	HTS_FailedDependency = 424,
	/**
	 * Too Early
	 *
	 * Indicates that the server is unwilling to risk processing a request that might be replayed.
	 */
	HTS_TooEarly = 425,
	/**
	 * Upgrade Required
	 *
	 * The client should switch to a different protocol such as TLS/1.0, given in the Upgrade header
	 * field.
	 */
	HTS_UpgradeRequired = 426,
	/**
	 * Precondition Required
	 *
	 * The origin server requires the request to be conditional. Intended to prevent the 'lost update'
	 * problem, where a client GETs a resource's state, modifies it, and PUTs it back to the server,
	 * when meanwhile a third party has modified the state on the server, leading to a conflict.
	 */
	HTS_PreconditionRequired = 428,
	/**
	 * Too Many Requests
	 *
	 * The user has sent too many requests in a given amount of time. Intended for use with rate-limiting
	 * schemes.
	 */
	HTS_TooManyRequests = 429,
	/**
	 * Request Header Fields Too Large
	 *
	 * The server is unwilling to process the request because either an individual header field, or all
	 * the header fields collectively, are too large.
	 */
	HTS_RequestHeaderFieldsTooLarge = 431,
	/**
	 * Unavailable For Legal Reasons
	 *
	 * A server operator has received a legal demand to deny access to a resource or to a set of resources
	 * that includes the requested resource. The code 451 was chosen as a reference to the novel Fahrenheit
	 * 451 (see the Acknowledgements in the RFC).
	 */
	HTS_UnavailableForLegalReasons = 451,
	/**
	 * Internal Server Error
	 *
	 * A generic error message, given when an unexpected condition was encountered and no more specific
	 * message is suitable.
	 */
	HTS_InternalServerError = 500,
	/**
	 * Not Implemented
	 *
	 * The server either does not recognize the request method, or it lacks the ability to fulfil the
	 * request. Usually this implies future availability (e.g., a new feature of a web-service API).
	 */
	HTS_NotImplemented = 501,
	/**
	 * Bad Gateway
	 *
	 * The server was acting as a gateway or proxy and received an invalid response from the upstream
	 * server.
	 */
	HTS_BadGateway = 502,
	/**
	 * Service Unavailable
	 *
	 * The server cannot handle the request (because it is overloaded or down for maintenance). Generally,
	 * this is a temporary state.
	 */
	HTS_ServiceUnavailable = 503,
	/**
	 * Gateway Timeout
	 *
	 * The server was acting as a gateway or proxy and did not receive a timely response from the upstream
	 * server.
	 */
	HTS_GatewayTimeout = 504,
	/**
	 * HTTP Version Not Supported
	 *
	 * The server does not support the HTTP protocol version used in the request.
	 */
	HTS_HttpVersionNotSupported = 505,
	/**
	 * Variant Also Negotiates
	 *
	 * Transparent content negotiation for the request results in a circular reference.
	 */
	HTS_VariantAlsoNegotiates = 506,
	/**
	 * Insufficient Storage
	 *
	 * The server is unable to store the representation needed to complete the request.
	 */
	HTS_InsufficientStorage = 507,
	/**
	 * Loop Detected
	 *
	 * The server detected an infinite loop while processing the request (sent instead of 208 Already
	 * Reported).
	 */
	HTS_LoopDetected = 508,
	/**
	 * Not Extended
	 *
	 * Further extensions to the request are required for the server to fulfil it.
	 */
	HTS_NotExtended = 510,
} HttpStatus;

typedef struct __WSConnection_ {
	WebSocket *parent;
	int socketId;
	unsigned short protocolVersion;
	char *path;
	char *protocol;
	HttpMethod method;
	char *httpVersion;

	char *message;
	size_t messageLength;
	size_t messageBufferLength;
	char *messageCursor;

	char *response;
	size_t responseLength;
	size_t responseBufferLength;

	bool isWebSocket;
	bool secureHttp;
	bool fin;
	bool useMask;
	unsigned char opcode;
	union {
		uint32_t num;
		char chrs[4];
	} mask;
	bool open;
	pthread_t thread;
	struct __WSConnection_ *connectionCursor;
} WebSocketConnection;

typedef struct {
	bool fin;
	bool useMask;
	unsigned char opcode;
	unsigned long responseBufferLength;
	unsigned long responseLength;
	char mask[4];
	char *content;
	bool open;
} WebSocketPayload;


typedef struct {
	bool finished;
	Array headers;
	size_t headerCount;
	void* data;
} HttpConnection;

typedef enum {
	WSMT_a,
} WebSocket_messageType;

int WebSocket_open(WebSocket *ws);
WebSocketConnection* WebSocket_accept(WebSocket *ws, int *error);

/**
 * Clears the message in the buffer
 * @param ws Connection to clear message from
 */
void WebSocketConnection_messageClear(WebSocketConnection *ws);
void WebSocketConnection_messageSet(WebSocketConnection *ws,
		const char *message);
void WebSocketConnection_messageAppend(WebSocketConnection *ws,
		const char *message);

void WebSocketConnection_status(WebSocketConnection *ws, HttpStatus status);
void WebSocketConnection_addHeader(WebSocketConnection *ws, const char *name,
		const char *value);
int WebSocketConnection_sendNl(WebSocketConnection *ws);
int WebSocketConnection_send(WebSocketConnection *ws);

int WebSocketConnection_read(WebSocketConnection *ws);
int WebSocketConnection_sendText(WebSocketConnection *ws, const char *text);
int WebSocketConnection_sendBinary(WebSocketConnection *ws, const char *blob,
		size_t length);
int WebSocketConnection_free(WebSocketConnection *ws);
void WebSocketPayload_init(WebSocketPayload *pl);

const char* HttpStatusName(const HttpStatus st);

typedef void (*WebSocketConnectionHandler)(WebSocketConnection*);

void WebSocket_addHandler(WebSocket *ws, const char *name,
		WebSocketConnectionHandler *handler);

void WebSocket_run(WebSocket *ws);

typedef struct __WSCHandlerAlias_ {
	WebSocketConnectionHandler *handler;
	char *path;
} WebSocketConnectionHandlerAlias;

#endif /* SRC_WEBSOCKET_H_ */

