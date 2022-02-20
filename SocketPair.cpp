#include"SocketPair.h"
SocketPair::SocketPair(QTcpSocket* Left,quint32 difficulty ,bool type,QObject* parent):QObject(parent)
{
	protocolType = type;
	left = Left;
	left->setParent(this);
	right = new QTcpSocket(this);
	right->connectToHost(globalSetting["retransmissionIp"].toString(), globalSetting["retransmissionPort"].toInt());
	if (!right->isOpen())
	{
		left->close();
		emit iNeedDelete(this);
		deleteLater();
	}
	else
	{
		difficultyWall = difficulty;
		connect(left, &QTcpSocket::disconnected, this, &SocketPair::someoneDisconnected);
		connect(right, &QTcpSocket::disconnected, this, &SocketPair::someoneDisconnected);
		connect(left, &QTcpSocket::readyRead, this, &SocketPair::leftReadyRead);
		connect(right, &QTcpSocket::readyRead, this, &SocketPair::rightReadyRead);
		connect(&skipCheckTimer, &QTimer::timeout, this, &SocketPair::skipCheckTimerTimeOut);
		initPOWP();
	}
}
void SocketPair::initPOWP()		//当初始建立连接时调用此
{
	//组装协议头
	POWPHeader header;
	header.dataLen = 0;
	header.difficulty = difficultyWall;
	if (protocolType)
	{
		header.statusCode = STATUS_CODE_INIT_LINER;
	}
	else header.statusCode = STATUS_CODE_INIT_EXPONENTIAL;
	header.key = 0;
	QByteArray head((char*)&header, sizeof(POWPHeader));
	cache = getRandomBytes(8);
	head += cache;	//初始字节
	left->write(head);
}
bool SocketPair::checkKeyExponential(quint64 key)
{
	QByteArray keyArray((char*)&key, 8);
	cache += keyArray;
	cache = QCryptographicHash::hash(cache, QCryptographicHash::Md5);
	quint64 check = 0;
	memcpy_s((uchar*)&check, 8, cache.constData(), 8);
	check = check << (64 - difficultyWall);
	if ((check & 0xffffffffffffffff) != 0)
	{
		return false;
	}
	return true;
}
bool SocketPair::checkKeyLiner(quint64 key)
{
	QByteArray keyArray((char*)&key, 8);
	cache += keyArray;
	cache = QCryptographicHash::hash(cache, QCryptographicHash::Md5);
	quint64 check = 0;
	memcpy_s((uchar*)&check, 8, cache.constData(), 8);
	if (check >=((0xffffffffffffffff >>32) * difficultyWall))
	{
		return true;
	}
	return false;
}
void SocketPair::someoneDisconnected()
{
	left->close();
	right->close();
	emit iNeedDelete(this);
	deleteLater();
}

void SocketPair::leftReadyRead()
{
	POWPHeader header;
	//判断协议
	QByteArray head = left->read(sizeof(POWPHeader));
	if (head.size() < sizeof(POWPHeader))
	{
		emit left->disconnected();
		return;
	}
	if (*((quint32*)head.constData()) != 0x50574f50)	//50 57 4f 50 为PWOP的ascii码//倒序
	{
		emit left->disconnected();
		return;
	}
	memcpy_s(&header, sizeof(POWPHeader), head.constData(), sizeof(POWPHeader));
	if (header.difficulty < difficultyWall)
	{
		emit left->disconnected();
		return;
	}
	if (difficultyWall == 0)goto skipcheck;
	if (skipCheck)goto skipcheck;
	if (protocolType)
	{
		if (!checkKeyLiner(header.key))
		{
			emit left->disconnected();
			return;
		}
	}
	else
	{
		if (!checkKeyExponential(header.key))
		{
			emit left->disconnected();
			return;
		}
	}
skipcheck:
	switch (header.statusCode)
	{
	case STATUS_CODE_SENDDATA:
	{
		right->write(left->read(header.dataLen));
		break;
	}
	default:
		emit left->disconnected();
		break;
	}
}

void SocketPair::rightReadyRead()	//转发来自服务器的数据
{
	POWPHeader header;
	header.difficulty = difficultyWall;
	header.key = 0;
	header.statusCode = STATUS_CODE_SENDDATA;
	QByteArray serverData = right->readAll();
	header.dataLen = serverData.size();
	QByteArray head((char*)&header, sizeof(POWPHeader));
	head += serverData;
	left->write(head);
}

void SocketPair::changeDifficulty(quint32 diff)
{
	skipCheck = true;
	skipCheckTimer.start(globalSetting["checkInvalidTime"].toInt());
	difficultyWall = diff;
	POWPHeader header;
	header.dataLen = 0;
	header.difficulty = diff;
	header.statusCode = STATUS_CODE_DIFFICULTY_CHANGE;
	header.key = 0;
	QByteArray head((char*)&header, sizeof(POWPHeader));
	cache = getRandomBytes(8);
	head += cache;	
	left->write(head);
}

void SocketPair::skipCheckTimerTimeOut()
{
	skipCheckTimer.stop();
	skipCheck = false;
}
