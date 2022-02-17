#include"MainWindow.h"

MainWindow::MainWindow()
{
	setupUi(this);
	manager = new NetManager(this);
	connect(manager,&NetManager::recvNewConnectionSignal, this, &MainWindow::recvNewConnectionSlot);
	QStringList temp(QString::fromLocal8Bit("客户端列表:"));
	clientTreeRoot = new QTreeWidgetItem(treeWidget, temp);
	treeWidget->insertTopLevelItem(0,clientTreeRoot);
	clientTreeRoot->setExpanded(true);
	treeWidget->setColumnWidth(0, 170);
}

void MainWindow::recvNewConnectionSlot(SocketPair* x)
{
	connect(x, &SocketPair::iNeedDelete, this, &MainWindow::socketPairDisconnectedSlot);
	QStringList temp(QString("clientId:"));
	temp.push_back(QString::number(id++));
	auto item = new QTreeWidgetItem(clientTreeRoot, temp);
	clientTreeWidgetMap[x] = item;
	//创建子IP节点
	temp = QStringList(QString::fromLocal8Bit("IP地址:"));
	temp.push_back(x->left->peerAddress().toString());
	item->addChild(new QTreeWidgetItem(item, temp));
	//创建子PORT节点
	temp = QStringList(QString::fromLocal8Bit("端口号:"));
	temp.push_back(QString::number(x->left->peerPort()));
	item->addChild(new QTreeWidgetItem(item, temp));
	//创建子难度节点
	temp = QStringList(QString::fromLocal8Bit("难度:"));
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