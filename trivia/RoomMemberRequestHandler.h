#pragma once
#pragma once
#include <iostream>
#include <string>
#include <mutex>
#include <exception>
#include "IRequestHandler.h"
#include "Requests.h"
#include "Room.h"
#include "LoggedUser.h"
#include "RoomManager.h"
#include "JsonResponsePacketSerializer.h"
#include "RequestHandlerFactory.h"
//this class handle all room member related requests
class RoomMemberRequestHandler : public IRequestHandler
{
public:
	/*
	constractor
	input: the room metadata, the user, ref to roomManager, ref to RequestHandlerFactory, ref to Communicator
	*/
	RoomMemberRequestHandler(RoomData metadata, const LoggedUser& user, RoomManager& roomManager, RequestHandlerFactory& handlerFactory, Communicator& communicator);
	//virtual function to check if request is relevant for the curr handler
	virtual bool isRequestRelevant(RequestInfo req) override;
	//virtual function to handle a client request
	virtual RequestResult handleRequest(RequestInfo req) override;
	//getter to the room id
	unsigned int getRoomID();
	//getter to the username
	std::string getUsername();
private:
	std::shared_ptr<Room> _room;  //the curr room
	LoggedUser _user;  //the user
	RoomManager& _roomManager;
	Communicator& _communicator;
	RequestHandlerFactory& _handlerFactory;
	/*
	This function handle the request of leaveRoom
	input: struct RequestInfo with request data from the client
	output: struct RequestResult with the response data from the server
	*/
	RequestResult leaveRoom(RequestInfo req);
	/*
	This function handle the request of getRoomState
	input: struct RequestInfo with request data from the client
	output: struct RequestResult with the response data from the server
	*/
	RequestResult getRoomState(RequestInfo req);
};