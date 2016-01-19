/*============================================================================
File Name:select_mcast_client.cpp
Purples:学习多播原理

============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>

#define MAXLINE     1024
#define LOCADDRESS  "127.0.0.2"
#define LOC_PORT	8888

#define MCASTADDR   "224.0.0.88"
#define MCASTPORT	8785

#define max(a,b) (a > b) ? a : b

#define MCAST_INTERVAL    5 

int main(int argc, char*argv[])
{
	int fd;
	char buf[MAXLINE];
	
	struct sockaddr_in localaddr;
	struct sockaddr_in mcast_addr;

	fd = socket(AF_INET, SOCK_DGRAM, 0); /*建立套接字*/

	if (fd < 0) {
		fprintf(stderr, "socket fail,error:%d,reason:%s\n", errno, strerror(errno));
		return -1; 
	}
    
	bzero(&localaddr,sizeof(localaddr));
    localaddr.sin_family = AF_INET;
    inet_pton(AF_INET, LOCADDRESS, &localaddr.sin_addr);
    localaddr.sin_port = htons(LOC_PORT);

    if (bind(fd, (struct sockaddr*) &localaddr, sizeof(localaddr)) < 0) {
		fprintf(stderr, "bind fail,error:%d,reason:%s\n", errno, strerror(errno));
        return -1;
    }
	
	memset(&mcast_addr, 0, sizeof(mcast_addr));/*初始化IP多播地址为0*/
	mcast_addr.sin_family      = AF_INET; /*设置协议族类行为AF*/
	mcast_addr.sin_addr.s_addr = inet_addr(MCASTADDR);/*设置多播IP地址*/
	mcast_addr.sin_port        = htons(MCASTPORT); /*设置多播 目的端口，多播也要指明接受者的端口号，不可能接受者的所有端口都来收听广播。*/

	/*向多播地址发送数据*/

	while(1) 
	{
		int n;
		fgets(buf, MAXLINE, stdin); 
		n = sendto(fd, buf, sizeof(buf), 0, (struct sockaddr*) &mcast_addr, sizeof(mcast_addr));
		
		if( n < 0) {
			fprintf(stderr, "socket fail,error:%d,reason:%s\n", errno, strerror(errno));
			return -2;
		}
	}
	sleep(MCAST_INTERVAL);    /*等待一段时间*/

	return 0;
}