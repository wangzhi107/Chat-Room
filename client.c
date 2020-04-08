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

void p_Recv(void *Param)
{
	int c_Socket;
	int iRecvLen;
	char p_RecvBuf[512] = "";
	c_Socket = *((int*)Param);

	while (1)
	{
		memset(p_RecvBuf, 0, sizeof(p_RecvBuf));
		iRecvLen = recv(c_Socket, p_RecvBuf, 512, 0);
		if (iRecvLen == -1)
		{
			printf("recv failed: %d!\r\n", errno);
			close(c_Socket);
			return;
		}
		p_RecvBuf[iRecvLen] = '\0';
		printf("\033[1m\033[40;33m%s\033[0m\r\n", p_RecvBuf);
	}
}

int main(int argc, char *argv[])
{
	char s_IP[32];
	char p_Name[64];
	unsigned short us_Port;

	if (argc < 4)
	{
		printf("Param wrong, such as:\r\n");
		printf("./client 192.168.1.100(server's IP) 16666(server's port) Jay(your name)\r\n");
		return -1;
	}
	strcpy(s_IP, argv[1]);
	us_Port = atoi(argv[2]);
	strcpy(p_Name, argv[3]);

	int i_SendLen;
	char p_SendBuf[512] = "";
	pthread_t thr_id;
	
	int c_Socket;
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
	
	if (pthread_create(&thr_id, NULL, (void *(*)(void *))p_Recv, &c_Socket) != 0)
	{
		printf("pthread failed.\r\n");
		return -1;
	}

	i_SendLen = send(c_Socket, p_Name, strlen(p_Name), 0);
	if (i_SendLen == -1)
	{
		printf("send name failed: %d!\r\n", errno);
		close(c_Socket);
		return -1;
	}

	while(1) 
	{
		scanf("%s", p_SendBuf);
		printf("\033[1m\033[40;31m%s: %s\033[0m\r\n", p_Name, p_SendBuf);
		i_SendLen = send(c_Socket, p_SendBuf, strlen(p_SendBuf), 0);
		if (i_SendLen == -1)
		{
			printf("send message failed: %d!\r\n", errno);
			close(c_Socket);
			return -1;
		}

		if (strcmp(p_SendBuf, "quit") == 0)
			break;
	}
	
	pthread_join(thr_id, NULL);
	close(c_Socket); 
	return 0;
}
