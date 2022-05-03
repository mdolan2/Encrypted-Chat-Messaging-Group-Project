#include <QCoreApplication>
#include <QDebug>
#include <dbmanager.h>
#include <iostream>

/**
 * Test main.cpp to exhibit the functionality of dbmanager.cpp
 * Also serves as an example of the proper syntax needed for the database manager functions
 */
 
int main()
{
    {DbManager db;
    if (db.isOpen())
    {
        db.createUserTable();
		
		// Note: if you run this with a pre-existing DB.sqlite file these will return errors because they already exist. This is ok
        db.addUser("Bob", "password1");
        db.addUser("Fred", "password2");
        db.addUser("Harry", "password3");
		db.addUser("Rick", "password4");

		if (db.userExists("Bob"))
		{
			std::cout << "Yes, Bob exists." << std::endl;
		}
		else
		{
			std::cout << "Error, Bob does not exist." << std::endl;
		}
		
		if (db.checkUserInfo("Bob", "password1"))
		{
			// This should print
			std::cout << "Yes, Bob's information is correct. His password is password1" << std::endl;
		}
		else
		{
			// This should not print
			std::cout << "Error, incorrect username and password combination accepted" << std::endl;
		}
		
		if (db.checkUserInfo("Bob", "wrongpassword"))
		{
			// This should not print
			std::cout << "Error, incorrect information for Bob was accepted." << std::endl;
		}
		else
		{
			// This should print
			std::cout << "Success: incorrect username and password combination correctly identified" << std::endl;
		}
		
		// This should print a qDebug statement and the statement below should not print
		if (db.checkUserInfo("Ted", "passwordtest"))
		{
			std::cout << "Error, non-existent user accepted" << std::endl;
		}
		
        qDebug() << "End of user database demo";
		
		db.createChatTables();
		
		// add chats
		QVector<QString> chat1;
		chat1.append("Bob");
		chat1.append("Fred");
		chat1.append("Harry");
		
		QVector<QString> chat2;
		chat2.append("Fred");
		chat2.append("Harry");
		
		// Try creating a chat with a non-existent user
		// Should print an error via qDebug
		db.addChat(1, "Nick", chat1);
		
		// Bob is the owner of chat1
		db.addChat(1, "Bob", chat1);
		
		// Harry is the owner of chat2
		db.addChat(2, "Harry", chat2);
		
		// Check the owner of a chat that does not exist
		// There should be no owner username printed
		QString chat9Owner = db.getChatOwner(9);
		std::cout << "The owner of chat 9 is: ";
		std::cout << chat9Owner.toStdString() << std::endl;
		
		// check that some chats exist and print out their users if so
		if (db.chatExists(1))
		{
			std::cout << "Chat 1 exists" << std::endl;
			QVector<QString> temp = db.getChatUsers(1);
			if (temp.size() > 0)
			{
				std::cout << "Retrieved chat users for chat 1" << std::endl;
			}
			for (int i=0; i<temp.size(); i++)
			{
				QString tempString = temp[i];
				std::cout << "Chat 1 has user: ";
				std::cout << tempString.toStdString() << std::endl;
			}
			
			QString chat1Owner = db.getChatOwner(1);
			std::cout << "The owner of chat 1 is: ";
			std::cout << chat1Owner.toStdString() << std::endl;
		}
		else
		{
			std::cout << "Error, this chat does not exist." << std::endl;
		}
		
		if (db.chatExists(2))
		{
			std::cout << "Chat 2 exists" << std::endl;
			QVector<QString> temp = db.getChatUsers(2);
			if (temp.size() > 0)
			{
				std::cout << "Retrieved chat users for chat 2" << std::endl;
			}
			for (int i=0; i<temp.size(); i++)
			{
				QString tempString = temp[i];
				std::cout << "Chat 2 has user: ";
				std::cout << tempString.toStdString() << std::endl;
			}
			
			QString chat2Owner = db.getChatOwner(2);
			std::cout << "The owner of chat 2 is: ";
			std::cout << chat2Owner.toStdString() << std::endl;
		}
		else
		{
			std::cout << "Error, this chat does not exist." << std::endl;
		}
		
		// Test the user chat information string for Fred
		QString chatInfoFred = db.getUserChatInfo("Fred");
		std::cout << "The chat information for user Fred is:" << std::endl;
		std::cout << chatInfoFred.toStdString() << std::endl;
		QString chatInfoHarry = db.getUserChatInfo("Harry");
		std::cout << "The chat information for user Harry is:" << std::endl;
		std::cout << chatInfoHarry.toStdString() << std::endl;
		
		
		// Show that two users chat
		if (db.doUsersChat("Bob", "Harry"))
		{
			std::cout << "Yes, Bob and Harry chat" << std::endl;
		}
		
		// Show that two user don't chat
		if (db.doUsersChat("Bob", "Rick"))
		{
			std::cout << "Error: Bob and Rick don't chat. Something is wrong" << std::endl;
		}
		else
		{
			std::cout << "It is true that Bob and Rick don't chat" << std::endl;
		}
		
		// Print a list of chats that a particular user is in
		if (db.userExists("Bob"))
		{
			QVector<int> temp = db.getChatsUserIsIn("Bob");
			
			if (temp.size() > 0)
			{
				std::cout << "Retrieved IDs of chats that Bob is in" << std::endl;
			}
			for (int i=0; i<temp.size(); i++)
			{
				QString s = QString::number(temp.at(i));
				std::cout << "Bob is in chat number: ";
				std::cout << s.toStdString() << std::endl;
			}
		}
		
		// Try removing a chat by a user that is not the owner
		// Should print an error via qDebug
		db.removeChat(1, "Harry");
		
		// Delete chat1 using the proper owner
		db.removeChat(1, "Bob");
		
		// prove the chat has been deleted
		if (db.chatExists(1))
		{
			std::cout << "Error, the chat was not successfully deleted." << std::endl;
			
		}
		else
		{
			std::cout << "The chat between Bob, Fred, and Harry has been deleted from chat table." << std::endl;
		}
		
		// Show that with chat 1 deleted, Bob and Harry no longer chat
		if (!db.doUsersChat("Bob", "Harry"))
		{
			std::cout << "Bob and Harry no longer chat" << std::endl;
		}
		else
		{
			std::cout << "Error: Bob and Harry still chat." << std::endl;
		}
		
		QVector<int> testVector = db.getChatsUserIsIn("Bob");
		
		// This should print
		if (testVector.size() == 0)
		{
			qDebug() << "Bob has no more chats";
		}
		else
		{
			qDebug() << "Error: Chat 1 not deleted properly";
		}
		qDebug() << "End of chat database demo";		
    }
    else
    {
        qDebug() << " The database is not open!";
    }
	db.close();
	}
	QSqlDatabase::removeDatabase("DB.sqlite");
	
    return 0;
}