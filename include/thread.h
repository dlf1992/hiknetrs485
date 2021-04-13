/******************************************************************
* Copyright(c) 2020-2028 ZHENGZHOU Tiamaes LTD.
* All right reserved. See COPYRIGHT for detailed Information.
*
* @fileName: thread.h
* @brief: 线程类
* @author: dinglf
* @date: 2020-05-21
* @history:
*******************************************************************/
#ifndef _THREAD_
#define _THREAD_
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "pub.h"
//#include "zlog_api.h"

using namespace std;

#define DEF_STACK_SIZE 2*1024*1024

typedef	void *(*ThreadFunc)(void *);
class Thread
{
private:
	pthread_t m_tid;  //线程id
	string m_threadname; //线程名字
	ThreadFunc m_pFunc;	//线程函数指针
	void *m_param;      //参数
	size_t m_stacksize; //堆栈大小
public:
	Thread()
	{}
	Thread(string threadname,ThreadFunc threadfunc,void *param=NULL,size_t iSize=DEF_STACK_SIZE):m_tid(0)
	{
		m_threadname = threadname;
		m_pFunc = threadfunc;
		m_param = param;
		m_stacksize = iSize;
	}
	~Thread()
	{
		m_tid = 0;
		m_pFunc = NULL;
	}
	void GetThreadName(string &threadname);
	unsigned long GetThreadid();
	bool ThreadStart();	
	bool ThreadStop();
};
#endif