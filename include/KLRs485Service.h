#ifndef __KLRS485SERVICE_H__
#define __KLRS485SERVICE_H__

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
//#include <algorithm>


//#include "ttyProtocol.h"
//#include "NetProtocol.h"
#include "rsprotocol.h"
#include "httpclient.h"


using namespace std;

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define TRUE true
#define FALSE false

#define RS485COM "/dev/ttyS1"

typedef unsigned char XnUInt8;
typedef int XnInt32;
typedef unsigned int XnUInt32;
typedef bool XnBool;
typedef unsigned short XnUInt16;

unsigned int CRC16_isr(unsigned char *Dat, unsigned int len);
std::string charToStr(char * contentChar);
void modifyContentInFile(std::string fileName,int lineNum,char *content);

class KLRs485Service
{
public:
    enum
    {
        CMD_DEV_HAND       = 0xC5,
        CMD_SET_DEV_ADDR   = 0xC6,
        CMD_SET_DOOR_STATE = 0xCA,
        CMD_GET_COUNT      = 0xC7,
        CMD_CLIEAR_COUNT   = 0xC8,
        CMD_GET_VERSION    = 0xC9,
    };
public:
    KLRs485Service();
    ~KLRs485Service();
    
public:
    //void Start(param_cfg cfg);
    void Start();
    void Stop();

    XnBool DoorIsOpened();
    void Process();
	//void SetNetProtocol(NetProtocol *DealNetObj){m_DealNetObj = DealNetObj;}
	XnUInt32 GetTimeMs();
    
private:
    virtual void InitRs485Dev();
    virtual void UninitRs485Dev();
    virtual XnInt32 Rs485Read(XnUInt8 *buf, XnUInt32 size);
    virtual XnInt32 Rs485Write(XnUInt8 *data, XnUInt32 len);
    //int Rs485Write_ex(const unsigned char* data, unsigned int len);
	void SetCfgParaDoor(XnUInt8 DoorIndex);
	
    void Parse(const XnUInt8 *data, XnUInt32 len, XnUInt32 timeStamp);
    XnUInt8 ProcSetDoorStateReq(const XnUInt8 *data, XnUInt32 len);
    XnUInt8 ProcGetCountReq(const XnUInt8 *data, XnUInt32 len);
    XnUInt8 ProcClearCountReq(const XnUInt8 *data, XnUInt32 len);
	XnUInt8 ProcShakeHandsReq(const XnUInt8 *data, XnUInt32 len);
    XnUInt8 ProcSetDevAddrReq(const XnUInt8 *data, XnUInt32 len);
	XnUInt8 ProcGetVersionReq(const XnUInt8 *data, XnUInt32 len);
	XnUInt16 Sum(char*data);
	XnBool SumCheck(char*data, XnUInt16 iCheck);
    XnUInt16 CRC16(const XnUInt8 *data, XnUInt32 len);
private:
    XnUInt32 m_inCount;
    XnUInt32 m_outCount;
    XnInt32 m_devFd;
    XnBool m_485WriteState; //true 485处于写状态
    XnBool m_doorOpened;
    XnUInt8 m_buf[256];
    XnUInt8 m_len;
    XnUInt32 m_timeStamp;
    XnUInt8 m_devAddr;  //本机设备地址，标记前门或者后门，从配置文件读取
    //NetProtocol *m_DealNetObj;
	//param_cfg m_cfg;
};
#endif
