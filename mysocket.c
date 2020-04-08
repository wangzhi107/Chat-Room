/*************************************************************************
    > File Name: mysocket.c
    > Author: wangzhi
    > Mail: wangzhi@crearo.com 
    > Created Time: Thursday, April 02, 2020 PM03:56:41 CST
 ************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_TCP_SERVER		0
#define SOCKET_TCP_CLIENT		1
#define SOCKET_UDP_SERVER		2
#define SOCKET_UDP_CLIENT		3

int tcp_server(unsigned short ts_port)
{
	int serverSocket;   
	struct sockaddr_in server_addr;
	struct sockaddr_in clientAddr;
	
	int addr_len = sizeof(clientAddr);
	int client;
	int iDataNum;
	char send_buffer[1024];
	char recv_buffer[1024];

	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");               
		return -1;
	}
	
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(ts_port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("connect");
		return -1;
	}
	
	if (listen(serverSocket, 5) < 0)
	{
		perror("listen");
	    return -1;
	}
	
	while (1)
	{
		client = accept(serverSocket, (struct sockaddr*)&clientAddr, (socklen_t*)&addr_len);
		if (client < 0)
	    {
		    perror("accept");
			continue;
		}                                                       

		pid_t pid = fork();
		if (pid == 0)
		{
			while (1)
			{
				send_buffer[0] = '\0';
				scanf("%s", send_buffer);
		        printf("\n");
				send(client, send_buffer, strlen(send_buffer), 0);
			    if (strcmp(send_buffer, "quit") == 0)
				    break;
			}
		}
		else
		{
			while (1)
			{
			    recv_buffer[0] = '\0';
				iDataNum = recv(client, recv_buffer, 1024, 0);
				if (iDataNum < 0)
		        {
			        perror("recv null");
				    continue;
		        }
	                                                                      
		        recv_buffer[iDataNum] = '\0';
				if (strcmp(recv_buffer, "quit") == 0)
			        break;
				printf("%s\n", recv_buffer);
			}
		}
	}

	close(serverSocket);
	return 0;
}

int tcp_client(char* tc_ip, unsigned short tc_port)
{
	int clientSocket;
	struct sockaddr_in serverAddr;
	char sendbuf[1024];
	char recvbuf[1024];
	int iDataNum;

	if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		return -1;
	}
	
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(tc_port);
	serverAddr.sin_addr.s_addr = inet_addr(tc_ip);
	if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("connect"); 
		return -1;
	}
	
	while (1) 
	{
		scanf("%s", sendbuf);
		printf("\n");
		send(clientSocket, sendbuf, strlen(sendbuf), 0);
		if (strcmp(sendbuf, "quit") == 0)
			break;
		
		recvbuf[0] = '\0';
		iDataNum = recv(clientSocket, recvbuf, 1024, 0);
		recvbuf[iDataNum] = '\0';
		printf("%s\n", recvbuf);
	}
	
	close(clientSocket); 
	return 0;
}

int udp_server(unsigned short us_port)
{
	int iDataNum;
	char buffer[1024];
	
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(us_port);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	int ret = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	if (0 > ret)
	{

		printf("bind\n");
		return -1;
	}
	
	struct sockaddr_in cli;
	socklen_t len = sizeof(cli);
	
	while (1)
	{
		buffer[0] = '\0';
		iDataNum = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cli, &len);
		if (iDataNum < 0)
	    {
	        perror("recv null");
	        continue;
	    }
	                                                                      
	    buffer[iDataNum] = '\0';
		if (strcmp(buffer, "quit") == 0)
	        break;
		printf("%s\n", buffer);
			
		scanf("%s", buffer);
	    printf("\n");
		sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cli, len);
	    if (strcmp(buffer, "quit") == 0)
	        break;
	}
	
	close(sockfd);
	return 0;
}

int udp_client(char* uc_ip, unsigned short uc_port)
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	char sendbuf[1024];
	char recvbuf[1024];
	int iDataNum;

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(uc_port);
	addr.sin_addr.s_addr = inet_addr(uc_ip);

	socklen_t len;
	while (1) 
	{
		scanf("%s", sendbuf);
		printf("\n");
		sendto(sockfd, sendbuf, sizeof(sendbuf), 0, (struct sockaddr*)&addr, sizeof(addr));
		if (strcmp(sendbuf, "quit") == 0)
			break;
		
		len = sizeof(addr);
		recvbuf[0] = '\0';
		iDataNum = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&addr, &len);
		recvbuf[iDataNum] = '\0';
		printf("%s\n", recvbuf);
	}
	
	close(sockfd);
	return 0;
}

int my_client(int socket_type, char* c_ip, unsigned short c_port)
{
	int rv = -1;

	if (socket_type == SOCKET_TCP_CLIENT)
	{
		if (tcp_client(c_ip, c_port))
		{
			printf("tcp_client fail!\r\n");
			return rv;
		}
	}
	else if (socket_type == SOCKET_UDP_CLIENT)
	{
		if (udp_client(c_ip, c_port))
		{
			printf("udp_client fail!\r\n");
			return rv;
		}
	}
	else
	{
		printf("Unknown client type %d.\r\n", socket_type);
		return rv;
	}

	return 0;
}

int my_server(int socket_type, unsigned short s_port)
{
	int rv = -1;

	if (socket_type == SOCKET_TCP_SERVER)
	{
		if (tcp_server(s_port))
		{
			printf("tcp_server fail!\r\n");
			return rv;
		}
	}
	else if (socket_type == SOCKET_UDP_SERVER)
	{
		if (udp_server(s_port))
		{
			printf("udp_server fail!\r\n");
			return rv;
		}
	}
	else
	{
		printf("Unknown server type %d.\r\n", socket_type);
		return rv;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int rv = -1;
	int s_type = -1;

	if (argc < 2)
	{
		printf("Please socket type and other prama, sush as:\r\n");
		printf("server - ./mysocket 0 16666\r\n");
		printf("client - ./mysocket 1 192.168.1.100 16666\r\n");
		return rv;
	}

	s_type = atoi(argv[1]);

	if ((s_type == 0) || (s_type == 2))
	{
		if (argc < 3)
		{
			printf("Server Param wrong!\r\n");
			printf("Please enter port, sush as ./mysocket 0 16666\r\n");
			return rv;
		}
		
		if (my_server(s_type, atoi(argv[2])))
		{
			printf("my_server fail!\r\n");
			return rv;
		}
	}
	else if ((s_type == 1) || (s_type == 3))
	{
		if (argc < 4)
		{
			printf("Client Param wrong!\r\n");
			printf("Please enter ip and port, sush as ./mysocket 1 192.168.1.100 16666\r\n");
			return rv;
		}
		
		if (my_client(s_type, argv[2], atoi(argv[3])))
		{
			printf("my_client fail!\r\n");
			return rv;
		}
	}
	else
	{
		printf("Unknown socket type %d!\r\n", s_type);
		printf("Support Socket-Type such as:\r\n");
		printf("	0 - tcp server\r\n");
		printf("	1 - tcp client\r\n");
		printf("	2 - udp server\r\n");
		printf("	3 - udp client\r\n");
		return rv;
	}

	return 0;
}
