#ifndef __RSPROTOCOL_H__
#define __RSPROTOCOL_H__

#define SCREEN_HEAD_FLAG        0X24242424 //$$$$
#define KL_DEV_ADDR             0XB6        //客流摄像机设备地址
#define CZJ_ADDR                0X01        //车载机设备地址
#define DATA_LEN_MAX 16
#define DATA_LEN     16
#define VERSION_DATA_LEN 5

typedef struct{
	unsigned int  head; //"$$$$"
	unsigned char dev_id;  //0xB6客流设备
	unsigned char cmd;
	unsigned char len;     //数据包总长度head<->crc校验
	unsigned char pkg_id;
	unsigned char door_id;
}__attribute__ ((__packed__)) packagehead_st;	//包头

typedef struct{
	unsigned char NU[5];
	unsigned short crc16;
}__attribute__ ((__packed__)) packagehandshake_st;

typedef struct{
	packagehead_st head;
	packagehandshake_st body;
}__attribute__ ((__packed__)) handshake_pnumclean_st;	//握手C5 清零C8 ><

typedef struct{
	unsigned char door_id;
	unsigned char NU[4];
	unsigned short crc16;
}__attribute__ ((__packed__)) packagesetaddress_st;

typedef struct{
	unsigned char status;
	unsigned char NU[4];
	unsigned short crc16;
}__attribute__ ((__packed__)) packagesetaddress_st_ack;

typedef struct{
	packagehead_st head;
	packagesetaddress_st body;
}__attribute__ ((__packed__)) setaddress_doorstatus_st;	//设置地址C6 门信号CA ><

typedef struct{
	packagehead_st head;
	packagesetaddress_st_ack body;
}__attribute__ ((__packed__)) setaddress_doorstatus_st_ack;	//设置地址C6 门信号CA ><


typedef struct{
	unsigned char PUP;
	unsigned char PDOWN;
	unsigned char NU[3];
	unsigned short crc16;
}__attribute__ ((__packed__)) packagepassengerflow_st;

typedef struct{
	packagehead_st head;
	packagepassengerflow_st body;
}__attribute__ ((__packed__)) passengerflow_st;	//获取人数C7 ><

typedef struct{
	unsigned char NU[5];
	unsigned short crc16;
}__attribute__ ((__packed__)) package_get_version_st;

typedef struct{
	packagehead_st head;
	package_get_version_st body;
}__attribute__ ((__packed__)) get_version_st;	//获取版本信息C9 ><


#endif

