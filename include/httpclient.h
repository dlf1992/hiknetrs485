/*  Make the necessary includes and set up the variables.  */

#ifndef HTTPCLIENT_H_
#define HTTPCLIENT_H_

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "md5.h"
#include "pub.h"
#include "log.h"

using namespace std;

#define HTTP_BUF 4096*2
typedef  enum{connect_NONE = 0,
			connect_ING,
			connect_OK,
			connect_BAD,           /* 连接不稳定 */
			connect_CLOSED} status;

struct http_auth_st
{
	char* serverIP;
	char* user;
	char* pass;
	char* realm;
	char* nonce;
} ;
typedef struct http_auth_st *http_auth_t;


struct httpclient_st
{
	http_auth_t auth;
	char* method;
	char* header;//1024
	char* body;//1024
	char* httpBuf;//HTTP_BUF
	status httpST;//http status
	int httpFD;
};
typedef struct httpclient_st *httpclient_t;

typedef struct CameraInfo
{
	char cameraip[16];
	char usrname[32];
	char passwd[32];
}__attribute__((packed)) CAMERAINFO;	//
#ifdef __cplusplus
extern "C" 
{
#endif

extern int generate_auth(httpclient_t ct,char *uri);
extern httpclient_t 	httpClientCreat(char *ip,char *user,char *passwd);
extern int 	httpClientPut(httpclient_t ct,char *uri,char *content);
extern int 	httpClientGet(httpclient_t ct,char *uri,int doorflag);
extern int 	httpSend(httpclient_t ct);
extern int     httpclientFree(httpclient_t ct);
extern int 	httpParse(httpclient_t ct);
extern int 	httpClearConn(httpclient_t ct);
extern void clearpdcnum(char* ip,char* usrname,char* passwd);

extern CAMERAINFO front_camerainfo;
extern CAMERAINFO back_camerainfo;
extern volatile int front_enternum;
extern volatile int front_exitnum;
extern volatile int back_enternum;
extern volatile int back_exitnum;

#ifdef __cplusplus
}  /* end extern "C" */
#endif

#endif