/******************************************************************
* Copyright(c) 2020-2028 ZHENGZHOU Tiamaes LTD.
* All right reserved. See COPYRIGHT for detailed Information.
*
* @fileName: threadmanage.h
* @brief: 线程管理类
* @author: dinglf
* @date: 2020-05-19
* @history:
*******************************************************************/
#ifndef _THREAD_MANAGE_
#define _THREAD_MANAGE_
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/prctl.h>
#include "thread.h"

using namespace std;

typedef struct stThreadinfo
{
	Thread *ppthread;
	ThreadFunc pfunc;
	void *param;
	unsigned int count;
	bool restartflag;
}__attribute__ ((__packed__)) ThreadInfo;
 
class ThreadManage
{
private:
	static ThreadManage *m_pthreadmanage;
	typedef map<string,ThreadInfo*> ThreadInfoMap;
	typedef map<unsigned long,string> ThreadMap;
	static ThreadInfoMap m_threadinfomap;   //存储线程名和线程信息
	static ThreadMap m_threadmap;	//存储tid和线程名
private:
	ThreadManage()
	{}
	~ThreadManage()
	{
	}
	class CGarbo //在析构函数中删除单例
    {
        public:
            ~CGarbo()
            {
                if(ThreadManage::m_pthreadmanage != NULL)
                {
                	delete ThreadManage::m_pthreadmanage;
					ThreadManage::m_pthreadmanage = NULL;
                }
            }
     };
     static CGarbo Garbo; //定义一个静态成员，程序结束时，系统会自动调用它的析构函数			
public:
	static ThreadManage* GetInstance()
	{
		if (NULL == m_pthreadmanage) 
		{
			m_pthreadmanage = new ThreadManage;
		}
		return m_pthreadmanage;		
	}
	static void AddThreadmap(unsigned long tid,string threadname);	//添加threadmap,threadname不能超过15字节，否则显示不全
	static void DelThreadmap(unsigned long tid);  //移除threadmap
	static void ClearThreadmap();	//清空threadmap
	static void AddThreadInfomap(string threadname,Thread *ppthread,ThreadFunc pfunc,void *param);
	static void UpdateThreadInfomap(string threadname);
	static void ClearThreadInfomap();
	void GetFuncByThreadname(string threadname,ThreadFunc &pfunc,void* &param);
	bool GetRestartFlagByName(string threadname);
	void SetRestartFlagByName(string threadname,bool flag);
	void SetRestartFlag(ThreadInfo *pthreadinfo,bool flag);
	bool IsThreadRun(unsigned long tid);	//判断线程是否正常运行
	int GetUnrunThread(ThreadMap &unrunthread); //获取threadmap中不运行的线程
	bool CreatThread(string threadname,ThreadFunc threadfunc,void *param=NULL,size_t iSize=DEF_STACK_SIZE);
	void RestartThread();
	void StopThreadByName(string threadname,bool flag);
};
#endif