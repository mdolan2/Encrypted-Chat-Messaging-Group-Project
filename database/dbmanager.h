/**
 * @file dbmanager.h
 * @class Database dbmanager.h "server/dbmanager.h"
 * @brief This contains the prototypes for the database manager and all the global variables needed.
 * @author mdolan2
 * @bug No known bugs
 */
 
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <iostream>
#include <QString>
#include <QtSql>
#include <QDebug>
#include <QVector>

class DbManager
{
    public:
		DbManager();
		~DbManager();
		bool isOpen() const;
		void close();
		int sqlSize(QSqlQuery query);
		bool createUserTable();
		bool addUser(const QString& username, const QString& password);
		bool userExists(const QString& inputusername);
		bool checkUserInfo(const QString& username, const QString& password);
		bool createChatTables();
		bool addChat(int chatID, const QString& username, QVector<QString> userVector);
		bool removeChat(int chatID, const QString& username);
		bool chatExists(int chatID);
		QString getChatOwner(int chatID);
		bool doUsersChat(const QString& inputusername1, const QString& inputusername2);
		QVector<QString> getChatUsers(int chatID);
		QVector<int> getChatsUserIsIn(const QString& inputusername);
		QString getUserChatInfo(const QString& inputusername);
	private:
		QSqlDatabase db;
};

#endif	// DBMANAGER_H
