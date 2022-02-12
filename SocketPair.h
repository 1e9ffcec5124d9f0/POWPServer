#pragma once
#include"tools.h"
#include<QtNetwork/qtcpserver.h>
#include<QtNetwork/qtcpsocket.h>
#include<QtNetwork/qhostaddress.h>
class SocketPair:public QObject
{
	Q_OBJECT
protected:
	void initPOWP();
	bool checkKey(quint64 key);
public:
	QTcpSocket* left;
	QTcpSocket* right;
	QByteArray cache;
	quint8 difficultyWall;
	SocketPair(QTcpSocket* Left, quint8 difficulty, QObject* parent);
public slots:
	void someoneDisconnected();
	void leftReadyRead();
	void rightReadyRead();
signals:
	void iNeedDelete(SocketPair* x);

};