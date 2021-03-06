#ifndef HTTPD_H
#define HTTPD_H

#include <esp8266.h>

#define HTTPDVER "0.4-based"

typedef enum {
	HTTPD_CGI_MORE = 0,
	HTTPD_CGI_DONE = 1,
	HTTPD_CGI_NOTFOUND = 2,
	HTTPD_CGI_AUTHENTICATED = 3,
} httpd_cgi_state;

typedef enum {
	HTTP_GET = 1,
	HTTP_POST = 2,
	HTTP_OPTIONS = 3,
	HTTP_PUT = 4,
	HTTP_DELETE = 5,
	HTTP_PATCH = 6,
	HTTP_HEAD = 7,
} http_method;


typedef struct HttpdPriv HttpdPriv;
typedef struct HttpdConnData HttpdConnData;
typedef struct HttpdPostData HttpdPostData;

typedef httpd_cgi_state (* cgiSendCallback)(HttpdConnData *connData);
typedef httpd_cgi_state (* cgiRecvHandler)(HttpdConnData *connData, char *data, int len);

//A struct describing a http connection. This gets passed to cgi functions.
struct HttpdConnData {
	ConnTypePtr conn;		// The TCP connection. Exact type depends on the platform.
	http_method requestType; // method type
	char *url;				// The URL requested, without hostname or GET arguments
	char *getArgs;			// The GET arguments for this request, if any.

	const void *cgiArg;		// Argument to the CGI function, as stated as the 3rd argument of
							// the builtInUrls entry that referred to the CGI function.

	const void *cgiArg2;	// Argument to the CGI function, as stated as the 4th argument of
							// the builtInUrls entry that referred to the CGI function.

	void *cgiData;			// Opaque data pointer for the CGI function

	char *hostName;			// Host name field of request
	HttpdPriv *priv;		// Opaque pointer to data for internal httpd housekeeping
	cgiSendCallback cgi;	// CGI function pointer
	cgiRecvHandler recvHdl;	// Handler for data received after headers, if any
	HttpdPostData *post;	// POST data structure
	int remote_port;		// Remote TCP port
	uint8 remote_ip[4];		// IP address of client
	uint8 slot;				// Slot ID
};

//A struct describing the POST data sent inside the http connection.  This is used by the CGI functions
struct HttpdPostData {
	int len;				// POST Content-Length
	int buffSize;			// The maximum length of the post buffer
	int buffLen;			// The amount of bytes in the current post buffer
	int received;			// The total amount of bytes received so far
	char *buff;				// Actual POST data buffer
	char *multipartBoundary; //Text of the multipart boundary, if any
};

//A struct describing an url. This is the main struct that's used to send different URL requests to
//different routines.
typedef struct {
	const char *url;
	cgiSendCallback cgiCb;
	const void *cgiArg;
	const void *cgiArg2;
} HttpdBuiltInUrl;


// macros for defining HttpdBuiltInUrl's

#define ROUTE_CGI_ARG2(path, handler, arg1, arg2)  {path, handler, (void *)arg1, (void *)arg2}

#define ROUTE_CGI_ARG(path, handler, arg1)         ROUTE_CGI_ARG2(path, handler, arg1, NULL)
#define ROUTE_CGI(path, handler)                   ROUTE_CGI_ARG2(path, handler, NULL, NULL)

#define ROUTE_FILE(path, filepath)                 ROUTE_CGI_ARG(path, cgiEspFsFile, filepath)

// the argument of a template route is accessible as cgiArg2 on the connData struct.
#define ROUTE_TPL(path, replacer)                  ROUTE_CGI_ARG(path, cgiEspFsTemplate, replacer)
#define ROUTE_TPL_FILE(path, replacer, filepath)   ROUTE_CGI_ARG2(path, cgiEspFsTemplate, replacer, filepath)

#define ROUTE_REDIRECT(path, target)               ROUTE_CGI_ARG(path, cgiRedirect, target)
#define ROUTE_AUTH(path, passwdFunc)               ROUTE_CGI_ARG(path, authBasic, passwdFunc)

// catch-all route
#define ROUTE_FS(path)                             ROUTE_CGI(path, cgiEspFsHook)

#define ROUTE_END() {NULL, NULL, NULL, NULL}

const char *http_method_str(http_method m);

httpd_cgi_state cgiRedirect(HttpdConnData *connData);
httpd_cgi_state cgiRedirectToHostname(HttpdConnData *connData);
httpd_cgi_state cgiRedirectApClientToHostname(HttpdConnData *connData);

void httpdRedirect(HttpdConnData *conn, char *newUrl);
int httpdUrlDecode(char *val, int valLen, char *ret, int retLen);
int httpdFindArg(char *line, char *arg, char *buff, int buffLen);
void httpdInit(HttpdBuiltInUrl *fixedUrls, int port);
const char *httpdGetMimetype(const char *url);
void httpdDisableTransferEncoding(HttpdConnData *conn);
void httpdStartResponse(HttpdConnData *conn, int code);
void httpdHeader(HttpdConnData *conn, const char *field, const char *val);
void httpdEndHeaders(HttpdConnData *conn);
int httpdGetHeader(HttpdConnData *conn, char *header, char *ret, int retLen);
int httpdSend(HttpdConnData *conn, const char *data, int len);
void httpdFlushSendBuffer(HttpdConnData *conn);

//Platform dependent code should call these.
void httpdSentCb(ConnTypePtr conn, char *remIp, int remPort);
void httpdRecvCb(ConnTypePtr conn, char *remIp, int remPort, char *data, unsigned short len);
void httpdDisconCb(ConnTypePtr conn, char *remIp, int remPort);
int httpdConnectCb(ConnTypePtr conn, char *remIp, int remPort);

#endif
