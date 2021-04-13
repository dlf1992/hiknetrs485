/******************************************************************
* Copyright(c) 2020-2028 ZHENGZHOU Tiamaes LTD.
* All right reserved. See COPYRIGHT for detailed Information.
*
* @fileName: threadmanage.cpp
* @brief: 线程管理类实现
* @author: dinglf
* @date: 2020-05-19
* @history:
*******************************************************************/
#include "threadmanage.h"

ThreadManage* ThreadManage::m_pthreadmanage = NULL;
//静态成员变量初始化
ThreadManage::ThreadInfoMap ThreadManage::m_threadinfomap;
ThreadManage::ThreadMap ThreadManage::m_threadmap;


void ThreadManage::AddThreadmap(unsigned long tid,string threadname)
{
	//先查找，有就更新，没有就添加
	//printf("tid = %lu\n",tid);
	//threadmaplock.mutex_lock();
	ThreadMap::iterator iter;
	iter = m_threadmap.find(tid);
	if(iter != m_threadmap.end())
	{
		//printf("AddThreadmap found tid.\n");
		if(iter->second == threadname)
		{
			//printf("tid and threadname exist.\n");
		}
		else
		{
			//printf("threadname does not match tid.\n");
			m_threadmap[tid] = threadname;
		}
	}
	else
	{
		//printf("AddThreadmap not found tid.\n");
		m_threadmap.insert(make_pair(tid,threadname));
	}
	//threadmaplock.mutex_unlock();
}
void ThreadManage::DelThreadmap(unsigned long tid)
{
	//查找，移除
	//threadmaplock.mutex_lock();
	ThreadMap::iterator iter;
	iter = m_threadmap.find(tid);
	if(iter != m_threadmap.end())
	{
		//printf("found tid.\n");
		m_threadmap.erase(iter);
	}
	else
	{
		//printf("not found tid.\n");
	}
	//threadmaplock.mutex_unlock();	
}
void ThreadManage::ClearThreadmap()
{
	//threadmaplock.mutex_lock();
	m_threadmap.clear();
	//threadmaplock.mutex_unlock();		
}
bool ThreadManage::IsThreadRun(unsigned long tid)
{
	//printf("IsThreadRun,tid = %lu\n",tid);
	int kill_rc = pthread_kill(tid,0);
	if(kill_rc == ESRCH)
	{
		//printf("thread did not exists or already quit\n");	
		return false;
	}
	else if(kill_rc == EINVAL)
	{
		//printf("signal is invalid\n");	
		return false;		
	}
	else
	{
		//printf("thread is run.\n");	
		return true;
	}
}
void ThreadManage::AddThreadInfomap(string threadname,Thread *ppthread,ThreadFunc pfunc,void *param)
{
	//添加threadinfomap
	ThreadInfo *pthreadinfo = NULL;
	//printf("threadname = %s,ppthread = %p,pfunc = %p\n",threadname.c_str(),ppthread,pfunc);
	//printf("threadname = %s,ppthread = %p,pfunc = %p\n",threadname.c_str(),ppthread,pfunc);
	ThreadInfoMap::iterator iter;
	iter = m_threadinfomap.find(threadname);
	if(iter != m_threadinfomap.end())
	{
		//printf("AddThreadInfomap found threadname.\n");
		pthreadinfo = iter->second;
		if(pthreadinfo != NULL)
		{
			if(ppthread != pthreadinfo->ppthread)
			{
				pthreadinfo->ppthread = ppthread;
			}
			if(pfunc != pthreadinfo->pfunc)
			{
				pthreadinfo->pfunc = pfunc;
			}
			if(param != pthreadinfo->param)
			{
				pthreadinfo->param = param;
			}			
			pthreadinfo->count++;	
			pthreadinfo->restartflag = true;		
		}
		else
		{
			pthreadinfo = new ThreadInfo;
			if(pthreadinfo != NULL)
			{
				pthreadinfo->ppthread = ppthread;
				pthreadinfo->pfunc = pfunc;
				pthreadinfo->param = param;
				pthreadinfo->count = 0;
				pthreadinfo->restartflag = true;
			}
		}		
		//printf("ppthread does not match threadname.\n");
		m_threadinfomap[threadname] = pthreadinfo;
	}
	else
	{
		//printf("AddThreadInfomap not found threadname.\n");
		pthreadinfo = new ThreadInfo;
		if(pthreadinfo != NULL)
		{
			pthreadinfo->ppthread = ppthread;
			pthreadinfo->pfunc = pfunc;
			pthreadinfo->param = param;
			pthreadinfo->count = 0;
			pthreadinfo->restartflag = true;
		}
		m_threadinfomap.insert(make_pair(threadname,pthreadinfo));
	}	
}
void ThreadManage::UpdateThreadInfomap(string threadname)
{
	//子线程退出后，将线程类指针对象delete
	ThreadInfo *pthreadinfo = NULL;
	//printf("threadname = %s.",threadname.c_str());
	ThreadInfoMap::iterator iter;
	iter = m_threadinfomap.find(threadname);
	if(iter != m_threadinfomap.end())
	{
		//printf("found threadname.\n");
		pthreadinfo = iter->second;
		if(NULL == pthreadinfo)
		{
			//printf("pthreadinfo=NULL\n");	
			return;
		}
		if(NULL == pthreadinfo->ppthread)
		{
			//printf("ppthread=NULL\n");	
			return;			
		}
		delete pthreadinfo->ppthread;
		pthreadinfo->ppthread = NULL;
	}
	else
	{
		//printf("not found threadname.\n");
	}		
}
void ThreadManage::ClearThreadInfomap()
{
	//清空map
	ThreadInfo *pthreadinfo = NULL;
	ThreadInfoMap::iterator iter;
	for (iter = m_threadinfomap.begin();iter != m_threadinfomap.end();)
	{
		pthreadinfo = iter->second;
		//printf("threadname = %s,pthreadinfo = %p.\n",(iter->first).c_str(),pthreadinfo);
		if(pthreadinfo != NULL)
		{
			if(pthreadinfo->ppthread != NULL)
			{
				delete pthreadinfo->ppthread;
				pthreadinfo->ppthread = NULL;				
			}
			delete pthreadinfo;
			pthreadinfo = NULL;			
		}
		m_threadinfomap.erase(iter++);
    }		
}
void ThreadManage::GetFuncByThreadname(string threadname,ThreadFunc &pfunc,void* &param)
{
	//通过threadname获取对应线程函数指针
	ThreadInfo *pthreadinfo = NULL;
	ThreadInfoMap::iterator iter;
	iter = m_threadinfomap.find(threadname);
	if(iter != m_threadinfomap.end())
	{
		//printf("found threadname.\n");
		pthreadinfo = iter->second;
		if(NULL == pthreadinfo)
		{
			//printf("pthreadinfo=NULL\n");	
			return;
		}
		pfunc = pthreadinfo->pfunc;
		param = pthreadinfo->param;
		//printf("pfunc = %p\n",pfunc);
	}
	else
	{
		//printf("not found threadname.\n");
	}		
}
bool ThreadManage::GetRestartFlagByName(string threadname)
{
	bool flag = false;
	ThreadInfo *pthreadinfo = NULL;
	ThreadInfoMap::iterator iter;
	iter = m_threadinfomap.find(threadname);
	if(iter != m_threadinfomap.end())
	{
		//printf("found threadname.\n");
		pthreadinfo = iter->second;
		if(NULL == pthreadinfo)
		{
			//printf("pthreadinfo=NULL\n");	
			return flag;
		}
		flag = pthreadinfo->restartflag;
	}
	else
	{
		//printf("not found threadname.\n");
	}
	return flag;
}
void ThreadManage::SetRestartFlagByName(string threadname,bool flag)
{
	if(threadname.empty())
	{
		//printf("threadname empty.\n");
		return;		
	}
	if(threadname.length() > 15)
	{
		//printf("threadname over 15.\n");
		threadname = threadname.substr(0,15);	
	}
	ThreadInfo *pthreadinfo = NULL;
	ThreadInfoMap::iterator iter;
	iter = m_threadinfomap.find(threadname);
	if(iter != m_threadinfomap.end())
	{
		//printf("found threadname.\n");
		pthreadinfo = iter->second;
		if(NULL == pthreadinfo)
		{
			//printf("pthreadinfo=NULL\n");	
			return;
		}
		pthreadinfo->restartflag = flag;
	}
	else
	{
		//printf("not found threadname.\n");
	}
}
void ThreadManage::SetRestartFlag(ThreadInfo *pthreadinfo,bool flag)
{	
	if(NULL == pthreadinfo)
	{
		//printf("pthreadinfo=NULL\n");	
		return;
	}
	pthreadinfo->restartflag = flag;
}
int ThreadManage::GetUnrunThread(ThreadMap &unrunthread)
{
	//遍历，判断，添加map，返回个数
	int ret = 0;
	//threadmaplock.mutex_lock();
	ThreadMap::iterator iter;
	for (iter = m_threadmap.begin();iter != m_threadmap.end(); ++iter)
	{
		//printf("m_threadmap loop\n");
		//printf("tid = %lu\n",iter->first);
	   if(!IsThreadRun(iter->first)) 
	   {
		   //线程没运行
		   //printf("unrunthread %d,tid = %lu\n",ret,iter->first);
		   unrunthread.insert(make_pair(iter->first,iter->second));
		   //printf("ret = %d\n",ret);
		   ret++;
	   }
    }
	//threadmaplock.mutex_unlock();
	//移除m_threadmap中unrunthread
	ThreadMap::iterator iter1;
	for (iter1 = unrunthread.begin();iter1 != unrunthread.end(); ++iter1)
	{
		DelThreadmap(iter1->first);
		UpdateThreadInfomap(iter1->second);
    }	
	return ret;
}
bool ThreadManage::CreatThread(string threadname,ThreadFunc threadfunc,void *param,size_t iSize)
{
	//先创建Thread指针对象，
	Thread *ppthread = NULL;
	unsigned long pthreadid  = -1;
	if(threadname.empty())
	{
		printf("threadname empty.\n");
		return false;		
	}
	if(threadname.length() > 15)
	{
		printf("threadname over 15.\n");
		threadname = threadname.substr(0,15);	
	}
	ppthread = new Thread(threadname,threadfunc,param,iSize);
	if(NULL == ppthread)
	{
		printf("ppthread=NULL.\n");
		return false;
	}
	if(!ppthread->ThreadStart())
	{
		printf("ppthread ThreadStart failed.\n");
		delete ppthread;
		ppthread = NULL;
		return false;
	}
	pthreadid = ppthread->GetThreadid();
	printf("pthreadid = %lu.\n",pthreadid);
	AddThreadmap(pthreadid,threadname);
	AddThreadInfomap(threadname,ppthread,threadfunc,param);
	return true;	
}
void ThreadManage::RestartThread()
{
	//printf("RestartThread.\n");
	bool RestartFlag = false;
	ThreadFunc pfunc = NULL;
	void *param = NULL;
	ThreadMap m_unrunthreadmap;
	m_unrunthreadmap.clear();
	int ret = 0;
	ret = GetUnrunThread(m_unrunthreadmap);
	if(ret > 0)	
	{
		//printf("unrunthread exist,num = %d.\n",ret);
		ThreadMap::iterator iter;
		for (iter = m_unrunthreadmap.begin();iter != m_unrunthreadmap.end(); ++iter)
		{
			//printf("threadid = %lu,threadname = %s\n",iter->first,(iter->second).c_str());
			RestartFlag = GetRestartFlagByName(iter->second);
			if(!RestartFlag)
			{
				//printf("threadname = %s,RestartFlag is false.\n",(iter->second).c_str());
				continue;
			}
			GetFuncByThreadname(iter->second,pfunc,param);
			if(pfunc != NULL)
			{
				if(!CreatThread(iter->second,pfunc,param,DEF_STACK_SIZE))
				{
					//printf("thread %s creat error.\n",(iter->second).c_str());
				}
			}
		}
		
	}	
}
void ThreadManage::StopThreadByName(string threadname,bool flag)
{
	//通过线程名停止线程，flag表示是否需要重新拉起
	ThreadInfo *pthreadinfo = NULL;
	ThreadInfoMap::iterator iter;
	iter = m_threadinfomap.find(threadname);
	if(iter != m_threadinfomap.end())
	{
		pthreadinfo = iter->second;
		//printf("found threadname,pthreadinfo = %p.\n",pthreadinfo);
		if(NULL == pthreadinfo)
		{
			//printf("pthreadinfo=NULL\n");	
			return;
		}
		SetRestartFlag(pthreadinfo,flag);
		if(NULL == pthreadinfo->ppthread)
		{
			//printf("ppthread=NULL\n");	
			return;			
		}		
		pthreadinfo->ppthread->ThreadStop();					
	}
	else
	{
		//printf("not found threadname.\n");
	}		
}