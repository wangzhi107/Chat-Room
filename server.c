/*************************************************************************
    > File Name: server.c
    > Author: wangzhi
    > Mail: wangzhi@crearo.com 
    > Created Time: Tuesday, April 07, 2020 PM05:00:35 CST
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

#define INVALID_SOCKET	-1
#define CLIENT_NUM		8

typedef struct _Client  
{  
	int c_Socket;
	pthread_t thr_recv_id;
    char r_Buffer[512];
    char s_Buffer[512];
    char s_UserName[64];
    char s_IP[32];
    int i_First;
}Client;

Client g_Client[CLIENT_NUM] = {0};  
      
int s_Flag = 0;  
int r_Flag = 0;  
 
void *p_Send(void* param)  
{
	int i, ret;
	int test = 0;

	while (1)
    {
		if (s_Flag == 0)
		{
			continue ;
		}
        else
        {
            for (i = 0; i < CLIENT_NUM; i ++)
            {
				if ((strlen(g_Client[r_Flag].r_Buffer) != 0) && (g_Client[i].thr_recv_id != 0) && (i != r_Flag))
                {
					memset(g_Client[i].s_Buffer, 0, sizeof(g_Client[i].s_Buffer));
					memcpy(g_Client[i].s_Buffer, g_Client[r_Flag].r_Buffer, sizeof(g_Client[i].s_Buffer));
					printf("send - %d: %s\r\n", i, g_Client[i].s_Buffer);
					ret = send(g_Client[i].c_Socket, g_Client[i].s_Buffer, strlen(g_Client[i].s_Buffer), 0);
					if (ret <= 0)
					{
						printf("%d send failed.\r\n", i);
						return NULL;
					}
                }
            }
  
			r_Flag = 0;
            s_Flag = 0;
		}
	}

	return NULL;
}

void *p_Recv(void *Param)  
{
	int i, ret;
	int i_Flag = 0;  
    char s_Temp[512] = {0};  
	int c_Socket;
	c_Socket = *((int*)Param);

	for (i = 0; i < CLIENT_NUM; i ++)  
    {  
		if ((g_Client[i].c_Socket == c_Socket) && (g_Client[i].c_Socket != INVALID_SOCKET))  
        {
			i_Flag = i;
        }
    }

    while (1)
    {
        memset(s_Temp, 0, sizeof(s_Temp));
        ret = recv(c_Socket, s_Temp, sizeof(s_Temp), 0);
        if (ret <= 0)
        {
			printf("%d recv failed.\r\n", i_Flag);
			g_Client[i_Flag].thr_recv_id = 0;
			shutdown(g_Client[i_Flag].c_Socket, SHUT_RDWR);
			g_Client[i_Flag].c_Socket = INVALID_SOCKET;

			return NULL;
        }

        if (g_Client[i_Flag].i_First == 1)
		{
			memset(g_Client[i_Flag].r_Buffer, 0, sizeof(g_Client[i_Flag].r_Buffer));
			sprintf(g_Client[i_Flag].r_Buffer, "%s:\r\n    %s", g_Client[i_Flag].s_UserName, s_Temp);
			printf("recv - %d: %s\r\n", i_Flag, g_Client[i_Flag].r_Buffer);  
		}

		//用户名
		if (g_Client[i_Flag].i_First == 0)
        {
            memcpy(g_Client[i_Flag].s_UserName, s_Temp, sizeof(s_Temp));
            g_Client[i_Flag].i_First = 1;
        }
        else
        {
			r_Flag = i_Flag;
            s_Flag = 1;
        }
	}

	return NULL;
}

void *p_Accept(void *Param)  
{
	int i, num;
	struct sockaddr_in c_Addr;
	int c_AddrLen = sizeof(c_Addr);

	int c_Socket;
	c_Socket = *((int*)Param);

    while (1)
	{
		num = 0;
        for (i = 0; i < CLIENT_NUM; i ++)
        {
			if (g_Client[i].c_Socket != INVALID_SOCKET)
            {
                num ++;
                continue ;
            }
            else
            {
                g_Client[i].c_Socket = accept(c_Socket, (struct sockaddr*)&c_Addr, (socklen_t*)&c_AddrLen);  
                if (g_Client[i].c_Socket == INVALID_SOCKET)  
                {  
                    printf("accept failed.\r\n");
                    close(c_Socket);
                    return NULL;
                }
				printf("%d: %s\r\n", i, inet_ntoa(c_Addr.sin_addr));
                memcpy(g_Client[i].s_IP, inet_ntoa(c_Addr.sin_addr), sizeof(g_Client[i].s_IP));
                num ++;
                break ;
            }
        }

		//if (num >= 2)
        {
            for (i = 0; i < CLIENT_NUM; i ++)
            {
                if ((g_Client[i].c_Socket != INVALID_SOCKET) && (g_Client[i].thr_recv_id == 0))
                {
					pthread_attr_t attr;
					pthread_attr_init(&attr);
					pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
					pthread_create(&g_Client[i].thr_recv_id, &attr, (void *(*)(void *))p_Recv, &g_Client[i].c_Socket);
					pthread_attr_destroy(&attr);
				}
			}
        }
    }

	return NULL;
}

int main(int argc, char *argv[])  
{
	int i;
	unsigned short us_Port;
	if (argc < 2)
	{
		printf("Param wrong, such as:\r\n");
		printf("./server 16666(server's port)\r\n");
		return -1;
	}
	us_Port = atoi(argv[1]);

    for (i = 0; i < CLIENT_NUM; i ++)  
	{
		g_Client[i].c_Socket = INVALID_SOCKET;
		g_Client[i].thr_recv_id = 0;
		memset(g_Client[i].r_Buffer, 0, sizeof(g_Client[i].r_Buffer));
		memset(g_Client[i].s_Buffer, 0, sizeof(g_Client[i].s_Buffer));
		memset(g_Client[i].s_IP, 0, sizeof(g_Client[i].s_IP));
		memset(g_Client[i].s_UserName, 0, sizeof(g_Client[i].s_UserName));
		g_Client[i].i_First = 0;
	}

	int s_Socket;   
	struct sockaddr_in s_Addr;
	if ((s_Socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket failed\r\n");               
		return -1;
	}
	
	bzero(&s_Addr, sizeof(s_Addr));
	s_Addr.sin_family = AF_INET;
	s_Addr.sin_port = htons(us_Port);
	s_Addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(s_Socket, (struct sockaddr *)&s_Addr, sizeof(s_Addr)) < 0)
	{
		printf("connect failed\r\n");
		return -1;
	}
	
	if (listen(s_Socket, 5) < 0)
	{
		printf("listen failed\r\n");
	    return -1;
	}

	pthread_t thr_send_id;
	pthread_t thr_accept_id;
	
	if (pthread_create(&thr_send_id, NULL, (void *(*)(void *))p_Send, NULL) != 0)
	{
		printf("pthread send failed.\r\n");
		return -1;
	}

	if (pthread_create(&thr_accept_id, NULL, (void *(*)(void *))p_Accept, &s_Socket) != 0)
	{
		printf("pthread send failed.\r\n");
		return -1;
	}

	//主线程休眠
    for (i = 0; i < 1000; i ++)
    {
		sleep(10000000);  
	}
      
    for (i = 0; i < CLIENT_NUM; i ++)  
    {  
		if (g_Client[i].c_Socket != INVALID_SOCKET)
		{
			close(g_Client[i].c_Socket);
		}
    }

	pthread_join(thr_send_id, NULL);
	pthread_join(thr_accept_id, NULL);
	close(s_Socket);
	
	return 0;
}

