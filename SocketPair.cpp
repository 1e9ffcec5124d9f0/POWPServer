#include"SocketPair.h"
SocketPair::SocketPair(QTcpSocket* Left,quint8 difficulty ,QObject* parent):QObject(parent)
{
	left = Left;
	left->setParent(this);
	right = new QTcpSocket(this);
	right->connectToHost(globalSetting["retransmissionIp"].toString(), globalSetting["retransmissionPort"].toInt());
	if (!right->isOpen())
	{
		left->close();
		deleteLater();
	}
	else
	{
		difficultyWall = difficulty;
		connect(left, &QTcpSocket::disconnected, this, &SocketPair::someoneDisconnected);
		connect(right, &QTcpSocket::disconnected, this, &SocketPair::someoneDisconnected);
		connect(left, &QTcpSocket::readyRead, this, &SocketPair::leftReadyRead);
		connect(right, &QTcpSocket::readyRead, this, &SocketPair::rightReadyRead);
		initPOWP();
	}
}
void SocketPair::initPOWP()		//����ʼ��������ʱ���ô�
{
	//��װЭ��ͷ
	POWPHeader header;
	header.dataLen = 0;
	header.difficulty = difficultyWall;
	header.statusCode = STATUS_CODE_INIT;
	header.key = 0;
	QByteArray head((char*)&header, sizeof(POWPHeader));
	cache = getRandomBytes(8);
	head += cache;	//��ʼ�ֽ�
	left->write(head);
}
bool SocketPair::checkKey(quint64 key)
{
	QByteArray keyArray((char*)&key, 8);
	cache += keyArray;
	QByteArray cache = QCryptographicHash::hash(cache, QCryptographicHash::Md5);
	quint64 check = 0;
	memcpy_s((uchar*)&check, 8, cache.constData(), 8);
	check = check << (64 - difficultyWall);
	if ((check & 0xffffffffffffffff) != 0)
	{
		return false;
	}
	return true;
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
	//�ж�Э��
	QByteArray head = left->read(sizeof(POWPHeader));
	if (head.size() < sizeof(POWPHeader))
	{
		emit left->disconnected();
		return;
	}
	if (*((quint32*)head.constData()) != 0x50574f50)	//50 57 4f 50 ΪPWOP��ascii��//����
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
	if (!checkKey(header.key))
	{
		emit left->disconnected();
		return;
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

void SocketPair::rightReadyRead()	//ת�����Է�����������
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