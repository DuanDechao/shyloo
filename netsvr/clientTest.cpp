#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
struct data
{
	int id;
	int value;
};

void str_cli(int sockfd, int iId)
{
	struct data _data;
	_data.id = iId;
	_data.value = iId + 10;
	int len = sizeof(_data) + 4;
	int ilen = htonl(len);
	char Sendbuffer[1024] = {0};
	char Recvbuffer[1024] = {0};
	memcpy(Sendbuffer, &ilen, (int)sizeof(int));
	memcpy(Sendbuffer+sizeof(int), &_data, sizeof(struct data));
	write(sockfd, Sendbuffer, len);
}
int main()
{
	int sockfd[5], n;
	char recvlinep[1024];
	struct sockaddr_in servaddr;
	int iRet = 0;
	for(int i = 0; i < 5; i++)
	{
		if((sockfd[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			iRet = -1;
			break;
		}
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(18912);
		if(inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)
		{
			iRet = -2;
			break;
		}
		
		if(connect(sockfd[i], (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		{
			iRet = -3;
			break;
		}
	}
	if(iRet < 0)
	{
		printf("iRet = %d\n", iRet);
		return 0;
	}
	
	for(int i = 0; i < 5; i++)
	{
		str_cli(sockfd[i], i);
	}
	while(true);
	return 0;
}