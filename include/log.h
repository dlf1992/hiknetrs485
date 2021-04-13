#ifndef _LOG_H_
#define _LOG_H_

#include <fstream> 
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

// 抽象日志接口
class Logger{
public:
    virtual void write(const char* p) = 0;// 纯虚函数
};
 
class FileSystemLogger :public Logger{
public:
    FileSystemLogger(){
		char defaultpath[64] = "./log.txt";
        this->path = defaultpath;
    }
 
    FileSystemLogger(char *path){
        this->path = path;
    }
 
    void write(const char* p){
        ofstream outFile(path,ios::app);
		char mtime[64] = {0};
		gettime(mtime);
		outFile << mtime << "  ";
        outFile << p << endl;
    }
 
    ~FileSystemLogger(){}
	void gettime(char *mtime);
private:
    char *path;
};

#ifdef __cplusplus
extern "C" 
{
#endif
extern bool INITLOG(char* logpath);
extern void LOGRECORD(const char* data);
#ifdef __cplusplus
}  /* end extern "C" */
#endif



#endif
