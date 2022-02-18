#include"tools.h"
QJsonObject globalSetting;
void loadSetting()
{
	QFile jsonFile("setting.json");
	if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		throw "setting.json file open fail";
	}
	QString json = jsonFile.readAll();
	jsonFile.close();
	QJsonParseError parseJsonErr;
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8(), &parseJsonErr);
	if (!(parseJsonErr.error == QJsonParseError::NoError))
	{
		throw "setting.json file open fail";
	}
	globalSetting = document.object();
}
void saveSetting()
{
	QJsonDocument doc(globalSetting);
	QByteArray data = doc.toJson();
	QFile file("setting.json");
	file.open(QIODevice::WriteOnly);
	file.write(data);
	file.close();
}

QByteArray getRandomBytes(int nLen)
{
	qsrand(QDateTime::currentMSecsSinceEpoch());
	const char ch[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int size = sizeof(ch);
	char* str = new char[nLen + 1];
	int num = 0;
	for (int nIndex = 0; nIndex < nLen; ++nIndex)
	{
		num = rand() % (size - 1);
		str[nIndex] = ch[num];
	}
	str[nLen] = '\0';
	QByteArray res(str);
	return res;
}