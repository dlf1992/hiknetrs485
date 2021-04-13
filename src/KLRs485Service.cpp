#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <algorithm>
#include "KLRs485Service.h"
//#include "Algorithm.h"
//#include "tm_syslog.h"


unsigned int CRC16table[256] =
{
  0x0000,0xC0C1,0xC181,0x0140,0xC301,0x03C0,0x0280,0xC241,
  0xC601,0x06C0,0x0780,0xC741,0x0500,0xC5C1,0xC481,0x0440,
  0xCC01,0x0CC0,0x0D80,0xCD41,0x0F00,0xCFC1,0xCE81,0x0E40,
  0x0A00,0xCAC1,0xCB81,0x0B40,0xC901,0x09C0,0x0880,0xC841,
  0xD801,0x18C0,0x1980,0xD941,0x1B00,0xDBC1,0xDA81,0x1A40,
  0x1E00,0xDEC1,0xDF81,0x1F40,0xDD01,0x1DC0,0x1C80,0xDC41,
  0x1400,0xD4C1,0xD581,0x1540,0xD701,0x17C0,0x1680,0xD641,
  0xD201,0x12C0,0x1380,0xD341,0x1100,0xD1C1,0xD081,0x1040,
  0xF001,0x30C0,0x3180,0xF141,0x3300,0xF3C1,0xF281,0x3240,
  0x3600,0xF6C1,0xF781,0x3740,0xF501,0x35C0,0x3480,0xF441,
  0x3C00,0xFCC1,0xFD81,0x3D40,0xFF01,0x3FC0,0x3E80,0xFE41,
  0xFA01,0x3AC0,0x3B80,0xFB41,0x3900,0xF9C1,0xF881,0x3840,
  0x2800,0xE8C1,0xE981,0x2940,0xEB01,0x2BC0,0x2A80,0xEA41,
  0xEE01,0x2EC0,0x2F80,0xEF41,0x2D00,0xEDC1,0xEC81,0x2C40,
  0xE401,0x24C0,0x2580,0xE541,0x2700,0xE7C1,0xE681,0x2640,
  0x2200,0xE2C1,0xE381,0x2340,0xE101,0x21C0,0x2080,0xE041,
  0xA001,0x60C0,0x6180,0xA141,0x6300,0xA3C1,0xA281,0x6240,
  0x6600,0xA6C1,0xA781,0x6740,0xA501,0x65C0,0x6480,0xA441,
  0x6C00,0xACC1,0xAD81,0x6D40,0xAF01,0x6FC0,0x6E80,0xAE41,
  0xAA01,0x6AC0,0x6B80,0xAB41,0x6900,0xA9C1,0xA881,0x6840,
  0x7800,0xB8C1,0xB981,0x7940,0xBB01,0x7BC0,0x7A80,0xBA41,
  0xBE01,0x7EC0,0x7F80,0xBF41,0x7D00,0xBDC1,0xBC81,0x7C40,
  0xB401,0x74C0,0x7580,0xB541,0x7700,0xB7C1,0xB681,0x7640,
  0x7200,0xB2C1,0xB381,0x7340,0xB101,0x71C0,0x7080,0xB041,
  0x5000,0x90C1,0x9181,0x5140,0x9301,0x53C0,0x5280,0x9241,
  0x9601,0x56C0,0x5780,0x9741,0x5500,0x95C1,0x9481,0x5440,
  0x9C01,0x5CC0,0x5D80,0x9D41,0x5F00,0x9FC1,0x9E81,0x5E40,
  0x5A00,0x9AC1,0x9B81,0x5B40,0x9901,0x59C0,0x5880,0x9841,
  0x8801,0x48C0,0x4980,0x8941,0x4B00,0x8BC1,0x8A81,0x4A40,
  0x4E00,0x8EC1,0x8F81,0x4F40,0x8D01,0x4DC0,0x4C80,0x8C41,
  0x4400,0x84C1,0x8581,0x4540,0x8701,0x47C0,0x4680,0x8641,
  0x8201,0x42C0,0x4380,0x8341,0x4100,0x81C1,0x8081,0x4040
};
unsigned int CRC16_isr(unsigned char *Dat, unsigned int len)
{
	unsigned int i,TxCRC16;
    TxCRC16	=	0;
    for (i=0; i<len; i++)
    {
        TxCRC16=(CRC16table[Dat[i]^(TxCRC16&0xFF)]^(TxCRC16/0x100));
    }
	if(TxCRC16==0x0000)	TxCRC16	= 0xFFFF;
	return TxCRC16;
}

std::string charToStr(char * contentChar)
{
	std::string tempStr;
	for (int i=0;contentChar[i]!='\0';i++)
	{
		tempStr+=contentChar[i];
	}
	return tempStr;
}

void modifyContentInFile(std::string fileName,int lineNum,char *content)
{
	std::ifstream in;
	char line[1024]={'\0'};
	in.open(fileName.c_str());
	int i=0;
	std::string tempStr;
	while(in.getline(line,sizeof(line)))
	{
		i++;
		if(lineNum!=i)
		{
			tempStr+=charToStr(line);
		}
		else
		{
	       tempStr+=charToStr(content);
		}
		tempStr+='\n';
	}
	//
	tempStr = tempStr.substr(0, tempStr.length()-1);
	in.close();
	std::ofstream out;
	out.open(fileName.c_str());
	out.flush();
	out<<tempStr;
	out.close();
}

KLRs485Service::KLRs485Service()
{
    m_inCount = 0;
    m_outCount = 0;
    m_devFd = -1;
    m_485WriteState = false;
    m_doorOpened = false;
    m_len = 0;
    m_timeStamp = 0;
}

KLRs485Service::~KLRs485Service()
{
    UninitRs485Dev();
}

void KLRs485Service::Start()
{
    //m_devAddr = cfg.dev_addr;
    m_timeStamp = GetTimeMs();
	//m_cfg = cfg;
    InitRs485Dev();
}

void KLRs485Service::Stop()
{
    m_devAddr = 0;
    UninitRs485Dev();
}

XnBool KLRs485Service::DoorIsOpened()
{
    return m_doorOpened;
}

void KLRs485Service::Process()
{
    XnUInt8 buf[32];
    XnInt32 rlen = Rs485Read(buf, sizeof(buf));
    
    if (rlen > 0)
    {
		XnUInt32 timeStamp = GetTimeMs();
		#if 0
		printf("\nread 485 data:len=%d\n", rlen);
		for(int i = 0; i < rlen; i++)
		{
		    printf("%02x ", buf[i]);
		}
		printf("\nread 485 end!\n");
	    #endif
        Parse(buf, rlen, timeStamp);
    }
}

void KLRs485Service::InitRs485Dev()
{  
    m_devFd = open(RS485COM, O_RDWR | O_NOCTTY | O_NDELAY);
    if (m_devFd != -1)
    {
     #if 1
        struct termios cfg;
        memset(&cfg, 0, sizeof(cfg));
        //tcgetattr(0, &cfg);
        cfsetispeed(&cfg, /*B115200*/B9600);
        
        
        cfg.c_cflag |= CLOCAL | CREAD; //使能串口输入
        //cfg.c_lflag |= ICANON; //标准模式
        cfg.c_lflag &= ~ICANON;//原始模式
        cfg.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

		cfg.c_oflag &= ~(ONLCR); //add by wy
		cfg.c_oflag &= ~(TABDLY); 
		cfg.c_oflag &= ~(OPOST);
    
        //8bit数据
        cfg.c_cflag &= ~CSIZE;
        cfg.c_cflag |= CS8;
        //1bit停止�?        cfg.c_cflag &= ~CSTOPB;
        //无校�?        cfg.c_cflag &= ~PARENB;
        //禁用硬件流控制：
        cfg.c_cflag &= ~CRTSCTS;
        
        cfg.c_cc[VTIME] = 1;
        cfg.c_cc[VMIN] = 0;

		tcflush(m_devFd, TCIFLUSH);
        tcsetattr(m_devFd, TCSANOW, &cfg);
	 #endif
    }
    else
    {
        printf("open dev err.\n");
    }
}

void KLRs485Service::UninitRs485Dev()
{
    if (m_devFd != -1)
    {
        close(m_devFd);
    }
}

XnInt32 KLRs485Service::Rs485Read(XnUInt8* buf, XnUInt32 size)
{
    XnInt32 rlen = -1;
    if (m_485WriteState)
    {
        m_485WriteState = false;
        //system("echo 0 > /sys/class/gpio/gpio42/value");
    }
    if (m_devFd != -1)
    {
        rlen = read(m_devFd, buf, size);
    }
    return rlen;
}
#if 0
XnInt32 KLRs485Service::Rs485Write_ex(const XnUInt8* data, XnUInt32 len)
{
    XnInt32 wlen = -1;
    if (!m_485WriteState)
    {
        m_485WriteState = true;
        system("echo 1 > /sys/class/gpio/gpio42/value");
    }
    
    if (m_devFd != -1)
    {
        wlen = write(m_devFd, data, len);
        tcflush(m_devFd, TCOFLUSH);
    }
    
    if (m_485WriteState)
    {
        m_485WriteState = false;
        system("echo 0 > /sys/class/gpio/gpio42/value");
    }
    return wlen;
}
#endif
int KLRs485Service::Rs485Write(XnUInt8 *data, unsigned int len)
{
    int wlen = -1;
    if (!m_485WriteState)
    {
        m_485WriteState = true;
        //system("echo 1 > /sys/class/gpio/gpio42/value");
		usleep(len * 8 * 1000000 / 9600);
    }
    
    if (m_devFd != -1)
    {
        #if 0
			printf("\n*****write 485 data:len=%d, df_state=%d*****\n", len, m_485WriteState);
			for(int i = 0; i < len; i++)
			{
			    printf("%x ", data[i]);
			}
			printf("\n***************write 485 end*********************\n");
		#endif
			
        wlen = write(m_devFd, data, len);
        tcflush(m_devFd, TCOFLUSH);
		usleep(len * 8 * 1000000 / 9600);
    }
    
    if (m_485WriteState)
    {
        m_485WriteState = false;
        usleep(len * 8 * 1000000 / 9600);
        //system("echo 0 > /sys/class/gpio/gpio42/value");
    }
    return wlen;
}


void KLRs485Service::Parse(const XnUInt8 *data, XnUInt32 len, XnUInt32 timeStamp)
{
    XnUInt8 frameLen = 0;

    if (timeStamp - m_timeStamp > 20*1000)
    {
        memcpy(m_buf, data, MIN(sizeof(m_buf), len));
        m_len = MIN(sizeof(m_buf), len);
    }
    else
    {
        memcpy(m_buf + m_len, data, MIN(sizeof(m_buf) - m_len, len));
        m_len += MIN(sizeof(m_buf) - m_len, len);
    }
    m_timeStamp = timeStamp;
	char tmp[128];
    while (m_len >= DATA_LEN_MAX)
    {
        packagehead_st *phead = (packagehead_st*)m_buf;

		if(phead->head == SCREEN_HEAD_FLAG && phead->dev_id == KL_DEV_ADDR) //是该设备命令同时是完整包
		{
		    #if 0
			printf("\n#####read 485 data:len=%d #####\n", m_len);
			for(int i = 0; i < len; i++)
			{
			    printf("%x ", m_buf[i]);
			}
			printf("\n########read 485 end########\n");
		    #endif

			memset(tmp,0,sizeof(tmp));
			snprintf(tmp,sizeof(tmp),"Parse success,cmd=%x,door=%d", phead->cmd, m_buf[9]);
			LOGRECORD(tmp);
		    if(phead->cmd == CMD_DEV_HAND)
		    {
		        ProcShakeHandsReq(m_buf, m_len);
		    }
			else if(phead->cmd == CMD_SET_DEV_ADDR)
			{
			    ProcSetDevAddrReq(m_buf, m_len);
			}
		    else if (phead->cmd == CMD_SET_DOOR_STATE)
	        {
	            ProcSetDoorStateReq(m_buf, m_len);
	        }
	        else if (phead->cmd == CMD_GET_COUNT)
	        {
	            ProcGetCountReq(m_buf, m_len);
	        }
	        else if (phead->cmd == CMD_CLIEAR_COUNT)
	        {
	            ProcClearCountReq(m_buf, m_len);
	        }
			else if(phead->cmd == CMD_GET_VERSION)
			{
			    ProcGetVersionReq(m_buf, m_len);
			}
			else
			{
			    printf("\nwrong cmd=%d\n", phead->cmd);
			}
	        frameLen = DATA_LEN_MAX;
		}
		else
		{
		    frameLen = 1;
		}
   
        if (frameLen > 0)
        {
            memmove(m_buf, m_buf + frameLen, m_len - frameLen);
            m_len -= frameLen;
        }
        else
        {
            break;
        }
    }
}

XnUInt8 KLRs485Service::ProcShakeHandsReq(const XnUInt8 *data, XnUInt32 len)
{
	handshake_pnumclean_st * pShakeHandsReq = (handshake_pnumclean_st*)data;
	if(CRC16_isr((XnUInt8 *)data, len-2) == pShakeHandsReq->body.crc16 && pShakeHandsReq->head.cmd == CMD_DEV_HAND)
	{
        handshake_pnumclean_st ShakeHandsReq;
		memset((char*)&ShakeHandsReq, 0 , sizeof(ShakeHandsReq));
		ShakeHandsReq.head.head = SCREEN_HEAD_FLAG;
		ShakeHandsReq.head.pkg_id = pShakeHandsReq->head.pkg_id;
		ShakeHandsReq.head.dev_id = CZJ_ADDR;
		ShakeHandsReq.head.door_id = /*m_devAddr*/pShakeHandsReq->head.door_id;
		ShakeHandsReq.head.cmd = CMD_DEV_HAND;
		ShakeHandsReq.head.len = DATA_LEN; //协议定长
		ShakeHandsReq.body.crc16 = CRC16_isr((unsigned char*)&ShakeHandsReq, sizeof(ShakeHandsReq)-2);

	    Rs485Write((XnUInt8*)&ShakeHandsReq, DATA_LEN_MAX);
	}
    return 0;
}

void KLRs485Service::SetCfgParaDoor(XnUInt8 DoorIndex) 
{ 
    //char DevAddr[16] = {0};
    //sprintf(DevAddr, "dev_addr=%d", DoorIndex);
    //modifyContentInFile(PARAM_CFG_FILE, 2, DevAddr);
}

XnUInt8 KLRs485Service::ProcSetDevAddrReq(const XnUInt8 *data, XnUInt32 len)
{
	setaddress_doorstatus_st * pSetDevAddrReq = (setaddress_doorstatus_st*)data;
    
	if(CRC16_isr((XnUInt8*)data, len-2) == pSetDevAddrReq->body.crc16 && pSetDevAddrReq->head.cmd == CMD_SET_DEV_ADDR)
	{
        setaddress_doorstatus_st_ack setaddressAck;

		setaddressAck.head.head = SCREEN_HEAD_FLAG;
		setaddressAck.head.dev_id = CZJ_ADDR;
		setaddressAck.head.pkg_id = pSetDevAddrReq->head.pkg_id;
		setaddressAck.head.door_id = /*m_devAddr*/pSetDevAddrReq->head.door_id;
		setaddressAck.head.cmd = CMD_SET_DEV_ADDR;
		setaddressAck.head.len = DATA_LEN; //协议定长
		//add by wy 2019/8/30将设置结果写文件
		//SetCfgParaDoor(pSetDevAddrReq->body.door_id);
		//param_cfg cfg;
	    //if(!get_cfg_para(cfg))//从文件中读取对应的设置看是否成功
	    //{
	    //    printf("\nline=%d, func=%s 207_ip.txt failed\n", __LINE__, __func__);
	    //}
		//根据是否按照要求写入文件，返回对应的成功或者失�?		setaddressAck.body.status = cfg.dev_addr == pSetDevAddrReq->body.door_id ? TRUE : FALSE;
		setaddressAck.body.status = TRUE;
		setaddressAck.body.crc16 = CRC16_isr((unsigned char*)&setaddressAck, sizeof(setaddressAck)-2);

		Rs485Write((XnUInt8*)&setaddressAck, DATA_LEN_MAX);
	}
    return 0;
}

XnUInt8 KLRs485Service::ProcSetDoorStateReq(const XnUInt8 *data, XnUInt32 len)
{
	setaddress_doorstatus_st * pSetDoorStateReq = (setaddress_doorstatus_st*)data;
	if(CRC16_isr((XnUInt8*)data, len-2) == pSetDoorStateReq->body.crc16 && pSetDoorStateReq->head.cmd == CMD_SET_DOOR_STATE)
	{

        setaddress_doorstatus_st_ack SetDoorStateAck;
		memset((char*)&SetDoorStateAck, 0 , sizeof(SetDoorStateAck));

		SetDoorStateAck.head.head = SCREEN_HEAD_FLAG;
		SetDoorStateAck.head.dev_id = CZJ_ADDR;
		SetDoorStateAck.head.pkg_id = pSetDoorStateReq->head.pkg_id;
		SetDoorStateAck.head.door_id = /*m_devAddr*/pSetDoorStateReq->head.door_id;
		SetDoorStateAck.head.cmd = CMD_SET_DOOR_STATE;
		SetDoorStateAck.head.len = DATA_LEN; //协议定长
		SetDoorStateAck.body.status = TRUE;
		SetDoorStateAck.body.crc16 = CRC16_isr((unsigned char *)&SetDoorStateAck, sizeof(SetDoorStateAck)-2);

	    Rs485Write((XnUInt8*)&SetDoorStateAck, DATA_LEN_MAX);
	}
	return 0;
}

XnUInt8 KLRs485Service::ProcGetCountReq(const XnUInt8 *data, XnUInt32 len)
{
	handshake_pnumclean_st * pGetCountReq = (handshake_pnumclean_st*)data;
	char tmp[128];
	if(CRC16_isr((XnUInt8*)data, len-2) == pGetCountReq->body.crc16 && pGetCountReq->head.cmd == CMD_GET_COUNT)
	{
        passengerflow_st GetCountAck;
		memset((char*)&GetCountAck, 0 , sizeof(GetCountAck));
		GetCountAck.head.head = SCREEN_HEAD_FLAG;
		GetCountAck.head.dev_id = CZJ_ADDR;
		GetCountAck.head.pkg_id = pGetCountReq->head.pkg_id;
		GetCountAck.head.door_id = /*m_devAddr*/pGetCountReq->head.door_id;
		GetCountAck.head.cmd = CMD_GET_COUNT;
		GetCountAck.head.len = DATA_LEN; //协议定长
		if(pGetCountReq->head.door_id == 0)
		{
			printf("front door.\n");
			GetCountAck.body.PUP = front_enternum;
			GetCountAck.body.PDOWN = front_exitnum;
		}
		else
		{
			printf("back door.\n");
			GetCountAck.body.PUP = back_enternum;
			GetCountAck.body.PDOWN = back_exitnum;
		}	
		GetCountAck.body.crc16 = CRC16_isr((unsigned char*)&GetCountAck, sizeof(GetCountAck)-2);

		printf("get count up=%d, down=%d\n", GetCountAck.body.PUP,GetCountAck.body.PDOWN);
		memset(tmp,0,sizeof(tmp));
		snprintf(tmp,sizeof(tmp),"get count up=%d, down=%d", GetCountAck.body.PUP,GetCountAck.body.PDOWN);
		LOGRECORD(tmp);

	    Rs485Write((XnUInt8*)&GetCountAck, DATA_LEN_MAX);
	}
	return 0;
}

XnUInt8 KLRs485Service::ProcClearCountReq(const XnUInt8 *data, XnUInt32 len)
{
	handshake_pnumclean_st * pClearCountReq = (handshake_pnumclean_st*)data;

	if(CRC16_isr((XnUInt8*)data, len-2) == pClearCountReq->body.crc16 && pClearCountReq->head.cmd == CMD_CLIEAR_COUNT)
	{		
        if(pClearCountReq->head.door_id == 0)
		{
			printf("Clear Count front door.\n");
			LOGRECORD("Clear Count front door");
			char m_ip[16] = {0};
			char m_usrname[32] = {0};
			char m_passwd[32] = {0};
			memcpy(&m_ip,&(front_camerainfo.cameraip),strlen(front_camerainfo.cameraip));	
			memcpy(&m_usrname,&(front_camerainfo.usrname),strlen(front_camerainfo.usrname));
			memcpy(&m_passwd,&(front_camerainfo.passwd),strlen(front_camerainfo.passwd));	
			clearpdcnum(m_ip,m_usrname,m_passwd);			
		}
		else
		{
			printf("Clear Count back door.\n");
			LOGRECORD("Clear Count back door");
			char m_ip[16] = {0};
			char m_usrname[32] = {0};
			char m_passwd[32] = {0};
			memcpy(&m_ip,&(front_camerainfo.cameraip),strlen(front_camerainfo.cameraip));	
			memcpy(&m_usrname,&(front_camerainfo.usrname),strlen(front_camerainfo.usrname));
			memcpy(&m_passwd,&(front_camerainfo.passwd),strlen(front_camerainfo.passwd));
			clearpdcnum(m_ip,m_usrname,m_passwd);
		}	
	
        handshake_pnumclean_st ClearCountAck;
		memset((char*)&ClearCountAck, 0 , sizeof(ClearCountAck));
		ClearCountAck.head.head = SCREEN_HEAD_FLAG;
		ClearCountAck.head.pkg_id = pClearCountReq->head.pkg_id;
		ClearCountAck.head.dev_id = CZJ_ADDR;
		ClearCountAck.head.door_id = /*m_devAddr*/pClearCountReq->head.door_id;
		ClearCountAck.head.cmd = CMD_CLIEAR_COUNT;
		ClearCountAck.head.len = DATA_LEN; //协议定长
		ClearCountAck.body.crc16 = CRC16_isr((unsigned char*)&ClearCountAck, sizeof(ClearCountAck)-2);
	    Rs485Write((XnUInt8*)&ClearCountAck, DATA_LEN_MAX);
	}
	return 0;
}

XnUInt8 KLRs485Service::ProcGetVersionReq(const XnUInt8 *data, XnUInt32 len)
{
	get_version_st * pGetVersionReq = (get_version_st*)data;

	if(CRC16_isr((XnUInt8*)data, len-2) == pGetVersionReq->body.crc16 && pGetVersionReq->head.cmd == CMD_GET_VERSION)
	{
	    printf("get version!\n");

        get_version_st GetVersionAck;
		memset((char*)&GetVersionAck, 0 , sizeof(GetVersionAck));
		GetVersionAck.head.head = SCREEN_HEAD_FLAG;
		GetVersionAck.head.dev_id = CZJ_ADDR;
		GetVersionAck.head.pkg_id = pGetVersionReq->head.pkg_id;
		GetVersionAck.head.door_id = /*m_devAddr*/pGetVersionReq->head.door_id;
		GetVersionAck.head.cmd = CMD_GET_VERSION;
		GetVersionAck.head.len = DATA_LEN; //协议定长
		std::string version = "V1.01";
		int len = version.length();
		len = len > VERSION_DATA_LEN ? VERSION_DATA_LEN : len;
		memcpy(GetVersionAck.body.NU,  version.c_str(), len);
		GetVersionAck.body.crc16 = CRC16_isr((unsigned char*)&GetVersionAck, sizeof(GetVersionAck)-2);
	    Rs485Write((XnUInt8*)&GetVersionAck, DATA_LEN_MAX);
	}
	return 0;
}


XnUInt32 KLRs485Service::GetTimeMs()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

XnBool KLRs485Service::SumCheck(char*data, XnUInt16 iCheck)
{
    int sum = 0;
    for(int i = 0; i < 12; i++)
    {
        sum += data[i];
    }
	XnBool bRet = sum == iCheck ? true : false;
	return bRet;
}

XnUInt16 KLRs485Service::Sum(char*data)
{
    XnUInt16 sum = 0;
    for(int i = 0; i < 12; i++)
    {
        sum += data[i];
    }
	return sum;
}

XnUInt16 KLRs485Service::CRC16(const XnUInt8 *data, XnUInt32 len)
{
    unsigned char i;
    unsigned short crc = 0xFFFF;
    while (len)
    {
        crc ^= *data;
        for (i = 0; i < 8; i++)
        {
            if (crc & 0x1)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
        ++data;
        --len;
    }
    return crc;
}
