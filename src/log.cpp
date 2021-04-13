#include "log.h"

Logger *plog = NULL;

void FileSystemLogger::gettime(char *mtime)
{
	int year,month,day,hour,minute,second;
	time_t t = time(NULL);
	struct tm* stime=localtime(&t);
	year = 1900+stime->tm_year;
	month = 1+stime->tm_mon;
	day = stime->tm_mday;
	hour = stime->tm_hour;
	minute = stime->tm_min;
	second = stime->tm_sec;
	char tmp[64];
	memset(tmp,0,sizeof(tmp));
	snprintf(tmp,sizeof(tmp),"%04d-%02d-%02d %02d:%02d:%02d",year,month,day,hour,minute,second);	
	memcpy(mtime,tmp,strlen(tmp));
} 
////////////////////////////////////////////////////////////////////////
bool INITLOG(char* logpath)
{
	plog = new FileSystemLogger(logpath);
	if(NULL == plog)
	{
		return false;
	}
	return true;
}
void LOGRECORD(const char* data)
{
	if(NULL == plog)
	{
		return;
	}	
	plog->write(data);
}
