#pragma once
#include "IRequestHandler.h"
#include "RequestHandlerFactory.h"
#include "LoggedUser.h"
#include <iostream>
#include "Requests.h"
#include <string>
//class that handle the menu requests
class MenuRequestHandler : public IRequestHandler
{
public:
	/*
	constractor
	input: LoggedUser instance, ref to RequestHandlerFactory class
	*/
	MenuRequestHandler(LoggedUser user, RequestHandlerFactory& factory);
	//virtual function to check if request is relevant for the curr handler
	virtual bool isRequestRelevant(RequestInfo req) override;
	//virtual function to handle a client request
	virtual RequestResult handleRequest(RequestInfo req) override;
private:
	mutable std::mutex mtx;
	RequestHandlerFactory& _handlerFactory;
	LoggedUser _user;
	/*
	This function handle the request of signout
	input: struct RequestInfo with request data from the client
	output: struct RequestResult with the response data from the server
	*/
	RequestResult signout(RequestInfo req);
	/*
	This function handle the request of getRooms
	input: struct RequestInfo with request data from the client
	output: struct RequestResult with the response data from the server
	*/
	RequestResult getRooms(RequestInfo req);
	/*
	This function handle the request of getPlayersInRoom
	input: struct RequestInfo with request data from the client
	output: struct RequestResult with the response data from the server
	*/
	RequestResult getPlayersInRoom(RequestInfo req);
	/*
	This function handle the request of getPersonalStats
	input: struct RequestInfo with request data from the client
	output: struct RequestResult with the response data from the server
	*/
	RequestResult getPersonalStats(RequestInfo req);
	/*
	This function handle the request of getHighScore
	input: struct RequestInfo with request data from the client
	output: struct RequestResult with the response data from the server
	*/
	RequestResult getHighScore(RequestInfo req);
	/*
	This function handle the request of joinRoom
	input: struct RequestInfo with request data from the client
	output: struct RequestResult with the response data from the server
	*/
	RequestResult joinRoom(RequestInfo req);
	/*
	This function handle the request of createRoom
	input: struct RequestInfo with request data from the client
	output: struct RequestResult with the response data from the server
	*/
	RequestResult createRoom(RequestInfo req);
};