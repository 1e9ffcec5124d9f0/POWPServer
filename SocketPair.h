#pragma once
#include"tools.h"
#include<QtNetwork/qtcpserver.h>
#include<QtNetwork/qtcpsocket.h>
#include<QtNetwork/qhostaddress.h>
#include<QtCore/qtimer>
#include <QtWidgets/qlistwidget.h>
class SocketPair:public QObject
{
	Q_OBJECT
protected:
	void initPOWP();
	bool checkKeyExponential(quint64 key);
	bool checkKeyLiner(quint64 key);
public:
	QTcpSocket* left;
	QTcpSocket* right;
	QByteArray cache;
	quint32 difficultyWall;
	bool skipCheck = false;
	bool protocolType;
	QTimer skipCheckTimer;
	QTimer pingPongTimer;
	QTimer pingPongTimerOutTimer;
	bool isPingPongING = false;
	SocketPair(QTcpSocket* Left, quint32 difficulty,bool type, QObject* parent);
	void changeDifficulty(quint32 diff);
	QListWidget* loger;
public slots:
	void someoneDisconnected();
	void leftReadyRead();
	void rightReadyRead();
	void skipCheckTimerTimeOut();
	void sendPingPong();
	void pingPongTimerOut();
signals:
	void iNeedDelete(SocketPair* x);
	
};