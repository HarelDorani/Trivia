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
//this class handle all room admin related requests
class RoomAdminRequestHandler : public IRequestHandler
{
public:
	/*
	constractor
	input: the room metadata, the user, ref to roomManager, ref to RequestHandlerFactory, ref to Communicator
	*/
	RoomAdminRequestHandler(RoomData metadata, const LoggedUser& user, RoomManager& roomManager, RequestHandlerFactory& handlerFactory, Communicator& communicator);
	//virtual function to check if request is relevant for the curr handler
	virtual bool isRequestRelevant(RequestInfo req) override;
	//virtual function to handle a client request
	virtual RequestResult handleRequest(RequestInfo req) override;
private:
	mutable std::mutex mtx;
	std::shared_ptr<Room> _room;
	LoggedUser _user;
	RoomManager& _roomManager;
	Communicator& _communicator;  //ref to Communicator
	RequestHandlerFactory& _handlerFactory;
	/*
	This function handle the request of closeRoom
	input: struct RequestInfo with request data from the client
	output: struct RequestResult with the response data from the server
	*/
	RequestResult closeRoom(RequestInfo req);
	/*
	This function handle the request of startGame
	input: struct RequestInfo with request data from the client
	output: struct RequestResult with the response data from the server
	*/
	RequestResult startGame(RequestInfo req);
	/*
	This function handle the request of getRoomState
	input: struct RequestInfo with request data from the client
	output: struct RequestResult with the response data from the server
	*/
	RequestResult getRoomState(RequestInfo req);
};