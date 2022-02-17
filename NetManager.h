#pragma once
#include"tools.h"
#include"SocketPair.h"
#include<QtCore/qmutex.h>
class NetManager:public QObject
{
	Q_OBJECT
public:
	NetManager(QObject* parent);
	QTcpServer* server;
	quint32 globalDifficultyWall;
	QList<SocketPair*> clientList;
public slots:
	void acceptNewConnection();	//����������ʱ����
	void socketPairDisconnected(SocketPair*x);
signals:
	void recvNewConnectionSignal(SocketPair* x);	//���ɹ��������Ӻ󷢳�
};