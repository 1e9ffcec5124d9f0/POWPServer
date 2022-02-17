#include"NetManager.h"
NetManager::NetManager(QObject* parent):QObject(parent)
{
	globalDifficultyWall = globalSetting["globalDifficultyWall"].toInt();
	server = new QTcpServer(this);
	connect(server, &QTcpServer::newConnection, this, &NetManager::acceptNewConnection);
	server->listen(QHostAddress::AnyIPv4, globalSetting["bindPort"].toInt());
	if (!server->isListening())
	{
		throw "bind port error";
	}
}
void NetManager::acceptNewConnection()
{
	quint32 diff;
	auto temp = globalSetting["globalDifficultyWall"].toString().toUInt();
	diff = (quint32)temp;
	SocketPair* x;
	if (globalSetting["protocolType"].toString() == QString("liner"))
	{
		x = new SocketPair(server->nextPendingConnection(),diff, true, this);
	}
	else if(globalSetting["protocolType"].toString() == QString("exponential"))
	{
		x = new SocketPair(server->nextPendingConnection(),diff,false , this);
	}
	else
	{
		server->nextPendingConnection()->close();
		throw "setting.json protocolType error";
	}
	clientList.push_back(x);
	connect(x, &SocketPair::iNeedDelete, this, &NetManager::socketPairDisconnected);
	emit recvNewConnectionSignal(x);
}

void NetManager::socketPairDisconnected(SocketPair* x)
{
	for (auto i = clientList.begin(); i != clientList.end(); i++)	//可优化为hashmap
	{
		if (x == *i)
		{
			clientList.erase(i);
			return;
		}
	}
}
