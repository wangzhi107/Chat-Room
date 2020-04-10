#include "mainwindow.h"
#include "client.h"
#include <QApplication>
#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	int socket = client_init(argv[1], argv[2], argv[3]);	
	if (socket == -1)
	{
		return -1;
	}

	MainWindow w(argv[3]);
    w.show();

    return a.exec();
}
