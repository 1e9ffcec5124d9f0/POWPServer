#include"SocketPair.h"
#include"MainWindow.h"
SocketPair::SocketPair(QTcpSocket* Left,quint32 difficulty ,bool type,QObject* parent):QObject(parent)
{
	protocolType = type;
	left = Left;
	left->setParent(this);
	right = new QTcpSocket(this);
	right->connectToHost(globalSetting["retransmissionIp"].toString(), globalSetting["retransmissionPort"].toInt());;
	loger = ((MainWindow*)(this->parent()->parent()))->listWidget;
	if (!right->isValid())
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
		connect(&pingPongTimer, &QTimer::timeout, this, &SocketPair::sendPingPong);
		connect(&pingPongTimerOutTimer, &QTimer::timeout, this, &SocketPair::pingPongTimerOut);
		initPOWP();
		QString log = QString::fromLocal8Bit("ʱ��:") + QTime::currentTime().toString() + QString::fromLocal8Bit(".IP��ַ:") + left->peerAddress().toString() + QString::fromLocal8Bit("�����ӽ���");
		loger->addItem(log);
		loger->update();
	}
}
void SocketPair::initPOWP()		//����ʼ��������ʱ���ô�
{
	//��װЭ��ͷ
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
	head += cache;	//��ʼ�ֽ�
	left->write(head);
	pingPongTimer.start(globalSetting["pingPongIntervalTime"].toInt());
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
	//�ж�Э��
	QByteArray head = left->read(sizeof(POWPHeader));
	if (head.size() < sizeof(POWPHeader))
	{
		QString log=QString::fromLocal8Bit("ʱ��:")+ QTime::currentTime().toString()+QString::fromLocal8Bit(".IP��ַ:")+left->peerAddress().toString()+QString::fromLocal8Bit("�Ͽ���ԭ��:");
		loger->addItem(log+QString::fromLocal8Bit("δ��ָ��Э�����ӣ������ر�"));
		loger->update();
		emit left->disconnected();
		return;
	}
	if (*((quint32*)head.constData()) != 0x50574f50)	//50 57 4f 50 ΪPWOP��ascii��//����
	{
		QString log = QString::fromLocal8Bit("ʱ��:") + QTime::currentTime().toString() + QString::fromLocal8Bit(".IP��ַ:") + left->peerAddress().toString() + QString::fromLocal8Bit("�Ͽ���ԭ��:");
		loger->addItem(log + QString::fromLocal8Bit("δ��ָ��Э�����ӣ������ر�"));
		loger->update();
		emit left->disconnected();
		return;
	}
	difficultyWall = (header.difficulty > difficultyWall) ? header.difficulty : difficultyWall;
	memcpy_s(&header, sizeof(POWPHeader), head.constData(), sizeof(POWPHeader));
	if (header.difficulty < difficultyWall)
	{
		QString log = QString::fromLocal8Bit("ʱ��:") + QTime::currentTime().toString() + QString::fromLocal8Bit(".IP��ַ:") + left->peerAddress().toString() + QString::fromLocal8Bit("�Ͽ���ԭ��:");
		loger->addItem(log + QString::fromLocal8Bit("����Ѷȵ���Ҫ�������ر�"));
		loger->update();
		emit left->disconnected();
		return;
	}
	if (skipCheck|| difficultyWall==0)goto skipcheck;
	if (protocolType)
	{
		if (!checkKeyLiner(header.key))
		{
			QString log = QString::fromLocal8Bit("ʱ��:") + QTime::currentTime().toString() + QString::fromLocal8Bit(".IP��ַ:") + left->peerAddress().toString() + QString::fromLocal8Bit("�Ͽ���ԭ��:");
			loger->addItem(log + QString::fromLocal8Bit("KeyУ��ʧ�ܣ������ر�"));
			loger->update();
			emit left->disconnected();
			return;
		}
	}
	else
	{
		if (!checkKeyExponential(header.key))
		{
			QString log = QString::fromLocal8Bit("ʱ��:") + QTime::currentTime().toString() + QString::fromLocal8Bit(".IP��ַ:") + left->peerAddress().toString() + QString::fromLocal8Bit("�Ͽ���ԭ��:");
			loger->addItem(log + QString::fromLocal8Bit("KeyУ��ʧ�ܣ������ر�"));
			loger->update();
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
	case STATUS_CODE_PONG:
	{
		pingPongTimerOutTimer.stop();
		isPingPongING = false;
		break;
	}
	case STATUS_CODE_ACCEPT_CHANGE:
	{
		skipCheckTimer.stop();
		skipCheck = false;
		break;
	}
	default:
		QString log = QString::fromLocal8Bit("ʱ��:") + QTime::currentTime().toString() + QString::fromLocal8Bit(".IP��ַ:") + left->peerAddress().toString() + QString::fromLocal8Bit("�Ͽ���ԭ��:");
		loger->addItem(log + QString::fromLocal8Bit("��Ч��Э��״̬�룬�����ر�"));
		loger->update();
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

void SocketPair::changeDifficulty(quint32 diff)
{
	skipCheck = true;
	QString log = QString::fromLocal8Bit("ʱ��:") + QTime::currentTime().toString() + QString::fromLocal8Bit(".IP��ַ:") + left->peerAddress().toString();
	loger->addItem(log + QString::fromLocal8Bit(",�Ѷȱ��"));
	loger->update();
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
void SocketPair::sendPingPong()
{
	if (!isPingPongING)
	{
		isPingPongING = true;
		POWPHeader header;
		header.dataLen = 0;
		header.difficulty = difficultyWall;
		header.statusCode = STATUS_CODE_PING;
		header.key = 0;
		QByteArray head((char*)&header, sizeof(POWPHeader));
		left->write(head);
		pingPongTimerOutTimer.start(globalSetting["pingPongTolerance"].toInt());
	}
}

void SocketPair::pingPongTimerOut()
{
	pingPongTimerOutTimer.stop();
	QString log = QString::fromLocal8Bit("ʱ��:") + QTime::currentTime().toString() + QString::fromLocal8Bit(".IP��ַ:") + left->peerAddress().toString() + QString::fromLocal8Bit("�Ͽ���ԭ��:");
	loger->addItem(log + QString::fromLocal8Bit("��������ʱ�������ر�"));
	loger->update();
	emit left->disconnected();
}
