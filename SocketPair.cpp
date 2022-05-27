#include"SocketPair.h"
#include"MainWindow.h"
Checker SocketPair::checker;
SocketPair::SocketPair(QTcpSocket* Left,quint32 difficulty ,bool type,QObject* parent):QObject(parent)
{
	isDefaultPassStrategy = globalSetting["defaultPassStrategy"].toBool();
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
	//	left->setReadBufferSize(128);
	//	right->setReadBufferSize(128);
	left->setSocketOption(QAbstractSocket::LowDelayOption, true);
	right->setSocketOption(QAbstractSocket::LowDelayOption, true);
	//	left->setSocketOption(QAbstractSocket::KeepAliveOption, true);
	//	right->setSocketOption(QAbstractSocket::KeepAliveOption, true);
		difficultyWall = difficulty;
		connect(left, &QTcpSocket::disconnected, this, &SocketPair::someoneDisconnected);
		connect(right, &QTcpSocket::disconnected, this, &SocketPair::someoneDisconnected);
		if (isDefaultPassStrategy)
		{
			connect(left, &QTcpSocket::readyRead, this, &SocketPair::leftReadReadDefaultPassStrategy);
			connect(&timer, &QTimer::timeout, this, &SocketPair::leftReadReadDefaultPassStrategy);
			checker.init(this);
			checker.start();
		}
		else
		{
			connect(left, &QTcpSocket::readyRead, this, &SocketPair::leftReadyRead);
			connect(&timer, &QTimer::timeout, this, &SocketPair::leftReadyRead);
		}
		connect(right, &QTcpSocket::readyRead, this, &SocketPair::rightReadyRead);
		connect(&timer, &QTimer::timeout, this, &SocketPair::rightReadyRead);
		connect(&skipCheckTimer, &QTimer::timeout, this, &SocketPair::skipCheckTimerTimeOut);
		connect(&pingPongTimer, &QTimer::timeout, this, &SocketPair::sendPingPong);
		connect(&pingPongTimeOutTimer, &QTimer::timeout, this, &SocketPair::pingPongTimerOut);
		initPOWP();
		QString log = QString::fromLocal8Bit("ʱ��:") + QTime::currentTime().toString() + QString::fromLocal8Bit(".IP��ַ:") + left->peerAddress().toString() + QString::fromLocal8Bit("�����ӽ���");
		loger->addItem(log);
		loger->update();
		timer.start(2);
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
	checker.deletePair(this);
	deleteLater();
}
void SocketPair::leftReadReadDefaultPassStrategy()
{
	POWPHeader header;
	//�ж�Э��
	QByteArray head = left->read(sizeof(POWPHeader));
	if (head.isEmpty())return;
	if (head.size() < sizeof(POWPHeader))
	{
		QString log = QString::fromLocal8Bit("ʱ��:") + QTime::currentTime().toString() + QString::fromLocal8Bit(".IP��ַ:") + left->peerAddress().toString() + QString::fromLocal8Bit("�Ͽ���ԭ��:");
		loger->addItem(log + QString::fromLocal8Bit("δ��ָ��Э�����ӣ������ر�"));
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
	if (skipCheck || difficultyWall == 0)goto skipcheck;
	if (header.difficulty < difficultyWall)
	{
		QString log = QString::fromLocal8Bit("ʱ��:") + QTime::currentTime().toString() + QString::fromLocal8Bit(".IP��ַ:") + left->peerAddress().toString() + QString::fromLocal8Bit("�Ͽ���ԭ��:");
		loger->addItem(log + QString::fromLocal8Bit("����Ѷȵ���Ҫ�������ر�"));
		loger->update();
		emit left->disconnected();
		return;
	}
	checker.pushKey(this, header.key);
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
		pingPongTimeOutTimer.stop();
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
	if (skipCheck|| difficultyWall==0)goto skipcheck;
	if (header.difficulty < difficultyWall)
	{
		QString log = QString::fromLocal8Bit("ʱ��:") + QTime::currentTime().toString() + QString::fromLocal8Bit(".IP��ַ:") + left->peerAddress().toString() + QString::fromLocal8Bit("�Ͽ���ԭ��:");
		loger->addItem(log + QString::fromLocal8Bit("����Ѷȵ���Ҫ�������ر�"));
		loger->update();
		emit left->disconnected();
		return;
	}
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
		pingPongTimeOutTimer.stop();
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
	QByteArray serverData = right->read(1460-sizeof(POWPHeader));
	if (!serverData.isEmpty())
	{
		POWPHeader header;
		header.difficulty = difficultyWall;
		header.key = 0;
		header.statusCode = STATUS_CODE_SENDDATA;
		header.dataLen = serverData.size();
		QByteArray head((char*)&header, sizeof(POWPHeader));
		head += serverData;
		left->write(head);
	}
	
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
	if (isDefaultPassStrategy)
	{
		checker.deletePair(this);
		checker.init(this);
	}
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
		pingPongTimeOutTimer.start(globalSetting["pingPongTolerance"].toInt());
	}
}
void SocketPair::pingPongTimerOut()
{
	pingPongTimeOutTimer.stop();
	QString log = QString::fromLocal8Bit("ʱ��:") + QTime::currentTime().toString() + QString::fromLocal8Bit(".IP��ַ:") + left->peerAddress().toString() + QString::fromLocal8Bit("�Ͽ���ԭ��:");
	loger->addItem(log + QString::fromLocal8Bit("��������ʱ�������ر�"));
	loger->update();
	emit left->disconnected();
}
void Checker::start()
{
	if (threadCount < globalSetting["defaultPassStrategyCheckThread"].toInt())
	{
		QThreadPool::globalInstance()->start(this);
		threadCount++;
	}
}
void Checker::init(SocketPair* x)
{
	auto temp = new checkContainer;
	temp->pair = x;
	checkListLocker.lock();
	pairToContainerMap[x] = temp;
	if (checkList.isEmpty())
	{
		checkList.push_front(temp);
		checkListLocker.unlock();
		return;
	}
	for (auto i = 0;i!=checkList.size();i++)
	{
		if (x->difficultyWall >= checkList[i]->pair->difficultyWall)
		{
			checkList.insert(i, temp);
			checkListLocker.unlock();
			return;
		}
	}
	checkList.push_back(temp);
	checkListLocker.unlock();
}
void Checker::diffChanged(SocketPair* x)
{
	deletePair(x);
	init(x);
}
void Checker::deletePair(SocketPair* x)
{
	checkListLocker.lock();
	pairToContainerMap.erase(pairToContainerMap.find(x));
	for (auto i = checkList.begin(); i != checkList.end(); i++)
	{
		if (x == (*i)->pair)
		{
			delete (*i);
			checkList.erase(i);
			checkListLocker.unlock();
			return;
		}
	}
	checkListLocker.unlock();
}
void Checker::pushKey(SocketPair* x, quint64 key)
{
	pairToContainerMap[x]->locker.lock();
	pairToContainerMap[x]->keyQueue.push_back(key);
	pairToContainerMap[x]->locker.unlock();
}
void Checker::run()
{
	while (true)
	{
		quint64 diffcount = 0;
		quint32 connectionCount = 0;
		if (!checkList.isEmpty())
		{
			checkListLocker.lock();
			for (auto i = checkList.begin(); i != checkList.end(); i++)
			{
				if ((*i)->keyQueue.size() < globalSetting["defaultPassStrategyKeyListLimit"].toInt())
				{
					diffcount += (*i)->pair->difficultyWall;
					connectionCount++;
					if ((*i)->locker.tryLock())//���Լ��������Ѿ���������˵�������߳����ڴ���������һ�����д���
					{
						auto pair = (*i)->pair;
						for (auto j = (*i)->keyQueue.begin(); j != (*i)->keyQueue.end(); j++)
						{
							if (pair->protocolType)//liner
							{
								if (!checkKeyLiner(pair))
								{
									QString log = QString::fromLocal8Bit("ʱ��:") + QTime::currentTime().toString() + QString::fromLocal8Bit(".IP��ַ:") + pair->left->peerAddress().toString() + QString::fromLocal8Bit("�Ͽ���ԭ��:");
									pair->loger->addItem(log + QString::fromLocal8Bit("KeyУ��ʧ�ܣ������ر�"));
									pair->loger->update();
									emit pair->left->disconnected();
									break;
								}
							}
							else//ָ������
							{
								if (!checkKeyExponential(pair))
								{
									//��ⲻͨ��
									QString log = QString::fromLocal8Bit("ʱ��:") + QTime::currentTime().toString() + QString::fromLocal8Bit(".IP��ַ:") + pair->left->peerAddress().toString() + QString::fromLocal8Bit("�Ͽ���ԭ��:");
									pair->loger->addItem(log + QString::fromLocal8Bit("KeyУ��ʧ�ܣ������ر�"));
									pair->loger->update();
									emit pair->left->disconnected();
									break;
								}
							}
						}
						(*i)->locker.unlock();
					}

				}
				else//��⵽�ж���������˵������˷�������
				{
					checkListLocker.unlock();
					(*i)->pair->changeDifficulty(quint32(diffcount / connectionCount));
					break;//��ֹУ�飬���´��Ѷȴ�Ŀ�ʼУ��
				}
			}
			checkListLocker.unlock();
		}
	}
}
bool Checker::checkKeyLiner(SocketPair* x)
{
	auto key = pairToContainerMap[x]->keyQueue.takeFirst();
	QByteArray keyArray((char*)&key, 8);
	x->cache += keyArray;
	x->cache = QCryptographicHash::hash(x->cache, QCryptographicHash::Md5);
	quint64 check = 0;
	memcpy_s((uchar*)&check, 8, x->cache.constData(), 8);
	if (check >= ((0xffffffffffffffff >> 32) * x->difficultyWall))
	{
		return true;
	}
	return false;
}
bool Checker::checkKeyExponential(SocketPair* x)
{
	auto key = pairToContainerMap[x]->keyQueue.takeFirst();
	QByteArray keyArray((char*)&key, 8);
	x->cache += keyArray;
	x->cache = QCryptographicHash::hash(x->cache, QCryptographicHash::Md5);
	quint64 check = 0;
	memcpy_s((uchar*)&check, 8, x->cache.constData(), 8);
	check = check << (64 - x->difficultyWall);
	if ((check & 0xffffffffffffffff) != 0)
	{
		return false;
	}
	return true;
}
