/*************************************************************************
    > File Name: client.h
    > Author: wangzhi
    > Mail: wangzhi@crearo.com 
    > Created Time: Thursday, April 09, 2020 AM10:56:04 CST
 ************************************************************************/
#ifndef CLIENT_H
#define CLIENT_H

#ifdef __cplusplus
extern "C"{
#endif

int client_init(const char *s_ip, const char *s_port, const char *s_name);

int client_send(const char *p_SendBuf);

char *client_recv();

#ifdef __cplusplus
}
#endif

#endif // CLIENT_H
