#pragma once
#include"tools.h"
#include"SocketPair.h"

class NetManager:public QObject
{
	Q_OBJECT
public:
	NetManager();
	QTcpServer* server;
	quint32 globalDifficultyWall;
	QList<SocketPair*> clientList;
public slots:
	void acceptNewConnection();
	void socketPairDisconnected(SocketPair*x);
};