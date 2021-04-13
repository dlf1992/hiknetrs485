/*  Make the necessary includes and set up the variables.  */

#include <sys/types.h> 
#include <sys/stat.h>

#include "httpclient.h"
#include "threadmanage.h"
#include "KLRs485Service.h"
#include "log.h"
#include "inifile.h"
using namespace inifile;


ThreadManage *pthreadmanage = NULL;
KLRs485Service *pklrs485service = NULL;
IniFile *pIniFile = NULL;

bool  InitObject()
{
	pthreadmanage = ThreadManage::GetInstance();
	if(NULL == pthreadmanage)
	{
		printf("pthreadmanage create failed.\n");
		return false;
	}	
	pklrs485service = new KLRs485Service;
	if(NULL == pklrs485service)
	{
		printf("pklrs485service create failed.\n");
		return false;
	}	
	pIniFile = new IniFile;
	if(NULL == pIniFile)
	{
		printf("pIniFile create failed.\n");
		return false;
	}
	return true;
}
bool initInifile(const char* inifile)
{
	string ini_file = inifile;
	int ret;
	if((ret = pIniFile->Load(ini_file)) != 0)
	{
		printf("load inifile failed,ret = %d\n",ret);
		return false;
	}	
	return true;
}
void initcameraparam()
{
	memset(&front_camerainfo,0,sizeof(CAMERAINFO));
	memset(&back_camerainfo,0,sizeof(CAMERAINFO));
	string frontip,frontusrname,frontpasswd;
	string backip,backusrname,backpasswd;
	const char* inifile = "/app/camera.ini";
	if(initInifile(inifile))
	{
		pIniFile->GetStringValue("FRONTCAMERA","IP",&frontip);
		pIniFile->GetStringValue("FRONTCAMERA","USRNAME",&frontusrname);
		pIniFile->GetStringValue("FRONTCAMERA","PASSWD",&frontpasswd);
		pIniFile->GetStringValue("BACKCAMERA","IP",&backip);
		pIniFile->GetStringValue("BACKCAMERA","USRNAME",&backusrname);
		pIniFile->GetStringValue("BACKCAMERA","PASSWD",&backpasswd);
	}
	else
	{
		frontip = "192.168.1.64";
		frontusrname = "admin";
		frontpasswd = "hikvision123";
		backip = "192.168.1.65";
		backusrname = "admin";
		backpasswd = "hikvision123";		
	}
	printf("frontip = %s\nfrontusrname = %s\nfrontpasswd = %s\n",frontip.c_str(),frontusrname.c_str(),frontpasswd.c_str());
	printf("backip = %s\nbackusrname = %s\nbackpasswd = %s\n",backip.c_str(),backusrname.c_str(),backpasswd.c_str());

	strcpy(&front_camerainfo.cameraip[0],frontip.c_str());
	strcpy(&front_camerainfo.usrname[0],frontusrname.c_str());
	strcpy(&front_camerainfo.passwd[0],frontpasswd.c_str());

	strcpy(&back_camerainfo.cameraip[0],backip.c_str());
	strcpy(&back_camerainfo.usrname[0],backusrname.c_str());
	strcpy(&back_camerainfo.passwd[0],backpasswd.c_str());

	front_enternum = 0;
	front_exitnum = 0;
	back_enternum = 0;
	back_exitnum = 0;
}
void getpdcnum(char* ip,char* usrname,char* passwd,int doorflag)
{
	const char* isapi = "/ISAPI/Event/notification/alertStream";
	httpclient_t 	ct = httpClientCreat(ip,usrname,passwd);
	//httpClientGet(ct,"/ISAPI/System/deviceInfo");
	httpClientGet(ct,(char*)isapi,doorflag);
	//printf("httpClientGet,doorflag = %d\n",doorflag);
	httpClearConn(ct);
	//printf("httpClearConn,doorflag = %d\n",doorflag);
	httpclientFree(ct);
	//printf("httpclientFree,doorflag = %d\n",doorflag);
}
static void *getfrontnum(void *arg)
{
	prctl(PR_SET_NAME,"getfrontnum");
	static unsigned int count = 0;
	while(true)
	{
		char m_ip[16] = {0};
		char m_usrname[32] = {0};
		char m_passwd[32] = {0};
		char tmp[128];
		memcpy(&m_ip,&(((CAMERAINFO*)arg)->cameraip),strlen(((CAMERAINFO*)arg)->cameraip));	
		memcpy(&m_usrname,&(((CAMERAINFO*)arg)->usrname),strlen(((CAMERAINFO*)arg)->usrname));
		memcpy(&m_passwd,&(((CAMERAINFO*)arg)->passwd),strlen(((CAMERAINFO*)arg)->passwd));
		printf("getfrontnum,m_ip = %s,m_usrname = %s,m_passwd = %s.\n",m_ip,m_usrname,m_passwd);
		memset(tmp,0,sizeof(tmp));
		snprintf(tmp,sizeof(tmp),"getfrontnum,m_ip = %s,m_usrname = %s,m_passwd = %s.",m_ip,m_usrname,m_passwd);
		LOGRECORD(tmp);
		getpdcnum(m_ip,m_usrname,m_passwd,0);
		memset(tmp,0,sizeof(tmp));
		sprintf(tmp,"getfrontnum count = %d",count);
		LOGRECORD(tmp);
		count++;	
		sleep(5);
	}
	//printf("getfrontnum\n");
	return NULL;
}
static void *getbacknum(void *arg)
{
	prctl(PR_SET_NAME,"getbacknum");
	static unsigned int count = 0;
	while(true)
	{
		char m_ip[16] = {0};
		char m_usrname[32] = {0};
		char m_passwd[32] = {0};
		char tmp[128];
		memcpy(&m_ip,&(((CAMERAINFO*)arg)->cameraip),strlen(((CAMERAINFO*)arg)->cameraip));	
		memcpy(&m_usrname,&(((CAMERAINFO*)arg)->usrname),strlen(((CAMERAINFO*)arg)->usrname));
		memcpy(&m_passwd,&(((CAMERAINFO*)arg)->passwd),strlen(((CAMERAINFO*)arg)->passwd));
		printf("getbacknum,m_ip = %s,m_usrname = %s,m_passwd = %s.\n",m_ip,m_usrname,m_passwd);
		snprintf(tmp,sizeof(tmp),"getbacknum,m_ip = %s,m_usrname = %s,m_passwd = %s.",m_ip,m_usrname,m_passwd);
		LOGRECORD(tmp);
		getpdcnum(m_ip,m_usrname,m_passwd,1);
		memset(tmp,0,sizeof(tmp));
		sprintf(tmp,"getbacknum count = %d",count);
		LOGRECORD(tmp);
		count++;	
		sleep(5);		
	}
	//printf("getbacknum\n");
	return NULL;
}
void  clearfrontnum()
{
	char m_ip[16] = {0};
	char m_usrname[32] = {0};
	char m_passwd[32] = {0};
	memcpy(&m_ip,&(front_camerainfo.cameraip),strlen(front_camerainfo.cameraip));	
	memcpy(&m_usrname,&(front_camerainfo.usrname),strlen(front_camerainfo.usrname));
	memcpy(&m_passwd,&(front_camerainfo.passwd),strlen(front_camerainfo.passwd));	
	clearpdcnum(m_ip,m_usrname,m_passwd);
}
void clearbacknum()
{
	char m_ip[16] = {0};
	char m_usrname[32] = {0};
	char m_passwd[32] = {0};
	memcpy(&m_ip,&(back_camerainfo.cameraip),strlen(back_camerainfo.cameraip));	
	memcpy(&m_usrname,&(back_camerainfo.usrname),strlen(back_camerainfo.usrname));
	memcpy(&m_passwd,&(back_camerainfo.passwd),strlen(back_camerainfo.passwd));
	clearpdcnum(m_ip,m_usrname,m_passwd);
}
void  clearnum()
{
	clearfrontnum();
	clearbacknum();
}
static void *getrs485(void *arg)
{
	prctl(PR_SET_NAME,"getrs485");
	while(true)
	{
		pklrs485service->Process();
	}
	//printf("getrs485\n");
	return NULL;
}
void  ThreadStart()
{
	
	if(!pthreadmanage->CreatThread("getfrontnum",getfrontnum,(void*)(&front_camerainfo)))
	{
		//printf("thread getfrontnum creat error.\n");
		LOGRECORD("thread getfrontnum creat error.");
	}
	if(!pthreadmanage->CreatThread("getbacknum",getbacknum,(void*)(&back_camerainfo)))
	{
		printf("thread getbacknum creat error.\n");
		LOGRECORD("thread getbacknum creat error.");
	}
	if(!pthreadmanage->CreatThread("getrs485",getrs485))
	{
		printf("thread getrs485 creat error.\n");
		LOGRECORD("thread getrs485 creat error.");
	}
}
int getfilesize(const char* file)
{
	std::string str = file;
	struct stat st;
	if(stat(str.c_str(), &st) != 0)
	{
		return 0;
	}

	FILE *pfile = NULL;
	pfile = fopen(str.c_str(), "rb");

	if(pfile)
	{
		/// 将文件指针移动到文件尾
		fseek(pfile, 0, SEEK_END);
		unsigned int file_length_bytes = ftell(pfile);
		fclose(pfile);
		pfile = NULL;
		return file_length_bytes;
	}
	else
	{
		return -1;/// pfile 文件指针为null
	}

}


int main(int argc, char *argv[])
{
	char logfile[64] = "/tmp/log.txt";
	char logfilenew[64] = "/tmp/log.txt0";
	if(getfilesize(logfile) >= 3*1024*1024)
	{
		rename(logfile,logfilenew);
	}

	//初始化log
	if(!INITLOG(logfile))
	{
		printf("Initlog  failed\n");
		return -1;		
	}
	LOGRECORD("program start.");
	//全局指针对象初始化
	if(!InitObject())
	{
		LOGRECORD("InitObject  failed.");
		return -1;
	}
	//初始化参数
	initcameraparam();
	//开启485串口
	pklrs485service->Start();
	//清除摄像机人数
	clearnum();
	//printf("thread start.\n");
	//线程启动
	ThreadStart();
	static unsigned int count = 0;
	char tmp[64];
	while(1)
	{
		pthreadmanage->RestartThread();
		memset(tmp,0,sizeof(tmp));
		sprintf(tmp,"main count = %d",count);
		LOGRECORD(tmp);
		count++;		
		sleep(10);
	}		
	return 0;
}
