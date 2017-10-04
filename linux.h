/*
 * linux.h
 *
 *  Created on: Jun 21, 2017
 *      Author: xieliming
 */
#ifndef LINUX_H_
#define LINUX_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <termios.h>
#include <sys/io.h>
#include <sys/ioctl.h>
/*
 *
 */
#define PACKED __attribute__( ( packed, aligned(1) ) )                                      //单字节对齐
#pragma pack(1)

typedef unsigned int   DWORD;
typedef unsigned short WORD;

//一般性函数接口
void setMousePos(unsigned int y,unsigned int x);                                            //设置游标位置
int  GetKeyWord();                                                                          //不回显检测键盘按下
int  Rand_value();                                                                          //产生随机数随机度较高
void clearScreen();                                                                         //清理屏幕
bool Sec_sleep(int s);                                                                      //秒级别误差在110us左右
bool Ms_sleep(int ms);                                                                      //毫秒级别误差在110us左右
bool Is_two_n(int num);                                                                     //3、判断一个数是不是2的n次方。
int  Bitap(const char *text, const char *find);                                             //Bitap算法主要利用位运算进行字符串的匹配
bool RegisteSignal(int sig,void (*handler)(int _sig));                                      //信号注册函数
int  CreateThread(void *(*Dealfunction)(void *),void * aParm,int priority);                 //需要sudo 运行程序
WORD CalCheckCRC(unsigned char* lpBuf,int iLen);                                            //CRC校验以WORD为单位
//配置文件接口
int  ReplaceStr(char * sSrc,const char * sMatchStr,const char * sReplaceStr);
int  GetPrivateProfileInt(const char *key,const char * cField,int cDefault,char *filename); //.ini 配置文件操作eg:"./vxcu.ini"
int  GetPrivateProfileString(const char* key,const char *cField,char * cDefault,char * cRet,int dRetLen,char * filename );
bool WritePrivateProfileInt(const char * key,const char *cField,int nValue,char * filename);
bool WritePrivateProfileString(const char * key,const char *cField,char *cValue,char * filename);
//网络相关接口
bool getLocalActiveNetworkInfo(char * res_name);                                            //获取本地当前工作的网卡信息
bool getLocalALLNetworkInfo(char * res_name);                                               //获取本地所有网卡信息
int  creatbindTcpsock(char * bindip,int bindport);
int  creatbindUdpsock(char * bindip,int bindport);
bool setaddrReuse(int sock);                                                                //设置地址复用
bool setsockNonblock(int sock);                                                             //设置非阻塞模式
bool setsockBroadcast(int sock);                                                            //设置广播模式
bool setsockRwbuf(int sock,int rwval);                                                      //设置收发缓存区大小
bool setsockTimeout(int sock,int tsval,int tusval);                                         //设置阻塞模式收发超时
//串口相关接口
bool InitSerialPort(int port,int baudrate,int blockflag,char checkflag,int databyte,int stopbyte);//需要sudo 运行程序
//位操作相关接口
int  calc_1_Num(int _n);                                                                    //计算二进制数中1的个数
void showByte8(unsigned char ch);
void clrAllbyte8(unsigned char * ch);
void setByte8(unsigned char * ch,unsigned int index);
void clrByte8(unsigned char * ch,unsigned int index);
//端口相关的接口 inb outb
bool getAllPermission();                                                                   //需要sudo 运行程序

#endif    /*LINUX_H_*/
