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
	bool checkKeyExponential(quint64 key);
	bool checkKeyLiner(quint64 key);
public:
	QTcpSocket* left;
	QTcpSocket* right;
	QByteArray cache;
	quint32 difficultyWall;
	bool protocolType;
	SocketPair(QTcpSocket* Left, quint32 difficulty,bool type, QObject* parent);
public slots:
	void someoneDisconnected();
	void leftReadyRead();
	void rightReadyRead();
signals:
	void iNeedDelete(SocketPair* x);

};