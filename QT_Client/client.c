/*************************************************************************
    > File Name: client.c
    > Author: wangzhi
    > Mail: wangzhi@crearo.com 
    > Created Time: Tuesday, April 07, 2020 AM10:19:22 CST
 ************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

static int c_Socket;

char *client_recv()
{
	int iRecvLen;
	static char p_RecvBuf[512];

	memset(p_RecvBuf, 0, sizeof(p_RecvBuf));
	iRecvLen = recv(c_Socket, p_RecvBuf, sizeof(p_RecvBuf), 0);
	if (iRecvLen == -1)
	{
		printf("recv failed: %d!\r\n", errno);
		close(c_Socket);
	
		return NULL;
	}
		
	p_RecvBuf[iRecvLen] = '\0';
	
	return p_RecvBuf;
}

int client_init(const char *s_ip, const char *s_port, const char *s_name)
{
	char s_IP[32];
	char p_Name[64];
	unsigned short us_Port;

	strcpy(s_IP, s_ip);
	us_Port = atoi(s_port);
	strcpy(p_Name, s_name);

	int i_SendLen;
	struct sockaddr_in s_Addr;
	if ((c_Socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket failed.\r\n");
		return -1;
	}
	
	s_Addr.sin_family = AF_INET;
	s_Addr.sin_port = htons(us_Port);
	s_Addr.sin_addr.s_addr = inet_addr(s_IP);
	if(connect(c_Socket, (struct sockaddr *)&s_Addr, sizeof(s_Addr)) < 0)
	{
		printf("connect failed.\r\n"); 
		return -1;
	}

	i_SendLen = send(c_Socket, p_Name, strlen(p_Name), 0);
	if (i_SendLen == -1)
	{
		printf("send name failed: %d!\r\n", errno);
		close(c_Socket);
		return -1;
	}

	return c_Socket;
}

int client_send(const char *p_SendBuf)
{
	int i_SendLen;
	i_SendLen = send(c_Socket, p_SendBuf, strlen(p_SendBuf), 0);
	if (i_SendLen == -1)
	{
		printf("send message failed: %d!\r\n", errno);
		close(c_Socket);
		return -1;
	}

	return 0;
}
