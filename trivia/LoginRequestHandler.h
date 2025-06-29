#pragma once
#include <iostream>
#include <string.h>
#include "LoginManager.h"
#include "Requests.h"
#include "LoggedUser.h"
#include "IRequestHandler.h"
#include "RequestHandlerFactory.h";
#define SUCCESS 1
#define FAIL 0
//class that implement IRequestHandler interface for login handler
class LoginRequestHandler : public IRequestHandler
{
public:
	//constractor
	LoginRequestHandler(RequestHandlerFactory& handlerFactory);
	//this func checks if the request is relevant for login/signup
	virtual bool isRequestRelevant(RequestInfo req) override;
	//this func handle the request and send the result
	virtual RequestResult handleRequest(RequestInfo req) override;
private:
	RequestHandlerFactory& _handlerFactory;  //instance of the class that create handlers
	//this func build the result of login request based on the req info
	RequestResult login(RequestInfo req);   
	//this func build the result of signup request based on the req info
	RequestResult signup(RequestInfo req);
};