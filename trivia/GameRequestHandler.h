#pragma once
#include "Game.h"
#include "LoggedUser.h"
#include "GameManager.h"
#include "IRequestHandler.h"
#include "RequestHandlerFactory.h"
#include "Requests.h"
#include <iostream>
#include <string>
#include <exception>
//this class handle all games related requests
class GameRequestHandler : public IRequestHandler
{
public:
	/*
	constractor
	input: a game pointer, instance of user, ref to GameManager, ref to RequestHandlerFactory
	*/
	GameRequestHandler(std::shared_ptr<Game> game, LoggedUser user, GameManager& gameManager, RequestHandlerFactory& handlerFactory);
	//virtual function to check if request is relevant for the curr handler
	virtual bool isRequestRelevant(RequestInfo req) override;
	//virtual function to handle a client request
	virtual RequestResult handleRequest(RequestInfo req) override;
private:
	std::shared_ptr<Game> _game;   //the game of the user
	LoggedUser _user;  //the user
	GameManager& _gameManager;  //ref to GameManager class
	RequestHandlerFactory& _handlerFactory;  //ref to RequestHandlerFactory class
	/*
	This function handle the request of getQuestion
	input: struct RequestInfo with request data from the client
	output: struct RequestResult with the response data from the server
	*/
	RequestResult getQuestion(RequestInfo request);
	/*
	This function handle the request of submitAnswer
	input: struct RequestInfo with request data from the client
	output: struct RequestResult with the response data from the server
	*/
	RequestResult submitAnswer(RequestInfo request);
	/*
	This function handle the request of getGameResults
	input: struct RequestInfo with request data from the client
	output: struct RequestResult with the response data from the server
	*/
	RequestResult getGameResults(RequestInfo request);
	/*
	This function handle the request of leaveGame
	input: struct RequestInfo with request data from the client
	output: struct RequestResult with the response data from the server
	*/
	RequestResult leaveGame(RequestInfo request);
};