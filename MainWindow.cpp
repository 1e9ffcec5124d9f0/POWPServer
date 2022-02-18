#include"MainWindow.h"

MainWindow::MainWindow()
{
	loadSetting();
	setupUi(this);
	manager = new NetManager(this);
	QStringList temp(QString::fromLocal8Bit("�ͻ����б�:"));
	clientTreeRoot = new QTreeWidgetItem(treeWidget, temp);
	treeWidget->insertTopLevelItem(0,clientTreeRoot);
	clientTreeRoot->setExpanded(true);
	treeWidget->setColumnWidth(0, 170);

	//�����ź�
	connect(manager, &NetManager::recvNewConnectionSignal, this, &MainWindow::recvNewConnectionSlot);
	connect(bindIpLineEdit, &QLineEdit::textChanged, this, &MainWindow::bindIpChanged);
	connect(bindPortLineEdit, &QLineEdit::textChanged, this, &MainWindow::bindPortChanged);
	connect(retransIpLineEdit, &QLineEdit::textChanged, this, &MainWindow::retransmissionIpChanged);
	connect(retransPortLineEdit, &QLineEdit::textChanged, this, &MainWindow::retransmissionPortChanged);
	connect(saveSettingPushButton, &QPushButton::clicked, this, &MainWindow::saveButtonClicked);
	connect(diffLineEdit, &QLineEdit::textChanged, this, &MainWindow::diffLineEditChanged);

	//��д���õ�UI
	bindIpLineEdit->setText(globalSetting["bindIp"].toString());
	bindPortLineEdit->setText(QString::number(globalSetting["bindPort"].toInt()));
	retransIpLineEdit->setText(globalSetting["retransmissionIp"].toString());
	retransPortLineEdit->setText(QString::number(globalSetting["retransmissionPort"].toInt()));
	diffLineEdit->setText(globalSetting["globalDifficultyWall"].toString());
}

void MainWindow::recvNewConnectionSlot(SocketPair* x)
{
	connect(x, &SocketPair::iNeedDelete, this, &MainWindow::socketPairDisconnectedSlot);
	QStringList temp(QString("clientId:"));
	temp.push_back(QString::number(id++));
	auto item = new QTreeWidgetItem(clientTreeRoot, temp);
	clientTreeWidgetMap[x] = item;
	//������IP�ڵ�
	temp = QStringList(QString::fromLocal8Bit("IP��ַ:"));
	temp.push_back(x->left->peerAddress().toString());
	item->addChild(new QTreeWidgetItem(item, temp));
	//������PORT�ڵ�
	temp = QStringList(QString::fromLocal8Bit("�˿ں�:"));
	temp.push_back(QString::number(x->left->peerPort()));
	item->addChild(new QTreeWidgetItem(item, temp));
	//�������ѶȽڵ�
	temp = QStringList(QString::fromLocal8Bit("�Ѷ�:"));
	temp.push_back(QString::number(x->difficultyWall));
	item->addChild(new QTreeWidgetItem(item, temp));
	
	clientTreeRoot->addChild(item);
	treeWidget->update();
}
void MainWindow::socketPairDisconnectedSlot(SocketPair* x)
{
	clientTreeRoot->removeChild(clientTreeWidgetMap[x]);
	treeWidget->update();
	clientTreeWidgetMap.take(x);
}

void MainWindow::bindIpChanged(QString x)
{
	globalSetting["bindIp"] = x;
}
void MainWindow::bindPortChanged(QString x)
{
	auto temp = x.toInt();
	if (temp == 0)return;
	globalSetting["bindPort"] = temp;
	
}
void MainWindow::retransmissionIpChanged(QString x)
{
	globalSetting["retransmissionIp"] = x;
}
void MainWindow::retransmissionPortChanged(QString x)
{
	auto temp = x.toInt();
	if (temp == 0)return;
	globalSetting["retransmissionPort"] = temp;
}
void MainWindow::saveButtonClicked(bool x)
{
	saveSetting();
}

void MainWindow::diffLineEditChanged(QString x)
{
	globalSetting["globalDifficultyWall"] = x;
}
