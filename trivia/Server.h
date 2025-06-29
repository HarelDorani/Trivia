#pragma once
#include "Communicator.h"
#include "IDataBase.h"
#include "RequestHandlerFactory.h"
#include <thread>
#include <iostream>
#include <string>
//class of the server
class Server
{
public:
	//constractor
	Server(IDataBase* database);
	//func creatse the main thread
	void run();
private:
	IDataBase* m_database;
	RequestHandlerFactory m_handlerFactory;
	Communicator m_communicator;
};
