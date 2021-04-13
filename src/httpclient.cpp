/*  Make the necessary includes and set up the variables.  */

#include "httpclient.h"

CAMERAINFO front_camerainfo;
CAMERAINFO back_camerainfo;
volatile int front_enternum;
volatile int front_exitnum;
volatile int back_enternum;
volatile int back_exitnum;

/* 从字符串中读取enter人数 */
int getpdcenter(const char* pcBuf)
{
	char *pcBegin = NULL;
	char *pcEnd = NULL;
	char tmp[10] = {0};
	int num = 0;
	while(strstr(pcBuf,"<enter>") != NULL)
	{
		//printf("pcBuf = %p\n",pcBuf);
		pcBegin = strstr((char*)pcBuf,"<enter>");
		pcEnd = strstr((char*)pcBuf,"</enter>");
		//printf("pcBegin = %p\n",pcBegin);
		//printf("pcEnd = %p\n",pcEnd);
		if(pcBegin == NULL || pcEnd == NULL || pcBegin > pcEnd)
		{
			//printf("Mail name not found!\n");
			break;
		}
		else
		{
			pcBegin += strlen("<enter>");
			memcpy(tmp, pcBegin, pcEnd-pcBegin);
		}
		//printf("tmp = %s\n",tmp);
		num += Strtoint(tmp);
		memset(tmp,0,sizeof(tmp));
		pcBuf += pcEnd+strlen("</enter>")-pcBuf;
		if(*pcBuf == '\0')
		{
			break;
		}
	}

	return num;
}
/* 从字符串中读取exit人数 */
int getpdcexit(const char* pcBuf)
{
	char *pcBegin = NULL;
	char *pcEnd = NULL;
	char tmp[10] = {0};
	int num = 0;
	while(strstr(pcBuf,"<exit>") != NULL)
	{
		//printf("pcBuf = %p\n",pcBuf);
		pcBegin = strstr((char*)pcBuf, "<exit>");
		pcEnd = strstr((char*)pcBuf, "</exit>");
		//printf("pcBegin = %p\n",pcBegin);
		//printf("pcEnd = %p\n",pcEnd);
		if(pcBegin == NULL || pcEnd == NULL || pcBegin > pcEnd)
		{
			//printf("Mail name not found!\n");
			break;
		}
		else
		{
			pcBegin += strlen("<exit>");
			memcpy(tmp, pcBegin, pcEnd-pcBegin);
		}
		//printf("tmp = %s\n",tmp);
		num += Strtoint(tmp);
		memset(tmp,0,sizeof(tmp));
		pcBuf += pcEnd+strlen("</exit>")-pcBuf;
		if(*pcBuf == '\0')
		{
			break;
		}
	}

	return num;
}
int 	httpClientPut(httpclient_t ct,char *uri,char *content)
{
    int len;
    struct sockaddr_in address;
    int res;
	
	const char *h_header="User-Agent: FHJT_Http_Client\r\nAccept: */*\r\nConnection: Keep-alive\r\nAccept-Encoding: identity\r\n";
	const char *h_con_type="Content-Type: application/binary; charset=\"UTF-8\"\r\n";
	
	/*  Create a socket for the client.  */
	if(ct->httpFD <= 0){
	    ct->httpFD = socket(AF_INET, SOCK_STREAM, 0);
	    address.sin_family = AF_INET;
	    address.sin_addr.s_addr = inet_addr(ct->auth->serverIP);//inet_addr()完成地址格式转换
	    address.sin_port = htons(80);//端口
	    len = sizeof(address);
		res = connect(ct->httpFD, (struct sockaddr *)&address, len);
		if(res == -1) {
	        perror("failed when connect");
			LOGRECORD("failed when connect");
			ct->httpST = connect_NONE;
	        return -1;
	    }
	}
	ct->method = strdup("PUT");
	sprintf(ct->header,"PUT %s HTTP/1.1\r\n",uri);
	len = strlen(ct->header);
	sprintf(ct->header+len,"Host: %s\r\n",ct->auth->serverIP);
	strcat(ct->header,h_header);
	strcat(ct->header,h_con_type);
	len = strlen(ct->header);
	if(content != NULL)
	{
		sprintf(ct->header+len,"Content-Length: %d\r\n",strlen(content));
	}
	strcat(ct->body,"\r\n");
	if(content != NULL)
	{
		strcat(ct->body,content);
	}
	res = httpSend(ct);
	if(res < 0) {httpClearConn(ct); return -1;}
	
	res=recv(ct->httpFD,ct->httpBuf,HTTP_BUF,0);
	if(res <=0) {httpClearConn(ct); return -1;}
		
	res = generate_auth(ct,uri); 
	if(res == -1) {httpClearConn(ct);return -1;};
	if(res == 0) {
		printf("*******closeing\n\r");
		res=recv(ct->httpFD,ct->httpBuf,HTTP_BUF,0);
		if(res ==0) {close(ct->httpFD);}
		ct->httpFD = socket(AF_INET, SOCK_STREAM, 0);
		len = sizeof(address);
		res = connect(ct->httpFD, (struct sockaddr *)&address, len);
		if(res == -1) {
			perror("failed when connect");
			ct->httpST = connect_NONE;
			httpClearConn(ct);
			return -1;
		}
	}
	
	if(res == 1) printf("******keep alive\n\r");
	strcat(ct->header,"Authorization: ");
	strcat(ct->header,ct->httpBuf);
	strcat(ct->header,"\r\n");
	
	
	res = httpSend(ct);
	if(res < 0) {httpClearConn(ct); return -1;}
	
	res=recv(ct->httpFD,ct->httpBuf,HTTP_BUF,0);
	if(res <=0) {httpClearConn(ct); return -1;}
	
	return res;
}


int httpParse(httpclient_t ct)
{
	char *_nonce=strstr(ct->httpBuf,"nonce"); 
	char *_conn=strstr(ct->httpBuf,"Connection");
	char *_realm=strstr(ct->httpBuf,"realm");
	char *_head = strchr(_nonce,'"');
	char *_tail = strchr(_head+2,'"');
	unsigned char nonce_len=_tail-_head-1;
	
	ct->auth->nonce = (char *) calloc(1,nonce_len+1);
	strncpy(ct->auth->nonce,_head+1,nonce_len);
	
	_head = strchr(_realm,'"');
	_tail = strchr(_head+2,'"');
	
	ct->auth->realm = (char *) calloc(1,_tail-_head-1+1);
	strncpy(ct->auth->realm,_head+1,_tail-_head-1);
	
	if(_conn == NULL ) return -1;
	if( 0 == strncmp(_conn+12,"keep-alive",10)) return  1;
	if( 0 == strncmp(_conn+12,"close",5)) return  0;
	return -2;
}
int generate_auth(httpclient_t ct,char *uri)
{
	char ha1Buf[33];
	char ha2Buf[33];
	unsigned char tmp[16];
	int res;
	int cnonce = rand();
	
	res = httpParse(ct);
	memset(ct->httpBuf,'\0',HTTP_BUF);
	sprintf((char*)ct->httpBuf, "%s:%s:%s",ct->auth->user,ct->auth->realm,ct->auth->pass);
	md5_hash((unsigned char*)ct->httpBuf, strlen(ct->httpBuf), tmp);
	hex_from_raw((const char*)tmp, 16, ha1Buf);
	
	
	snprintf((char*)ct->httpBuf, HTTP_BUF, "%s:%s",ct->method,uri);
	md5_hash((unsigned char*)ct->httpBuf, strlen(ct->httpBuf), tmp);
	hex_from_raw((const char*)tmp, 16, ha2Buf);
	
	memset(ct->httpBuf,'\0',HTTP_BUF);
	sprintf((char*)ct->httpBuf, "%s:%s:00000001:%08x:auth:%s",\
					ha1Buf, ct->auth->nonce, cnonce, ha2Buf);
	md5_hash((unsigned char*)ct->httpBuf, strlen(ct->httpBuf), tmp);
	hex_from_raw((const char*)tmp, 16, ha1Buf);
	
	
	snprintf(ct->httpBuf, HTTP_BUF, "Digest username=\"%s\", realm=\"%s\", "
					"nonce=\"%s\", uri=\"%s\", response=\"%s\", qop=auth, "
					"nc=00000001, cnonce=\"%08x\"",
					ct->auth->user, ct->auth->realm, ct->auth->nonce, uri, ha1Buf, cnonce);
	return res;
}


int 	httpClientGet(httpclient_t ct,char *uri,int doorflag)
{
    int len;
    struct sockaddr_in address;
    int res;
	
	const char *h_header="User-Agent: FHJT_Http_Client\r\nAccept: */*\r\nConnection: Keep-alive\r\nAccept-Encoding: identity\r\n";
	
	ct->httpFD = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ct->auth->serverIP);//inet_addr()完成地址格式转换
    address.sin_port = htons(80);//端口
    len = sizeof(address);
	res = connect(ct->httpFD, (struct sockaddr *)&address, len);
	if(res == -1) {
        perror("failed when connect");
		LOGRECORD("failed when connect");
		ct->httpST = connect_NONE;
		httpClearConn(ct);
        return -1;
    }
	ct->method = strdup("GET");
	sprintf(ct->header,"GET %s HTTP/1.1\r\n",uri);
	len = strlen(ct->header);
	sprintf(ct->header+len,"Host: %s\r\n",ct->auth->serverIP);
	strcat(ct->header,h_header);
	strcat(ct->body,"\r\n");
	res = httpSend(ct);
	if(res < 0) {httpClearConn(ct); return -1;}
	
	res=recv(ct->httpFD,ct->httpBuf,HTTP_BUF,0);
	if(res <=0) {httpClearConn(ct); return -1;}
		
	res = generate_auth(ct,uri); 
	if(res == -1) {httpClearConn(ct);return -1;};
	if(res == 0) {
		printf("*******closeing\n\r");
		res=recv(ct->httpFD,ct->httpBuf,HTTP_BUF,0);
		if(res ==0) {close(ct->httpFD);}
		ct->httpFD = socket(AF_INET, SOCK_STREAM, 0);
		len = sizeof(address);
		res = connect(ct->httpFD, (struct sockaddr *)&address, len);
		if(res == -1) {
			perror("failed when connect");
			ct->httpST = connect_NONE;
			httpClearConn(ct);
			return -1;
		}
	}
	
	if(res == 1) printf("****keep alive\n\r");
	
	strcat(ct->header,"Authorization: ");
	strcat(ct->header,ct->httpBuf);
	strcat(ct->header,"\r\n");
	res = httpSend(ct);
	if(res < 0) {httpClearConn(ct); return -1;}
	
	//int enternum,exitnum;
	char tmp[128];
	while(true)
	{
		memset(tmp,0,sizeof(tmp));
		res=recv(ct->httpFD,ct->httpBuf,HTTP_BUF,0);
		*((ct->httpBuf)+HTTP_BUF) = '\0';
		if((strstr(ct->httpBuf,"<enter>")!=NULL) && (strstr(ct->httpBuf,"<exit>")!=NULL) && (strstr(ct->httpBuf,"<RealTime>")!=NULL))
		{
			printf("received realtime pdc num.res = %d\n",res);
			//printf("%s\n",ct->httpBuf);
			//printf("received end.\n\n");
			if(doorflag == 0)
			{
				front_enternum = getpdcenter(ct->httpBuf);
				front_exitnum = getpdcexit(ct->httpBuf);
				printf("front_enternum = %d,front_exitnum = %d.\n",front_enternum,front_exitnum);
				snprintf(tmp,sizeof(tmp),"front_enternum = %d,front_exitnum = %d.",front_enternum,front_exitnum);
			}
			else
			{
				back_enternum = getpdcenter(ct->httpBuf);
				back_exitnum = getpdcexit(ct->httpBuf);
				printf("back_enternum = %d,back_exitnum = %d.\n",back_enternum,back_exitnum);
				snprintf(tmp,sizeof(tmp),"back_enternum = %d,back_exitnum = %d.",back_enternum,back_exitnum);
			}
			LOGRECORD(tmp);
		}
		memset(ct->httpBuf,0,HTTP_BUF);
		if(res<0)
		{
			break;
		}
	}

	if(res <=0) {httpClearConn(ct); return -1;}
	
	return res;

}
int 	httpSend(httpclient_t ct)
{
	int res =0 ;
	
	snprintf(ct->httpBuf,HTTP_BUF,"%s",ct->header);
	strcat(ct->httpBuf,ct->body);
	res = send(ct->httpFD,ct->httpBuf,strlen(ct->httpBuf),0);
	memset(ct->httpBuf,'\0',HTTP_BUF);
	return res;
}

httpclient_t 	httpClientCreat(char *ip,char *user,char *passwd)
{
	httpclient_t ct = (httpclient_t) calloc(1,sizeof(struct httpclient_st));
	ct->auth = (http_auth_t) calloc(1,sizeof(struct http_auth_st));
	ct->auth->serverIP = strdup(ip);
	if(user && passwd){
		ct->auth->user = strdup(user);
		ct->auth->pass = strdup(passwd);
	}
	ct->header = (char *)calloc(1,1024);
	ct->body = (char *)calloc(1,1024);
	ct->httpBuf = (char *)calloc(1,HTTP_BUF);
	ct->httpFD = 0;
	return ct;
}
int    httpclientFree(httpclient_t ct)
{
	if(ct->httpFD) 		close(ct->httpFD);
	if(ct->method) 		{free(ct->method);ct->method=NULL;}
	if(ct->header) 		{free(ct->header);ct->header=NULL;}
	if(ct->body) 		{free(ct->body);ct->body= NULL;}
	if(ct->httpBuf) 	{free(ct->httpBuf);ct->httpBuf= NULL;}
	if(ct->auth->user) 	{free(ct->auth->user);ct->auth->user= NULL;}
	if(ct->auth->pass) 	{free(ct->auth->pass);ct->auth->pass= NULL;}
	if(ct->auth->serverIP) 	{free(ct->auth->serverIP);ct->auth->serverIP= NULL;}
	if(ct->auth->nonce) 	{free(ct->auth->nonce);ct->auth->nonce= NULL;}
	if(ct->auth->realm) 	{free(ct->auth->realm);ct->auth->realm= NULL;}
	
	if(ct->auth) {free(ct->auth);ct->auth = NULL;}
	if(ct) {free(ct);}
	
	return 0;
}

int httpClearConn(httpclient_t ct)
{
	if(ct->httpFD)		{close(ct->httpFD); ct->httpFD=0;}
	if(ct->method) 		{free(ct->method);ct->method=NULL;}
	if(ct->header) 		{memset((ct->header),'\0',1024);}
	if(ct->body) 		{memset((ct->body),'\0',1024);}
	if(ct->httpBuf) 	{memset((ct->httpBuf),'\0',HTTP_BUF);}
	
	if(ct->auth->nonce) 	{free(ct->auth->nonce);ct->auth->nonce= NULL;}
	if(ct->auth->realm) 	{free(ct->auth->realm);ct->auth->realm= NULL;}
	return 0;
}
void clearpdcnum(char* ip,char* usrname,char* passwd)
{
	const char* isapi = "/ISAPI/System/Video/inputs/channels/1/counting/resetCount";
	httpclient_t 	ct = httpClientCreat(ip,usrname,passwd);
	//printf("httpclient creat.\n");
	//httpClientGet(ct,"/ISAPI/System/deviceInfo");
	httpClientPut(ct,(char*)isapi,NULL);
	//printf("httpclient put.\n");
	httpClearConn(ct);
	httpclientFree(ct);
}
