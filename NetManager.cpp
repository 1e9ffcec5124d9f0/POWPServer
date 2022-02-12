#include"NetManager.h"
NetManager::NetManager()
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
	SocketPair* x = new SocketPair(server->nextPendingConnection(), globalDifficultyWall,this);
	clientList.push_back(x);
	connect(x, &SocketPair::iNeedDelete, this, &NetManager::socketPairDisconnected);
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
