#pragma once
#include<QtCore/qjsonobject.h>
#include<QtCore/qjsondocument.h>
#include<QtCore/qfile.h>
#include<QtCore/qcryptographichash.h>
#include<QtCore/qdatetime.h>
#include<QtCore/qstring.h>
#include<iostream>
extern QJsonObject globalSetting;
#define STATUS_CODE_SENDDATA 0
#define STATUS_CODE_INIT_LINER 1
#define STATUS_CODE_INIT_EXPONENTIAL 2
#define STATUS_CODE_PING 3
#define STATUS_CODE_PONG 4
#define STATUS_CODE_DIFFICULTY_CHANGE 5
struct POWPHeader
{
	quint8 flag[4] = { 'P','O','W','P' };//POWP
	quint32 difficulty = 0;//DifficultyWall
	quint8 statusCode;
	quint64 key = 0;
	quint64 dataLen = 0;
};

QByteArray getRandomBytes(int nLen);
void loadSetting();
void saveSetting();