#include "Server.h"
Server::Server(IDataBase* database)
	: m_database(database), m_handlerFactory(m_database, m_communicator), m_communicator(m_handlerFactory)
{
}
void Server::run()
{
	//creating new thread that handles sockets connection and listening
	try
	{
		std::thread t_connector(&Communicator::startHandlerRequest, &m_communicator);
		t_connector.detach();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Server failed to start: " << e.what() << std::endl;

	}
}