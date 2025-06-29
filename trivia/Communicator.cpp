#include "Communicator.h"
#include "MenuRequestHandler.h"
#include "RoomAdminRequestHandler.h"
Communicator::Communicator(RequestHandlerFactory& handlerFactory) : m_handlerFactory(handlerFactory)
{
	m_port = 2000;  //the server port
	//creating the server socket
	m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	try
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			throw std::exception(__FUNCTION__ " - WSAStartup");
		}
		
		if (m_serverSocket == INVALID_SOCKET)  //checking if socket creation failed
		{
			throw std::exception(__FUNCTION__ " - socket");
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;

	}

}
Communicator::~Communicator()
{
}
void Communicator::startHandlerRequest()
{
	try
	{
		bindAndListent();  //calling func that create the listen socket
		while (true)
		{
			SOCKET client_socket = accept(m_serverSocket, NULL, NULL);  //accepting new client
			if (client_socket == INVALID_SOCKET)
			{
				throw std::exception(__FUNCTION__);
			}
			std::cout << "New client connected!" << std::endl;
			//creating new thread for the new client connected that will handle the client requests
			std::thread clientThread(&Communicator::handleNewClient, this, client_socket); 
			clientThread.detach();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

}
void Communicator::bindAndListent() const
{
	try
	{
		struct sockaddr_in sa = { 0 };

		sa.sin_port = htons(m_port); // port that server will listen for
		sa.sin_family = AF_INET;   // must be AF_INET
		sa.sin_addr.s_addr = INADDR_ANY;    // when there are few ip's for the machine. We will use always "INADDR_ANY"

		// Connects between the socket and the configuration (port and etc..)
		if (bind(m_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
			throw std::exception(__FUNCTION__ " - bind");

		// Start listening for incoming requests of clients
		if (listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR)
			throw std::exception(__FUNCTION__ " - listen");
		std::cout << "Listening on port " << m_port << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;

	}
}

void Communicator::handleNewClient(SOCKET socket)
{
	try
	{
		{
			std::lock_guard<std::mutex> lock(m_clientsMutex);
			int idx = findClientIndex(socket);  //calling the func that finds client index in the clients vector
			if (idx == -1)  //if client doesnt exist in vector
			{
				//adding the new client to the vector with the first handler - login handler
				m_clients.emplace_back(socket, std::make_unique<LoginRequestHandler>(this->m_handlerFactory));
			}
			else
			{
				//if client does exist in vector update the handler to be first handler - login handler
				m_clients[idx].second = std::make_unique<LoginRequestHandler>(this->m_handlerFactory);
			}
		}
		//loop to keep communicating with the connected client
		while (true)
		{
			// rreading the code msg from client (first byte)
			unsigned char code;
			if (recv(socket, (char*)&code, 1, 0) <= 0) break;

			// reading the msg length (4 bytes)
			unsigned char lenBuffer[4];
			if (recv(socket, (char*)lenBuffer, 4, 0) <= 0) break;

			//converting the length to int num
			int size = (lenBuffer[0] << 24) | (lenBuffer[1] << 16) |
				(lenBuffer[2] << 8) | lenBuffer[3];

			// vector of chars that will containg the msg data from client (buffer)
			std::vector<unsigned char> buffer(size);
			//reading the msg data in the length that got from the client
			if (recv(socket, (char*)buffer.data(), size, 0) <= 0) break;

			//creating req info sruct that contains the code, the time of req in seconds, and the data buffer 
			RequestInfo req{ static_cast<int>(code), std::time(nullptr), buffer };

			// getting the hundler of the client
			IRequestHandler* handler = nullptr;
			{
				std::lock_guard<std::mutex> lock(m_clientsMutex);
				int idx = findClientIndex(socket);  //finding index of client in the clients vector
				if (idx != -1)
					handler = m_clients[idx].second.get();  //getting their curr handler
			}

			//checking if the request is relevant in the curr handler state
			if (handler && handler->isRequestRelevant(req))
			{
				//calling func that handle the request and getting the server response
				RequestResult result = handler->handleRequest(req);
				//sending the response to the client
				if (send(socket, (char*)result.response.data(), result.response.size(), 0) == SOCKET_ERROR)
				{
					break;
				}

				//checking if new handler exist
				if (result.newHandler != nullptr)
				{
					//updating the new handler to be the client curr handler in the vector
					std::lock_guard<std::mutex> lock(m_clientsMutex);
					int idx = findClientIndex(socket);
					if (idx != -1)
						m_clients[idx].second.reset(result.newHandler);
				}
			}
			else   //if request isnt valid in the curr handler state
			{
				//creating a new error response struct with the err msg
				ErrorResponse errRes{ "Request not valid in current state." };
				//serializing the err msg
				auto errBuffer = JsonResponsePacketSerializer::serializeResponse(errRes);
				//sending to client
				if (send(socket, (char*)errBuffer.data(), errBuffer.size(), 0) == SOCKET_ERROR)
				{
					break;
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception in client thread: " << e.what() << std::endl;
	}

	// removing client if exited the loop
	{
		std::lock_guard<std::mutex> lock(m_clientsMutex);
		int idx = findClientIndex(socket);  //getting client index in the vec
		if (idx != -1)
			m_clients.erase(m_clients.begin() + idx);  //erasing from the vector
	}
	closesocket(socket);  //closing socket
	std::cout << "Client disconnected." << std::endl;
}

std::vector<std::pair<SOCKET, std::unique_ptr<IRequestHandler>>>& Communicator::getClientHandlersRef()
{
	return m_clients;  //returning the clients vector as ref
}

void Communicator::updateHandler(SOCKET socket, IRequestHandler* newHandler)
{
	int idx = findClientIndex(socket); //finding the clients index
	if (idx != -1)
	{
		m_clients[idx].second.reset(newHandler);  //updating the clients handler to the given handler
	}
	else
	{
		throw std::runtime_error("updateHandler: No handler found for socket");
	}
}

int Communicator::findClientIndex(SOCKET socket)
{
	//looping through the vector
	for (size_t i = 0; i < m_clients.size(); ++i)
	{
		//if curr client socket is equal to curr socket
		if (m_clients[i].first == socket)
			return static_cast<int>(i);
	}
	return -1;  //client wasnt found
}
std::mutex& Communicator::getClientsMutex()
{
	//returning mtx of the clients
	return m_clientsMutex;
}
