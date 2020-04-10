/*************************************************************************
    > File Name: recvthread.cpp
    > Author: wangzhi
    > Mail: wangzhi@crearo.com 
    > Created Time: Friday, April 10, 2020 PM01:50:34 CST
 ************************************************************************/

#include "recvthread.h"
#include <QMutex> 

RecvThread::RecvThread(QObject *parent) :
	QThread(parent)
{
} 

void RecvThread::run()
{
	char *r_data;
	r_data = (char *)calloc(1, 512);

	while (1)
	{
		r_data = client_recv();
		if (r_data == NULL)
		{
			free(r_data);
			return ;
		}
		else
		{
			QString strText = QString(QLatin1String(r_data));
			emit RecvSignal(strText);
		}
	}

	free(r_data);
}
