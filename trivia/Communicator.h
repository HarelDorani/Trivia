#pragma comment (lib, "ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <mutex>
#include <vector>
#include <exception>
#include <memory>
#include <thread>
#include "RequestHandlerFactory.h"
#include "IRequestHandler.h"
#include "LoginRequestHandler.h"
#include "JsonResponsePacketSerializer.h"
#include "Requests.h"
//class that handles communication with clients
class Communicator
{
public:
	/*
	this is the constractor of the class.
	input: fer to instance of the class that creates handlers
	*/
	Communicator(RequestHandlerFactory& handlerFactory);
	/*
	distracotr
	*/
	~Communicator();
	/*
	this func accept connections
	*/
	void startHandlerRequest();
	/*
	this func is a getter for the vector of pair - socket and handler
	input: none
	output: ref to the vector of clients with their socket and their handler
	*/
	std::vector<std::pair<SOCKET, std::unique_ptr<IRequestHandler>>>& getClientHandlersRef();
	/*
	this func update the handler of a client
	input: the socket of the client to update and the new handler
	output: none
	*/
	void updateHandler(SOCKET socket, IRequestHandler* newHandler);
	/*
	this func find the index of a client in the clients vector by their socket
	input: the client socket
	output: the index of the client in the vector
	*/
	int findClientIndex(SOCKET socket);
	/*
	this func returns the mutex that used for the clients vector 
	input: none
	output: ref to the mutex 
	*/
	std::mutex& getClientsMutex();
private:
	//vector of clients that contains pairs that keep the socket of the client and the handler that used for the client
	std::vector<std::pair<SOCKET, std::unique_ptr<IRequestHandler>>> m_clients;
	//the mutex for the clients vector mutable for const functions
	mutable std::mutex m_clientsMutex;
	SOCKET m_serverSocket;  //the server socket
	/*
	this func create the listen socket and listen for connection
	*/
	void bindAndListent() const;
	/*
	this func handle connection with new clients
	input: the socket
	*/
	void handleNewClient(SOCKET socket);
	int m_port;  //the server port;
	RequestHandlerFactory& m_handlerFactory;  //ref to instance of the class that creates handlers


};