/**
 * @file dbmanager.cpp
 * @brief A class containing all required functions that create and access database information
 *
 * The database contains three tables.
 * 1st table called userinfo--usernames and associated passwords.
 * 2nd table called chats--just the chat ID numbers.
 * 3rd table called chatusers--each row contains a chat ID and a username of
 * a user in that chat. 
 *
 * @author Michael Dolan (mdolan5)
 * @bug No known bugs.
 */
 
#include <dbmanager.h>

/**
 * @brief Constructor for the database manager
 * This initialises the private database variable used throughout the class
 * Uses an SQLite database stored on the server
 */

DbManager::DbManager()
{
   db = QSqlDatabase::addDatabase("QSQLITE");
   db.setDatabaseName("DB.sqlite");

   if (!db.open())
   {
      qDebug() << "Error: connection with database failed";
   }
}

/**
 * @brief Destructor for the database manager
 * Checks if the database connection is open, and if so, closes it
 */
DbManager::~DbManager()
{
	if (db.isOpen())
	{
		db.close();
	}
}

/**
 * @brief Checks if the database is currently open
 * @return boolean indicating whether the database is open
 */
bool DbManager::isOpen() const
{
	return db.isOpen();
}

/**
 * @brief Closes the database
 * @return void
 */
void DbManager::close()
{
	db.close();
	return;
}

/**
 * @brief Determines the size of a QSqlQuery
 * Moves from the first query entry to the last and records this index
 * Then returns to the initial position of the query so that it can be searched by other functions
 * @param query This is the QSqlQuery to be assessed
 * @return An integer indicating the size of the query
 */
int DbManager::sqlSize(QSqlQuery query)
{
    int initialPos = query.at();

    int pos = 0;
    if (query.last())
        pos = query.at() + 1;
    else
        pos = 0;

    query.seek(initialPos);
    return pos;
}

/**
 * @brief Creates the database table that stores user information
 * The table userinfo stores a username string and a password string
 * The username is the primary key for the table
 * @return boolean indicating whether the table was successfully created
 */
bool DbManager::createUserTable()
{
	bool success = false;
	
	// Create the userinfo table
	QSqlQuery query(db);
	query.prepare("CREATE TABLE userinfo(username VARCHAR(20) PRIMARY KEY, password VARCHAR(20));");
	
	if (!query.exec())
	{
		qDebug() << "Couldn't create the table 'userinfo': one might already exist.";
	}
	else
	{
		success = true;
	}
	
	return success;
}

/**
 * @brief Adds a user to the userinfo table
 * Checks if the user already exists in the table, and if not runs an SQL query to add them
 * @param username A QString of the username to be entered
 * @param password A QString of the user's password to be entered
 * @return boolean indicating whether the user was successfully added to the table
 */
bool DbManager::addUser(const QString& username, const QString& password)
{
	bool success = false;
	
	// Make sure user doesn't already exist
	if (userExists(username))
	{
		qDebug() << "Error: this user already exists";
	}
	else
	{
		// Add the user to the userinfo table
		QSqlQuery query(db);
		query.prepare("INSERT INTO userinfo VALUES (:username, :password)");
		query.bindValue(":username", username);
		query.bindValue(":password", password);
	
		if (!query.exec())
		{
			qDebug() << "addUser error: " << query.lastError();
		}
		else
		{
			success = true;
		}
	}
	return success;
}

/**
 * @brief Checks whether a username already exists in the userinfo table
 * Runs an SQL query on the userinfo table using the given username and assesses whether the result is empty
 * If the query result is empty then the user does not exist in the table
 * @param inputusername A QString of the username to be checked
 * @return boolean indicating whether the user exists in the table
 */
bool DbManager::userExists(const QString& inputusername)
{
	bool exists = false;
	
	// See if the given username is already in the userinfo table
	QSqlQuery query(db);
	query.prepare("SELECT username FROM userinfo WHERE username = (:inputusername)");
	query.bindValue(":inputusername", inputusername);
	
	if (query.exec())
	{
		// If the user is already in the table the size of the query result will be greater than 0
		if (sqlSize(query) > 0)
		{
			exists = true;
		}
	}
	
	return exists;
}

/**
 * @brief Checks if the given username and password match each other according to the userinfo table
 * Runs a query to find rows in the userinfo table with the given username and password
 * If the query result is empty then the username and password do not go together
 * Used for log-ins
 * @param username The username to be checked
 * @param password The password to be checked
 * @return boolean indicating whether the username and password correspond to the same row in the table
 */
bool DbManager::checkUserInfo(const QString& username, const QString& password)
{
	bool success = false;
	
	// Make sure the user exists first
	if (!userExists(username))
	{
		qDebug() << "Error: this user does not exist";
	}
	else
	{
		// See if the given username and password match a row in the userinfo table
		QSqlQuery query(db);
		query.prepare("SELECT * FROM userinfo WHERE username = (:username) AND password = (:password)");
		query.bindValue(":username", username);
		query.bindValue(":password", password);
	
		if (!query.exec())
		{
			qDebug() << "Chat's users could not be retrieved: " << query.lastError();	
		}
		else if (sqlSize(query) > 0) // The result is not empty, so username and password match a row
		{
			success = true;
		}
	}
	return success;
}

/**
 * @brief Creates the database tables that store chat information and chat users
 * The chats table stores the chat ID number and the username of the chat's owner
 * The chat ID number is the primary key for the chats table
 * The chatusers table stores a row ID number (only used internally within the table), the chat ID number, and the username of one of the user's in the chat
 * The row ID number is the primary key for the chatusers table
 * Each user in a particular chat will be given an individual row in chatusers
 * @return boolean indicating whether the tables were successfully created
 */
bool DbManager::createChatTables()
{
	bool success = false;
	
	// Create the chats table
	QSqlQuery query1(db);
	query1.prepare("CREATE TABLE chats(chatid INTEGER PRIMARY KEY, owner VARCHAR(20) NOT NULL, FOREIGN KEY(owner) references userinfo(username));");
	
	if (!query1.exec())
	{
		qDebug() << "Couldn't create the table 'chats': one might already exist.";
	}
	else
	{
		// Create the chatusers table
		QSqlQuery query2(db);
		query2.prepare("CREATE TABLE chatusers(rowid INTEGER PRIMARY KEY, chatid INTEGER, username VARCHAR(20) NOT NULL, FOREIGN KEY(chatid) references chats(chatid), FOREIGN KEY(username) references userinfo(username));");
	
		if (!query2.exec())
		{
			qDebug() << "Couldn't create the table 'chatusers': one might already exist.";
		}
		else
		{
			success = true;
		}
	}
	
	return success;
}

/**
 * @brief Creates a new chat and adds its information to the appropriate tables
 * @param chatID An integer representing the chat ID number
 * @param username The username of the chat's owner
 * @param userVector A QVector of QStrings containing the usernames of all users to be placed in the chat
 * @return boolean indicating whether the chat was successfully added to the tables
 */
bool DbManager::addChat(int chatID, const QString& username, QVector<QString> userVector)
{
	bool success = false;
	
	// Make sure this chat doesn't already exist
	if (chatExists(chatID))
	{
		qDebug() << "addChat Error: a chat with this ID already exists";
	}
	else
	{
		// Make sure the user who is the owner already exists
		if (!userExists(username))
		{
			qDebug() << "addChat Error: the specified owner user does not exist";
		}
		else
		{
			// Add the chat information to the chats table
			QSqlQuery query(db);
			query.prepare("INSERT INTO chats (chatid, owner) VALUES (:chatID, :username)");
			query.bindValue(":chatID", chatID);
			query.bindValue(":username", username);
	
			if (!query.exec())
			{
				qDebug() << "addChat query error: " << query.lastError();
			}
			else
			{
				// Place all the users from the userVector into the chatusers table for this chat
				for (int i=0; i < userVector.size(); i++)
				{
					QSqlQuery query1(db);
					query1.prepare("INSERT INTO chatusers (chatid, username) VALUES (:chatID, :username)");
					query1.bindValue(":chatID", chatID);
					query1.bindValue(":username", userVector[i]);
	
					if (!query1.exec())
					{
						qDebug() << "addChat query error: " << query1.lastError();
					}
					else
					{
						// If this is the last entry in the userVector then all were added without error
						if (i == (userVector.size() - 1))
						{
							success = true;
						}
					}
				}
			}
		}
		
	}
	return success;
}

/**
 * @brief Removes a chat from the database
 * Deletes all instances of the chat with the given chat ID from the chats and chatusers tables
 * Only the chat owner can delete a chat, so must ensure that the given username is the chat owner
 * @param chatID An integer representing the chat ID number
 * @param username The username of the chat's owner
 * @return boolean indicating whether the chat was successfully removed from the database
 */
bool DbManager::removeChat(int chatID, const QString& username)
{
	bool success = false;
	
	// Make sure that this chat exists
	if (chatExists(chatID))
	{
		QString chatOwner = getChatOwner(chatID);
		
		// Make sure that it is the chat's owner who is calling for the deletion
		if (chatOwner == username)
		{
			// Delete the chat from the chats table
			QSqlQuery queryDelete(db);
			queryDelete.prepare("DELETE FROM chats WHERE chatid = (:chatID)");
			queryDelete.bindValue(":chatID", chatID);
		
			if (!queryDelete.exec())
			{
				qDebug() << "Remove chat failed: " << queryDelete.lastError();
			}
			else
			{
				// Delete all instances of the chat from the chatusers table
				QSqlQuery queryDelete1(db);
				queryDelete1.prepare("DELETE FROM chatusers WHERE chatid = (:chatID)");
				queryDelete1.bindValue(":chatID", chatID);
			
				if (!queryDelete1.exec())
				{
					qDebug() << "Remove chat failed: " << queryDelete.lastError();
				}
				else
				{
					success = true;
				}
			}
		}
		else
		{
			qDebug() << "Remove chat failed: this user is not the chat owner and does not have permission to delete it";
		}
	}
	else
	{
		qDebug() << "Remove chat failed: this chat does not exist";
	}
	
	return success;
}

/**
 * @brief Check if the chat with the given chat ID number exists
 * Runs a query of the chats table and if there is a row with the given chatID, returns true
 * @param chatID An integer representing the chat ID number
 * @return boolean indicating whether the chat exists
 */
bool DbManager::chatExists(int chatID)
{
	bool exists = false;
	
	// See if a chat with the given ID number is in the chats table
	QSqlQuery query(db);
	query.prepare("SELECT * FROM chats WHERE chatid = (:chatID)");
	query.bindValue(":chatID", chatID);
	
	if (query.exec())
	{
		// If there is a chat with the given ID number the query result size will be greater than 0
		if (sqlSize(query) > 0)
		{
			exists = true;
		}
	}
	
	return exists;
}

/**
 * @brief Gets the username of the user that is the owner of the chat with the given ID number
 * Runs a query on the chats table and if there is a row with the given ID number, returns the associated chat owner username
 * @param chatID An integer representing the chat ID number
 * @return QString of the chat owner's username
 */
QString DbManager::getChatOwner(int chatID)
{
	QString chatOwner(QString::null);
	
	// Make sure this chat exists
	if (!chatExists(chatID))
	{
		return chatOwner;
	}
	
	// Retrieve the owner for the chat with the given ID number
	QSqlQuery query(db);
	query.prepare("SELECT owner FROM chats WHERE chatid = (:chatID)");
	query.bindValue(":chatID", chatID);
	
	if (!query.exec())
	{
		qDebug() << "Chat owner could not be retrieved: " << query.lastError();	
	}
	else if (sqlSize(query) > 0)
	{
		while (query.next())
		{
			chatOwner = query.value(0).toString();

		}
	}

	return chatOwner;
}

/**
 * @brief Checks if two given users are in the same chat
 * Runs a query using each username to get all the chats to which each given user belongs
 * @param inputusername1 The firt username to be evaluated
 * @param inputusername2 The second username to be evaluated
 * @return boolean indicating whether the two users are in the same chat
 */
bool DbManager::doUsersChat(const QString& inputusername1, const QString& inputusername2)
{
	// Get the chats that user1 is in
	QSqlQuery query1(db);
	query1.prepare("SELECT chatid FROM chatusers WHERE username = (:inputusername1)");
	query1.bindValue(":inputusername1", inputusername1);
	
	if (query1.exec() && (sqlSize(query1) > 0))
	{
		// Get the chats that user2 is in
		QSqlQuery query2(db);
		query2.prepare("SELECT chatid FROM chatusers WHERE username = (:inputusername2)");
		query2.bindValue(":inputusername2", inputusername2);
		
		if (query2.exec() && (sqlSize(query2) > 0))
		{
			// Iterate through user1's chats
			while (query1.next())
			{
				// Iterate through user2's chats
				while (query2.next())
				{
					// If the chat ID numbers match then the users are in the same chat
					if (query1.value(0).toString() == query2.value(0).toString())
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

/**
 * @brief Gets all the usernames that belong to a chat with the given chat ID number
 * @param chatID An integer representing the chat ID number
 * @return QVector<QString> containing all of the usernames that are in the chat
 */
QVector<QString> DbManager::getChatUsers(int chatID)
{
	// Make sure the chat exists
	if (!chatExists(chatID))
	{
		return QVector<QString>();
	}
	
	// Retrieve the usernames of all users in the chat with the given chatID
	QVector<QString> chatUsersVector;
	QSqlQuery query(db);
	query.prepare("SELECT username FROM chatusers WHERE chatid = (:chatID)");
	query.bindValue(":chatID", chatID);
	
	if (!query.exec())
	{
		qDebug() << "Chat's users could not be retrieved: " << query.lastError();	
	}
	else if (sqlSize(query) > 0)
	{
		while (query.next())
		{
			// Put each username in the QVector<QString>
			QString user = query.value(0).toString();
			chatUsersVector.append(user);
		}
	}
	
	return chatUsersVector;
}

/**
 * @brief Gets all of the chat ID numbers for the chats to which the given user belongs
 * @param inputusername The username for which we are retrieving the chats
 * @return QVector<int> containing all the relevant chat ID numbers
 */
QVector<int> DbManager::getChatsUserIsIn(const QString& inputusername)
{
	// Make sure the user exists
	if (!userExists(inputusername))
	{
		return QVector<int>();
	}
	
	// Retrieve the chat ID numbers for the chats in which the given username appears
	QVector<int> chatsUserIsInVector;
	QSqlQuery query(db);
	query.prepare("SELECT chatid FROM chatusers WHERE username = (:inputusername)");
	query.bindValue(":inputusername", inputusername);
	
	if (!query.exec())
	{
		qDebug() << "Users's chats could not be retrieved: " << query.lastError();
	}
	else if (sqlSize(query) > 0)
	{
		while (query.next())
		{
			// Place the chat ID numbers in the QVector<int>
			int chatNum = query.value(0).toInt();
			chatsUserIsInVector.append(chatNum);
		}
	}
	
	return chatsUserIsInVector;
}

/**
 * @brief Gets all the information about the chats to which the given user belongs
 * Runs queries to gather all of the chats to which the user belongs and all of the other users in those chats
 * Puts the information in the format of chatID followed by a user in that chat
 * For example: 1,userC,3,userB,3,userC,5,userD could be the information for the chats to which userA belongs
 * @param inputusername The username for which we are collecting information
 * @return QString containing all of the relevant information separated by commas
 */
QString DbManager::getUserChatInfo(const QString& inputusername)
{
	QString temp = "";
	// Used to check if this is the first item in the string
	int flag = 0;
	// The chat ID numbers for the chats to which the given user belongs
	QVector<int> chatsUserIsIn = getChatsUserIsIn(inputusername);
	
	// If the user is in any chats
	if (!chatsUserIsIn.empty())
	{
		// An iterator to go through the chat ID numbers
		QVectorIterator<int> itChatIDs(chatsUserIsIn);
		while (itChatIDs.hasNext())
		{
			int tempChatID = itChatIDs.next();
			// The users in the current chat
			QVector<QString> users = getChatUsers(tempChatID);
			// An iterator to go through the chat users
			QVectorIterator<QString> itUsers(users);
			
			while (itUsers.hasNext())
			{
				QString tempUser = itUsers.next();
				if (tempUser != inputusername)
				{
					if (flag == 1)
					{
						// This is not the first item, so put a comma before the text
						temp.append(",");
					}
					
					// Add the chat ID number to the string
					temp.append(QString::number(tempChatID));
					temp.append(",");
					// Add the username to the string
					temp.append(tempUser);
					flag = 1;
				}
			}
		}
	}
	
	return temp;
}