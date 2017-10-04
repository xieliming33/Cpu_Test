//============================================================================
// Name        : CpuHightTest.cpp
// Author      : xieliming
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include "linux.h"
#include <iostream>
using namespace std;

int        tval=0;
bool       flag=true;
long long  L   =0;
long long  N   =0;
/* L 为位数，N是array长度
 * 圆周率后的小数位数是无止境的，如何使用电脑来计算这无止境的小数是一些数学家与程式设计师所感兴趣的，
 * 在这边介绍一个公式配合 大数运算，可以计算指定位数的圆周率。
 * J.Marchin的圆周率公式：
 * PI = [16/5 - 16 / (3*53) + 16 / (5*55) - 16 / (7*57) + ......] -  [4/239 - 4/(3*2393) + 4/(5*2395) - 4/(7*2397) + ......]
 */
void add ( long long *a, long long *b, long long *c )
{
	int i, carry = 0;
	for ( i = N+1; i >= 0; --i )
	{
		*(c+i) = *(a+i) + *(b+i) + carry;
		if ( *(c+i) < 10000 )
			carry = 0;
		else                              // 进位
		{
			*(c+i) = *(c+i) - 10000;
			carry = 1;
		}
	}
}

void sub ( long long *a, long long *b, long long *c )
{
	int i, borrow = 0;
	for ( i = N+1; i >= 0; --i )
	{
		*(c+i) = *(a+i) - *(b+i) - borrow;
		if ( *(c+i) >= 0 )
			borrow = 0;
		else                       // 借位
		{
			*(c+i) = *(c+i) + 10000;
			borrow = 1;
		}
	}
}

void div ( long long *a, long long b, long long *c ) // b 为除数
{
	long long  i, tmp, remain = 0;
	for ( i = 0; i <= N+1; ++i )
	{
		tmp = *(a+i) + remain;
		*(c+i) = tmp / b;
		remain = ( tmp % b ) * 10000;
	}
}
static void * calcTime(void *arg)
{

	while(flag)
	{
		Sec_sleep(1);
		++tval;
		setMousePos(7,51);
		printf("%d\n",tval);
	}
	setMousePos(11,0);
	return (void *)NULL;
}
bool GetKeyBorad()
{
	char ckey=GetKeyWord();
	switch(ckey)
	{
	case '1':
		L=10000;
		break;
	case '2':
		L=100000;
		break;
	case '3':
		L=500000;
		break;
	case '4':
		L=1000000;
		break;
	default:
		GetKeyBorad();
	}
	N=L/4+1;
	return true;
}

int main ( int argc,char**argv )
{
	clearScreen();
	setMousePos(0,0);
	printf("\t\t\t+++++++++++++++++++++++++++++++\n");
	printf("\t\t\t++     1. PI 1W   Bit        ++\n");
	printf("\t\t\t++     2. PI 10W  Bit        ++\n");
	printf("\t\t\t++     3. PI 50W  Bit        ++\n");
	printf("\t\t\t++     4. PI 100W Bit        ++\n");
	printf("\t\t\t+++++++++++++++++++++++++++++++\n");
	printf("\t\t\t++ Please Choose Operate:    ++\n");
	printf("\t\t\t+++++++++++++++++++++++++++++++\n");
	setMousePos(7,51);
	if(!GetKeyBorad()) return -1;
	pthread_t ts;
	pthread_create(&ts,NULL,calcTime,NULL);

	struct timeval start_time,end_time;
	long long *s=(long long*)malloc((N+3)*sizeof(long long));
	long long *w=(long long*)malloc((N+3)*sizeof(long long));
	long long *v=(long long*)malloc((N+3)*sizeof(long long));
	long long *q=(long long*)malloc((N+3)*sizeof(long long));
	memset((char *)s,0,(N+3)*sizeof(long long));
	memset((char *)v,0,(N+3)*sizeof(long long));
	memset((char *)w,0,(N+3)*sizeof(long long));
	memset((char *)q,0,(N+3)*sizeof(long long));
	memset(&start_time,0,sizeof(struct timeval));
	memset(&end_time,0,sizeof(struct timeval));

	gettimeofday(&start_time,NULL);
	long long k,n = ( long long ) ( L/1.39793 + 1 );
	*(w+0) = 16*5;
	*(v+0) = 4*239;
	for ( k = 1; k <= n; k++ )
	{
		                                           // 套用公式
		div ( w, 25, w );
		div ( v, 239, v );
		div ( v, 239, v );
		sub ( w, v, q );
		div ( q, 2*k-1, q );

		if ( k%2 )                                 // 奇数项
			add ( s, q, s );
		else                                       // 偶数项
			sub ( s, q, s );
	}

	flag=false;
	gettimeofday(&end_time,NULL);
	setMousePos(9,25);
	printf("Check CPU End Cost:%lf s\n",((end_time.tv_sec-start_time.tv_sec)+(end_time.tv_usec-start_time.tv_usec)/1000000.0));

	pthread_join(ts,NULL);
	//	print results of PI
//	printf ( "%lld.", *(s+0) );
//	for ( k = 1; k < N; k++ )
//		printf ( "%04lld", *(s+k) );
//	printf ( "\n" );

	if(s!=NULL)
	{
		free(s);
		s=NULL;
	}
	if(v!=NULL)
	{
		free(v);
		v=NULL;
	}
	if(w!=NULL)
	{
		free(w);
		w=NULL;
	}
	if(q!=NULL)
	{
		free(q);
		q=NULL;
	}
	return 0;
}
