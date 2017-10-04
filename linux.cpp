/*
 * linux.cpp
 *
 *  Created on: Jun 21, 2017
 *      Author: xieliming
 */
#include "linux.h"
void setMousePos(unsigned int y,unsigned int x)
{
	printf("\033[%d;%dH\033[0m",y,x);
}
void clearScreen()
{
	printf("\033[2J");
}
int GetKeyWord()
{
	int in;                                    //通过tcsetattr函数设置terminal的属性来控制需不需要回车来结束输入
	struct termios new_settings;
	struct termios stored_settings;
	tcgetattr(0,&stored_settings);
	new_settings = stored_settings;
	new_settings.c_lflag &= (~ICANON);         //屏蔽整行缓存。那就只能单个了
	new_settings.c_lflag &= ~ECHO;
	new_settings.c_cc[VTIME] = 0;
	new_settings.c_cc[VMIN] = 1;
	tcsetattr(0,TCSANOW,&new_settings);
	in = getchar();
	tcsetattr(0,TCSANOW,&stored_settings);
	return in;
}
bool Sec_sleep(int s)
{
	struct timespec request,remain;
	memset(&request,0,sizeof(request));
	memset(&remain,0,sizeof(remain));
	//Retrieve current value of CLOCK_REALTIME clock
	if(clock_gettime(CLOCK_REALTIME,&request)==-1)
	{
		fprintf(stderr,"clock_gettime error\n");
		return false;
	}
	request.tv_sec+=s;
	int res=clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME,&request,&remain);
	if(res!=0)
	{
		fprintf(stderr,"clock_nanosleep error\n");
		return false;
	}
	return true;
}
bool Ms_sleep(int ms)
{
	if(ms>999)
	{
		printf("the argument is overflow,please use Sec_sleep\n");
		return false;
	}
	struct timespec _t;
	memset(&_t,0,sizeof(_t));
	_t.tv_sec=0;
	_t.tv_nsec=ms*1000000;
	nanosleep(&_t,0);
	return false;
}
int Rand_value()
{
	struct timeval tv;
	memset(&tv,0,sizeof(tv));
	gettimeofday(&tv,NULL);
	srand(tv.tv_sec*1000000+tv.tv_usec);
	return rand();
}
bool RegisteSignal(int sig,void (*handler)(int _sig))
{
	struct sigaction sa;
	memset(&sa,0,sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_flags=0;
	sa.sa_handler=handler;
	if(sigaction(sig,&sa,NULL)==-1)
		return false;
	return true;
}
int Bitap(const char *text, const char *find)  //0~
{
    int text_len = strlen(text);
    int find_len = strlen(find);
    if (text_len==0||find_len==0||(text_len < find_len))    return -1;
    int i = 0 ,j=find_len - 1;
    char *map=(char*)malloc(find_len + 1);
    *(map+0) = 1;
    for (i=1; i<=find_len; ++i)
    {
        *(map+i) = 0;
    }
    for (i=0; i< text_len; ++i)
    {
        for (j=find_len-1; j>=0; --j)
        {
            *(map+j+1) = *(map+j) & (*(text+i) == *(find+j));
        }
        if (*(map+find_len) == 1)
            return i - find_len + 1;
    }
    free(map);
    return -1;
}
bool Is_two_n(int num)
{
	if (!(num&(num-1)))
		return true;
	else
		return false;
}
int CreateThread(void *(*Dealfunction) (void *),void * aParm,int priority)
{
	pthread_t t;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	struct sched_param param;
	param.__sched_priority=priority;                                       //set thread priority
	pthread_attr_setschedpolicy(&attr,SCHED_RR);                           //set thread scheduler
	pthread_attr_setschedparam(&attr,&param);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);            //set pthread detach state
	pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);            //the step is very important
	pthread_create(&t,&attr,Dealfunction,aParm);
	pthread_attr_destroy(&attr);
	return t;
}

int ReplaceStr(char * sSrc,const char * sMatchStr,const char * sReplaceStr)
{
	int StringLen;
	char caNewString[256];
	char *FindPos=strstr(sSrc,sMatchStr);
	if((!FindPos)||(!sMatchStr)) return -1;

	while(FindPos)
	{
		memset(caNewString,0,sizeof(caNewString));
		StringLen=FindPos-sSrc;
		strncpy(caNewString,sSrc,StringLen);
		strcat(caNewString,sReplaceStr);
		strcat(caNewString,FindPos+strlen(sMatchStr));
		strcpy(sSrc,caNewString);
		FindPos=strstr(sSrc,sMatchStr);
	}
	return 0;
}
int GetPrivateProfileInt(const char *key,const char * cField,int nDefault,char *filename)
{
	int nRet=nDefault;
	char str[1024],cTmp[200],*pc=NULL;
	FILE*fp=NULL;
	int nLen,len=0;
	if(!key||!cField)
	{
		return nRet;
	}
	if((fp=fopen(filename,"r"))==NULL)
	{
		return nRet;
	}
	str[0]='\0';
	sprintf(cTmp,"[%s]",key);
	while(fgets(str,1024,fp))
	{
		ReplaceStr(str," ","");
		ReplaceStr(str,"\r","");
		ReplaceStr(str,"\n","");
		len=strlen(str);
		if(len<=1)continue;
		if(strcmp(str,cTmp)!=0)
		{
			continue;
		}
		while(fgets(str,1024,fp))
		{
			ReplaceStr(str," ","");
			ReplaceStr(str,"\r","");
			ReplaceStr(str,"\n","");
			if(len<=1)
			{
				continue;
			}
			if(str[0]==';'||str[0]=='\0')
			{
				continue;
			}
			if(str[0]=='[')
			{
				break;
			}
			pc=strstr(str,"=");
			if(pc==NULL)
			{
				continue;
			}
			if((pc-str)<1)
			{
				continue;
			}
			if(strncmp(cField,str,pc-str)==0)
			{
				len=strlen(str)-1;
				if((pc-str)<len)
				{
					if(pc[2]=='x')
					{
						nLen=sscanf(pc+1,"0x%x",&nRet);
					}
					else if(pc[2]=='X')
					{
						nLen=sscanf(pc+1,"0X%x",&nRet);
					}
					else
					{
						nLen=sscanf(pc+1,"%d",&nRet);
					}
				}
				break;
			}
		}
		break;
	}
	fclose(fp);
	return nRet;
}
int GetPrivateProfileString(const char* key,const char *cField,char * cDefault,char * cRet,int dRetLen,char * filename )
{
	char str[1024],cTmp[200],*pc=NULL;
	FILE* fp=NULL;
	int len=0;
	cRet[0]=0;
	if(cDefault)
	{
		strcpy(cRet,cDefault);
	}
	if(!key||!cField)
	{
		return 0;
	}
	if((fp=fopen(filename,"r"))==NULL)
	{
		return 0;
	}
	str[0]='\0';
	sprintf(cTmp,"[%s]",key);
	while(fgets(str,1024,fp))
	{
		ReplaceStr(str," ","");
		ReplaceStr(str,"\r","");
		ReplaceStr(str,"\n","");
		len=strlen(str);
		if(len<=1) continue;
		if(str[0]==';'||str[0]=='\0')
		{
			continue;
		}
		if(strcmp(str,cTmp)!=0)
		{
			continue;
		}
		while(fgets(str,1024,fp))
		{
			ReplaceStr(str," ","");
			ReplaceStr(str,"\r","");
			ReplaceStr(str,"\n","");
			len=strlen(str);
			if(len<=1) continue;
			if(str[0]==';'||str[0]=='\0')
			{
				continue;
			}
			if(str[0]=='[')
			{
				break;
			}
			pc=strstr(str,"=");
			if(pc==NULL)
			{
				continue;
			}
			if((pc-str)<1)
			{
				continue;
			}
			if(strncmp(cField,str,pc-str)==0)
			{
				len=strlen(pc+1);
				if(len<dRetLen)
				{
					strcpy(cRet,pc+1);
				}
				else
				{
					memcpy(cRet,pc+1,dRetLen-1);
				}
				break;
			}
		}
		break;
	}
	fclose(fp);
	return (strlen(cRet)-1);
}
///////////////////////////////////////////////////////////////////
int GetFileSize(char * path,DWORD *len)
{
	int filesize=-1;
	struct stat statbuff;
	if(stat(path,&statbuff)<0)
	{
		return filesize;
	}

	filesize=statbuff.st_size;
	return filesize;
}
int FindKey(const char *cKey,char * filename)
{
	int nFind=0;
	char str[1024],cTmp[100];
	FILE*fp=NULL;
	DWORD nLen;
	DWORD nhLen;
	int len=0;
	int lastpos=0;
	if(!cKey)
	{
		return nFind;
	}
	if((fp=fopen(filename,"r"))==NULL)
	{
		return nFind;
	}
	nLen=GetFileSize(filename,&nhLen);
	if(nLen<=0)
	{
		fclose(fp);
		return nFind;
	}
	str[0]='\0';
	sprintf(cTmp,"[%s]",cKey);
	while(fgets(str,1024,fp))
	{
		len=strlen(str);
		str[len-1]=0;
		if(str[0]!=';'&&str[0]!='\0')
		{
			if(nFind==0)
			{
				if(strcmp(str,cTmp)==0)
				{
					nFind=1;
				}
			}
			else
			{
				if(str[0]=='[')
				{
					break;
				}
			}
		}
		lastpos=ftell(fp);
	}
	fclose(fp);
	return nFind;

}
void *CBuf(unsigned int nSize)
{
	char *m_pAddr;
	int m_nlen=nSize;
	m_pAddr=NULL;
	if((m_nlen)>0)
	{
		m_pAddr=(char *)malloc(nSize);
		if(!m_pAddr)
		{
			m_nlen=0;
			return NULL;
		}
		memset(m_pAddr,0,sizeof(nSize));
	}
	return (m_pAddr);
}
///////////////////////////////////////////////////////////////////
bool WritePrivateProfileInt(const char * key,const char *cField,int nValue,char * filename)
{
	FILE* fp=NULL;
	int len,newlen,hinewlen;
	char str[1024],cNewStr[200],cTmp[100],*pc=NULL;
	char *m_pAddr;
	int nFind=0;
	int nFindNextKey=0;
	char tmpstr[100];
	if(!key||!cField)
	{
		return false;
	}
	sprintf(cNewStr,"%s=%d\n",cField,nValue);
	sprintf(cTmp,"[%s]",key);
	//new
	if(!FindKey(key,filename))
	{
		if((fp=fopen(filename,"a+"))==NULL)
		{
			return false;
		}
		fprintf(fp,"\n[%s]\n",key);
		fprintf(fp,"%s",cNewStr);
		fclose(fp);
		return true;
	}
	//exist
	if((fp=fopen(filename,"r+"))==NULL)
	{
		return false;
	}
	newlen=GetFileSize(filename,(DWORD*)&hinewlen)+strlen(cNewStr)+1000;
	m_pAddr=(char*)CBuf(newlen);
	if(m_pAddr==NULL)
	{
		return false;
	}
	//key past part
	while(fgets(str,1024,fp))
	{
		len=strlen(str);
		str[len-1]=0;
		strcat(m_pAddr,str);
		strcat(m_pAddr,"\n");
		if(strcmp(str,cTmp)==0)
		{
			break;
		}
	}

	//key part
	while(fgets(str,1024,fp))
	{
		len=strlen(str);
		str[len-1]=0;
		if(str[0]=='\0')
		{
			continue;
		}
		if(str[0]=='['&&strcmp(str,cTmp))
		{
			nFindNextKey=1;
			break;
		}
		pc=strstr(str,"=");
		if(pc&&(pc-str)>=1)
		{
			memset(tmpstr,0,100);
			strncpy(tmpstr,str,pc-str);
		}

		if(pc&&(pc-str)>=1&&strcmp(cField,tmpstr)==0)
		{
			nFind=1;
			strcat(m_pAddr,cNewStr);
		}
		else
		{
			strcat(m_pAddr,str);
			strcat(m_pAddr,"\n");
		}
	}
	if(nFind==0)
	{
		strcat(m_pAddr,cNewStr);
		strcat(m_pAddr,"\n");
	}
	//key after part
	if(nFindNextKey)
	{
		strcat(m_pAddr,"\n");
		strcat(m_pAddr,str);
		strcat(m_pAddr,"\n");
		while(fgets(str,1024,fp))
		{
			len=strlen(str);
			str[len-1]=0;
			strcat(m_pAddr,str);
			strcat(m_pAddr,"\n");
		}
	}
	fclose(fp);
	//create file again
	fp=fopen(filename,"w+");
	fwrite(m_pAddr,strlen((char*)m_pAddr),1,fp);
	fclose(fp);
	free(m_pAddr);
	return true;
}
bool WritePrivateProfileString(const char * key,const char *cField,char *cValue,char * filename)
{
	FILE* fp=NULL;
	int len,newlen,hinewlen;
	char str[1024],cNewStr[200],cTmp[100],*pc=NULL;
	char *m_pAddr;
	int nFind=0;
	int nFindNextKey=0;
	char tmpstr[100];
	if(!key||!cField)
	{
		return false;
	}
	sprintf(cNewStr,"%s=%s\n",cField,cValue);
	sprintf(cTmp,"[%s]",key);
	//new
	if(!FindKey(key,filename))
	{
		if((fp=fopen(filename,"a+"))==NULL)
		{
			return false;
		}
		fprintf(fp,"\n[%s]\n",key);
		fprintf(fp,"%s",cNewStr);
		fclose(fp);
		return true;
	}
	//exist
	if((fp=fopen(filename,"r+"))==NULL)
	{
		return false;
	}
	newlen=GetFileSize(filename,(DWORD*)&hinewlen)+strlen(cNewStr)+1000;
	m_pAddr=(char*)CBuf(newlen);
	if(m_pAddr==NULL)
	{
		return false;
	}
	//key past part
	while(fgets(str,1024,fp))
	{
		len=strlen(str);
		str[len-1]=0;
		strcat(m_pAddr,str);
		strcat(m_pAddr,"\n");
		if(strcmp(str,cTmp)==0)
		{
			break;
		}
	}

	//key part
	while(fgets(str,1024,fp))
	{
		len=strlen(str);
		str[len-1]=0;
		if(str[0]=='\0')
		{
			continue;
		}
		if(str[0]=='['&&strcmp(str,cTmp))
		{
			nFindNextKey=1;
			break;
		}
		pc=strstr(str,"=");
		if(pc&&(pc-str)>=1)
		{
			memset(tmpstr,0,100);
			strncpy(tmpstr,str,pc-str);
		}

		if(pc&&(pc-str)>=1&&strcmp(cField,tmpstr)==0)
		{
			nFind=1;
			strcat(m_pAddr,cNewStr);
		}
		else
		{
			strcat(m_pAddr,str);
			strcat(m_pAddr,"\n");
		}
	}
	if(nFind==0)
	{
		strcat(m_pAddr,cNewStr);
		strcat(m_pAddr,"\n");
	}
	//key after part
	if(nFindNextKey)
	{
		strcat(m_pAddr,"\n");
		strcat(m_pAddr,str);
		strcat(m_pAddr,"\n");
		while(fgets(str,1024,fp))
		{
			len=strlen(str);
			str[len-1]=0;
			strcat(m_pAddr,str);
			strcat(m_pAddr,"\n");
		}
	}
	fclose(fp);
	//create file again
	fp=fopen(filename,"w+");
	fwrite(m_pAddr,strlen((char*)m_pAddr),1,fp);
	fclose(fp);
	free(m_pAddr);
	return true;
}
bool getLocalActiveNetworkInfo(char * res_name)
{
	struct ifreq ifr;
	struct ifconf ifc;
	char buf[2048]={0};
	int sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_IP);
	if(sock==-1) return false;
	ifc.ifc_len=sizeof(buf);
	ifc.ifc_buf=buf;
	if(ioctl(sock,SIOCGIFCONF,&ifc)==-1) return false;
	struct ifreq* it=ifc.ifc_req;
	const struct ifreq* const end=it+(ifc.ifc_len/sizeof(struct ifreq));
	char szMac[64]={0};
	bool _flag=false;
	for(;it!=end;++it)
	{
		strcpy(ifr.ifr_name,it->ifr_name);
		if(ioctl(sock,SIOCGIFFLAGS,&ifr)==0)
		{
			if(!(ifr.ifr_flags&IFF_LOOPBACK))
			{

				unsigned char * ptr;
				ptr=(unsigned char *)&ifr.ifr_ifru.ifru_hwaddr.sa_data[0];
				snprintf(szMac,64,"%02x:%02x:%02x:%02x:%02x:%02x",*ptr,*(ptr+1),*(ptr+2),*(ptr+3),*(ptr+4),*(ptr+5));
				if(_flag)
				{
					sprintf(res_name,"%s#%s",res_name,ifr.ifr_name);
				}
				else
				{
					sprintf(res_name,"%s",ifr.ifr_name);
					_flag=true;
				}
			}
		}
		else
			return false;
	}
	return true;
}
bool getLocalALLNetworkInfo(char * ips)
{
	struct ifaddrs *ifAddrStruct;
	void * tmpAddrPtr=NULL;
	char ip[INET_ADDRSTRLEN]={0};
	int n=0;
	getifaddrs(&ifAddrStruct);
	while(ifAddrStruct!=NULL)
	{
		if(ifAddrStruct->ifa_addr->sa_family==AF_INET)
		{
			tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
			inet_ntop(AF_INET,tmpAddrPtr,ip,INET_ADDRSTRLEN);
			if(n<2)
			{
				if(strncmp(ifAddrStruct->ifa_name,"lo",strlen("lo"))!=0)
				{
					strncat(ips,ifAddrStruct->ifa_name,strlen(ifAddrStruct->ifa_name));
				}
			}
			else
			{
				if(strncmp(ifAddrStruct->ifa_name,"lo",strlen("lo"))!=0)
				{
					strncat(ips,"#",1);
					strncat(ips,ifAddrStruct->ifa_name,strlen(ifAddrStruct->ifa_name));
				}
			}
			++n;
		}
		ifAddrStruct=ifAddrStruct->ifa_next;
	}
	freeifaddrs(ifAddrStruct);
	return true;
}
int  creatbindTcpsock(char * bindip,int bindport)
{
	int fd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==fd)
	{
		fprintf(stderr,"socket() failed:%s\n",strerror(errno));
		return -1;
	}
	struct sockaddr_in bindaddr;
	memset(&bindaddr,0,sizeof(bindaddr));
	bindaddr.sin_family=AF_INET;
	bindaddr.sin_addr.s_addr=inet_addr(bindip);
	bindaddr.sin_port=htons(bindport);
	if(bind(fd,(struct sockaddr*)&bindaddr,sizeof(struct sockaddr))==-1)
	{
		close(fd);
		fd=-1;
		fprintf(stderr,"bind addr failed:%s\n",strerror(errno));
		return -1;
	}
	return fd;
}
int  creatbindUdpsock(char * bindip,int bindport)
{
	int fd=socket(AF_INET,SOCK_DGRAM,0);
	if(-1==fd)
	{
		fprintf(stderr,"socket() failed:%s\n",strerror(errno));
		return -1;
	}
	struct sockaddr_in bindaddr;
	memset(&bindaddr,0,sizeof(bindaddr));
	bindaddr.sin_family=AF_INET;
	bindaddr.sin_addr.s_addr=inet_addr(bindip);
	bindaddr.sin_port=htons(bindport);
	if(bind(fd,(struct sockaddr*)&bindaddr,sizeof(struct sockaddr))==-1)
	{
		close(fd);
		fd=-1;
		fprintf(stderr,"bind addr failed:%s\n",strerror(errno));
		return -1;
	}
	return fd;
}
bool setaddrReuse(int sock)
{
	const int opt=1;
	if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof(opt))==-1)
	{
		return false;
	}
	return true;
}
bool setsockNonblock(int sock)
{
	unsigned long opt=1;
	if(ioctl(sock,FIONBIO,&opt)==-1)
	{
		return false;
	}
	return true;
}
bool setsockBroadcast(int sock)
{
	const int opt=1;
	if(setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(char*)&opt,sizeof(opt))!=0)
	{
		return false;
	}
	return true;
}
bool setsockRwbuf(int sock,int rwval)
{
	if(setsockopt(sock,SOL_SOCKET,SO_RCVBUF,(char*)&rwval,sizeof(int))==0)
	{
		if(setsockopt(sock,SOL_SOCKET,SO_SNDBUF,(char*)&rwval,sizeof(int))==0)
		{
			return true;
		}
	}
	return false;
}
bool setsockTimeout(int sock,int tsval,int tusval)
{
	struct timeval tv;
	memset(&tv,0,sizeof(struct timeval));
	tv.tv_sec=tsval;
	tv.tv_usec=tusval;
	if(setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&tv,sizeof(struct timeval))==0)
	{
		if(setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,(char*)&tv,sizeof(struct timeval))==0)
		{
			return true;
		}
	}
	return false;
}
void set_speed(int fd, int speed)
{
	int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,
			B38400, B19200, B9600, B4800, B2400, B1200, B300, };
	int name_arr[] = {38400,  19200,  9600,  4800,  2400,  1200,  300, 38400,
			19200,  9600, 4800, 2400, 1200,  300, };
	int   i,status;
	struct termios   Opt;
	tcgetattr(fd, &Opt);
	for ( i= 0;  i < sizeof(speed_arr)/sizeof(int);  ++i)
	{
		if(speed == *(name_arr+i))
		{
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&Opt, *(speed_arr+i));
			cfsetospeed(&Opt, *(speed_arr+i));
			status = tcsetattr(fd, TCSANOW, &Opt);
			if  (status != 0)
			{
				fprintf(stderr,"tcsetattr fd failed:%s\n",strerror(errno));
				return;
			}
			tcflush(fd,TCIOFLUSH);
		}
	}
}
/*
 * 在读操作时，如果读不到数据，O_NDELAY会使I/O函数马上返回0，但这又衍生出一个问题，
 * 因为读取到文件末尾(EOF)时返回的也是0，这样无法区分是哪种情况。因此，O_NONBLOCK就产生出来，
 * 它在读取不到数据时会回传-1，并且设置errno为EAGAIN。
 * O_NDELAY是在System V的早期版本中引入的，在编码时，还是推荐POSIX规定的O_NONBLOCK，
 * O_NDELAY是在System O_NONBLOCK可以在open和fcntl时设置。
 */
bool InitSerialPort(int port,int baudrate,int blockflag,char checkflag,int databyte,int stopbyte)
{
	int fd;
	char devpath[20]={0};
	memset(devpath,0,20);
	sprintf(devpath,"/dev/ttyS%d",port);
	//以读写方式打开串口
	if(!blockflag)
		fd = open(devpath, O_RDWR|O_NOCTTY|O_NONBLOCK);
	else
		fd = open(devpath, O_RDWR|O_NOCTTY);
	if (-1==fd){
		fprintf(stderr,"open serialport %d failed:%s\n",port,strerror(errno));
		return false;
	}
	//设置波特率
	set_speed(fd,baudrate);
	struct termios options;
	if ( tcgetattr(fd,&options) != 0){
		perror("SetupSerial 1");
		return false;
	}
	options.c_cflag &= ~CSIZE;
	switch (databyte) /*设置数据位数*/
	{
		case 7:
			options.c_cflag |= CS7;
			break;
		case 8:
			options.c_cflag |= CS8;
			break;
		default:
			fprintf(stderr,"Unsupported data size\n"); return false;
	}
	switch(checkflag)
	{
		case 'n':                    //无校验
		case 'N':
		{
			options.c_cflag &= ~PARENB;    /* Clear parity enable */
			options.c_iflag &= ~INPCK;     /* Enable parity checking */
			break;
		}
		case 'e':                    //偶校验
		case 'E':
		{
			options.c_cflag |= PARENB;     /* Enable parity */
			options.c_cflag &= ~PARODD;   /* 转换为偶效验*/
			options.c_iflag |= INPCK;       /* Disnable parity checking */
			break;
		}
		case 'o':                    //奇校验
		case 'O':
		{
			options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
			options.c_iflag |= INPCK;             /* Disnable parity checking */
			break;
		}
		case 's':                    //Space 校验
		case 'S':
		{
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;break;
			break;
		}
		default:
			fprintf(stderr,"checkflag set error\n");
	}
	/* 设置停止位*/
	switch (stopbyte)
	{
		case 1:
			options.c_cflag &= ~CSTOPB;
			break;
		case 2:
			options.c_cflag |= CSTOPB;
		   break;
		default:
			 fprintf(stderr,"Unsupported stop bits\n");
			 return false;
	}
	/* Set input parity option */
	if (checkflag != 'n')
		options.c_iflag |= INPCK;
	tcflush(fd,TCIFLUSH);
	if(blockflag)
	{
		/*
		 * VTIME == 0, VMIN == 0          非阻塞模式
		 * VTIME >  0, VMIN == 0          带有超时机制的读操作
		 * VTIME == 0, VMIN >  0          阻塞读取最少读VMIN个字节数返回
		 * VTIME >  0, VMIN >  0
		 */
		options.c_cc[VTIME] = 50;        /* 设置超时5 seconds                  */
		options.c_cc[VMIN]  = 0;         /* Update the options and do it NOW */
	}
	if (tcsetattr(fd,TCSANOW,&options) != 0)
	{
		perror("SetupSerial 3");
		return false;
	}
	//如果不是开发终端之类的，只是串口传输数据，而不需要串口来处理，那么使用原始模式(Raw Mode)方式来通讯
	//设置方式如下：
	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input */
	options.c_oflag  &= ~OPOST;                           /*Output*/
	return true;
}

void showByte8(unsigned char ch)
{
	for(int i=7;i>=0;--i)
	{
		if(ch&(1<<i))
			printf("1");
		else
			printf("0");
	}
	printf("\n");
}
void clrAllbyte8(unsigned char * ch)
{
	*ch='\0';
}
void setByte8(unsigned char * ch,unsigned int index)
{
	*ch|=(1<<index);
}
void clrByte8(unsigned char * ch,unsigned int index)
{
	*ch&=~(1<<index);
}
int  calc_1_Num(int _n)
{
	int num=0;
	while(_n)
	{
		++num;
		_n=_n&(_n-1);
	}
	return num;
}
bool getAllPermission()
{
	if(iopl(3)!=0)
	{
		return false;
	}
	return true;
}
unsigned int crc16tab[256] =
{
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};
//以下是CRC校验相关函数
WORD updcrc16(WORD crc,int c)
{
	int tmp;
	tmp=crc^c;
	crc=(crc>>8)^crc16tab[tmp & 0xff];
	return crc;
}
WORD CalCheckCRC(unsigned char* lpBuf,int iLen)
{
	 int i,ch;
	 WORD wCRC=0xffff;
	 for(i=0;i<iLen;i++)
	 {
	   ch=lpBuf[i];
	   wCRC=updcrc16(wCRC,ch);
	 }
	 return wCRC;
}
