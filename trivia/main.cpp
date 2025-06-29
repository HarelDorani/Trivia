#pragma comment (lib, "ws2_32.lib")
#include "WSAInitializer.h"
#include "Server.h"
#include "SqliteDatabase.h"
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <map>
int main()
{

	WSAInitializer wsaInit;
	std::string input;
	try
	{	
		//setting new db
		SqliteDatabase* database = new SqliteDatabase();
		Server server(database);  //creating instance of the server
		server.run();  //call function that waits for connections
	}
	catch (const std::exception& e)
	{
		std::cout << "Error occured: " << e.what() << std::endl;
	}

	while (true)
	{
		std::getline(std::cin, input);  //getting input from consule
		if (input == "EXIT")  //if user want to exit
		{
			std::cout << "Closing the server, Bye!" << std::endl;
			break;  //exiting loop
		}
	}
	return 0;
}
