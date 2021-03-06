#pragma once
#include"UI/ui_MainWindow.h"
#include<QtWidgets/qmainwindow.h>
#include"NetManager.h"
class MainWindow :public QMainWindow ,public Ui_MainWindow
{
	Q_OBJECT
public:
	NetManager* manager;
	MainWindow();
	QMap<SocketPair*,QTreeWidgetItem*>clientTreeWidgetMap;
	QTreeWidgetItem* clientTreeRoot;
	quint32 id=1;
public slots:
	void recvNewConnectionSlot(SocketPair* x);
	void socketPairDisconnectedSlot(SocketPair* x);
	void bindIpChanged(QString x);
	void bindPortChanged(QString x);
	void retransmissionIpChanged(QString x);
	void retransmissionPortChanged(QString x);
	void saveButtonClicked(bool x);
	void diffLineEditChanged(QString x);
};