/*************************************************************************
    > File Name: recvthread.h
    > Author: wangzhi
    > Mail: wangzhi@crearo.com 
    > Created Time: Friday, April 10, 2020 PM01:52:34 CST
 ************************************************************************/

#ifndef RECVTHREAD_H
#define RECVTHREAD_H 

#include <QThread> 
#include "client.h"

class RecvThread : public QThread
{
	Q_OBJECT

public:    
	explicit RecvThread(QObject *parent = 0);

protected:    
	virtual void run(); 

signals:
	void RecvSignal(QString);  

};

#endif // RECVTHREAD_H
