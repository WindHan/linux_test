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

#define MAXLINE 1024
#define IPADDRESS "127.0.0.2"
#define SERV_PORT 8787

#define LOCADDRESS "127.1.0.3"
#define LOC_PORT   8786

#define max(a,b) (a > b) ? a : b

static int handle_recv_msg(int sockfd, char *buf) 
{
	int n;
    assert(buf);
	printf("client recv msg is:%s\n", buf);
	fgets(buf, MAXLINE, stdin);   
	n = write(sockfd, buf, strlen(buf) +1);
	
	if (n < 0)  
	{  
		perror("sendto error");  
		return -1;  
	}  
    return 0;
}

static void handle_connection(int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE];
	int maxfdp,stdineof;
	fd_set readfds;
	int n;
	struct timeval tv;
	int retval = 0;

	while (1) 
	{
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);
		maxfdp = sockfd;

		tv.tv_sec = 5;
		tv.tv_usec = 0;

		retval = select(maxfdp + 1, &readfds, NULL, NULL, &tv);

		if (retval == -1) {
			return ;
		}

		if (retval == 0) {
			printf("client timeout.\n");
			continue;
		}

		if (FD_ISSET(sockfd, &readfds)) {
			n = read(sockfd, recvline, MAXLINE);
			if (n <= 0) {
				fprintf(stderr,"client: server is closed.\n");
				close(sockfd);
				FD_CLR(sockfd, &readfds);
				return;
			}

			handle_recv_msg(sockfd, recvline);
			}
		}
	}

int main(int argc,char *argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;
	struct sockaddr_in localaddr;

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	
	bzero(&localaddr,sizeof(localaddr));
    localaddr.sin_family = AF_INET;
    inet_pton(AF_INET, LOCADDRESS, &localaddr.sin_addr);
    localaddr.sin_port = htons(LOC_PORT);

    if (bind(sockfd, (struct sockaddr*) &localaddr, sizeof(localaddr)) < 0) {
		fprintf(stderr, "bind fail,error:%d,reason:%s\n", errno, strerror(errno));
        return -1;
    }

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET,IPADDRESS,&servaddr.sin_addr);

	int retval = 0;
	retval = connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	if (retval < 0) {
		fprintf(stderr, "connect fail,error:%s\n", strerror(errno));
		return -2;
	}

	printf("client send to server .\n");
	write(sockfd, "hello server", 32);

	handle_connection(sockfd);

	return 0;												
}