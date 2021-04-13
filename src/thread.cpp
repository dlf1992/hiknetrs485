/******************************************************************
* Copyright(c) 2020-2028 ZHENGZHOU Tiamaes LTD.
* All right reserved. See COPYRIGHT for detailed Information.
*
* @fileName: thread.cpp
* @brief: 线程类实现
* @author: dinglf
* @date: 2020-05-21
* @history:
*******************************************************************/
#include "thread.h"

void Thread::GetThreadName(string &threadname)
{
	//获取线程名字
	threadname = m_threadname;
}
unsigned long Thread::GetThreadid()
{
	//获取线程id
	return m_tid;
}
bool Thread::ThreadStart()
{
	//创建启动线程
	pthread_attr_t attr;

	pthread_attr_init(&attr);

	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);//绑定

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//分离

    int status = pthread_attr_setstacksize(&attr, m_stacksize);
    if (status != 0)
    {
        printf("pthread_attr_setstacksize, errno-%d err-%s!\n",errno,strerror(errno));
        //printf("pthread_attr_setstacksize, errno-%d err-%s!\n",errno,strerror(errno));
        return false;
    }
	
	if(NULL == m_pFunc)
	{
		printf("m_pFunc=NULL.\n");
		//printf("m_pFunc=NULL.\n");
		return false;
	}
	if(pthread_create(&m_tid, &attr, m_pFunc, m_param) == 0)//创建线程
	{
		//printf("m_tid = %lu\n",m_tid);
		pthread_attr_destroy(&attr);
		return true;
	}

	pthread_attr_destroy(&attr);

	return false;		
}
bool Thread::ThreadStop()
{
	//外部停止线程
	if(m_tid == 0)
	{
		//printf("m_tid=0.\n");
		return false;
	}
	if(pthread_cancel(m_tid) != 0)
	{
		printf("pthread_cancel, errno-%d err-%s!\n",errno,strerror(errno));
		return false;
	}
	m_tid = 0;
	return true;
}