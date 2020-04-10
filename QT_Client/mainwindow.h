#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include "ui_mainwindow.h"
#include "recvthread.h"
#include "client.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	explicit MainWindow(char *s_Name, QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow *ui;
	RecvThread *r_thread;
	char s_UserName[64];

private:
	QPushButton *m_button;
	QTextEdit *m_text_1;
	QTextEdit *m_text_2;

private slots:
	void myslot();
	void DisplayMsg(QString);
};

#endif // MAINWINDOW_H
