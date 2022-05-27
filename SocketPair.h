#pragma once
#include"tools.h"
#include<QtNetwork/qtcpserver.h>
#include<QtNetwork/qtcpsocket.h>
#include<QtNetwork/qhostaddress.h>
#include<QtCore/qtimer>
#include <QtWidgets/qlistwidget.h>
#include<QtCore/qqueue.h>
#include<QtCore/qlist.h>
#include<QtCore/qmutex.h>
#include<QtCore/qrunnable.h>
#include<QtCore/qmap.h>
#include<QtCore/qthreadpool.h>
class SocketPair;
struct checkContainer
{
	SocketPair* pair;
	QQueue<quint64> keyQueue;
	QMutex locker;
};
class Checker :public QObject,public QRunnable
{
	Q_OBJECT
protected:
	QList<checkContainer*> checkList;
	QMap<SocketPair*, checkContainer*> pairToContainerMap;
public:
	QMutex checkListLocker;
	quint32 threadCount = 0;
	void start();
	void init(SocketPair* x);
	void diffChanged(SocketPair* x);
	void deletePair(SocketPair* x);
	void pushKey(SocketPair* x, quint64 key);
	void run() override;
	bool checkKeyLiner(SocketPair* x);
	bool checkKeyExponential(SocketPair* x);
};
class SocketPair :public QObject
{
	Q_OBJECT
protected:
	void initPOWP();
	bool checkKeyExponential(quint64 key);
	bool checkKeyLiner(quint64 key);
public:
	bool isDefaultPassStrategy;
	static Checker checker;
	QTcpSocket* left;
	QTcpSocket* right;
	QByteArray cache;
	quint32 difficultyWall;
	bool skipCheck = false;
	bool protocolType;
	QTimer skipCheckTimer;
	QTimer pingPongTimer;
	QTimer pingPongTimeOutTimer;
	QTimer timer;
	bool isPingPongING = false;
	SocketPair(QTcpSocket* Left, quint32 difficulty, bool type, QObject* parent);
	void changeDifficulty(quint32 diff);
	QListWidget* loger;
public slots:
	void someoneDisconnected();
	void leftReadyRead();
	void leftReadReadDefaultPassStrategy();
	void rightReadyRead();
	void skipCheckTimerTimeOut();
	void sendPingPong();
	void pingPongTimerOut();
signals:
	void iNeedDelete(SocketPair* x);
signals:
	void difficultyChanged(SocketPair* x);

};

