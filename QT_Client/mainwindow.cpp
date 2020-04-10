#include "mainwindow.h"

MainWindow::MainWindow(char *s_Name, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	this->resize(QSize(460, 460));
	this->setWindowTitle("聊天室");

	m_button = new QPushButton(this);
	m_button->setText("发送");
	m_button->setGeometry(360, 410, 90, 40);
	
	m_text_1 = new QTextEdit(this);
	m_text_1->setGeometry(10, 310, 440, 90);

	m_text_2 = new QTextEdit(this);
	m_text_2->setGeometry(10, 10, 440, 290);

	r_thread = new RecvThread;
	connect(r_thread, SIGNAL(RecvSignal(QString)), this, SLOT(DisplayMsg(QString)));
	r_thread->start();

	strcpy(s_UserName, s_Name);
	connect(m_button, SIGNAL(clicked()), this, SLOT(myslot()));
}

void MainWindow::DisplayMsg(QString strText)  
{
	m_text_2->setTextColor(QColor(255, 107, 107));
	m_text_2->append(strText);
}

void MainWindow::myslot()
{
	QString strText = m_text_1->toPlainText();
	std::string str = strText.toStdString();
	client_send(str.c_str());

	char p_ShowText[512] = "";
	sprintf(p_ShowText, "%s:\r\n    %s", s_UserName, str.c_str());

	m_text_2->setTextColor(QColor(100, 184, 255));
	QString strNewText = QString(QLatin1String(p_ShowText));
	m_text_2->append(strNewText);
	m_text_1->clear();
}

MainWindow::~MainWindow()
{
    delete ui;
}
