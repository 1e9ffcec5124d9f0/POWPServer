#pragma once
#include"tools.h"
#include<QtNetwork/qtcpserver.h>
#include<QtNetwork/qtcpsocket.h>
#include<QtNetwork/qhostaddress.h>
#include<QtCore/qtimer>
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
	SocketPair(QTcpSocket* Left, quint32 difficulty,bool type, QObject* parent);
	void changeDifficulty(quint32 diff);
public slots:
	void someoneDisconnected();
	void leftReadyRead();
	void rightReadyRead();
	void skipCheckTimerTimeOut();
signals:
	void iNeedDelete(SocketPair* x);

};