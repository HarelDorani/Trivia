#include "LoginRequestHandler.h"
#include "MenuRequestHandler.h"
LoginRequestHandler::LoginRequestHandler(RequestHandlerFactory& handlerFactory) : _handlerFactory(handlerFactory)
{
}
bool LoginRequestHandler::isRequestRelevant(RequestInfo req)
{
	//checking if the req id is 1 of the relevant ones
	if (req.id == LOGIN_REQUEST || req.id == SIGNUP_REQUEST)
	{
		return true;
	}
	return false;
}
RequestResult LoginRequestHandler::handleRequest(RequestInfo req)
{
	RequestResult result;
	if (req.id == LOGIN_REQUEST)  //for login req
	{
		return login(req);  //calling func that creates RequestResult for login
	}
	else if (req.id == SIGNUP_REQUEST)   //for signup req
	{
		return signup(req);  //calling func that creates RequestResult for signup
	}
	else
	{
		ErrorResponse error = { "Invalid request!" };
		//creating ErrorResponse as the RequestResult 
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createLoginRequestHandler()};
	}
}
RequestResult LoginRequestHandler::login(RequestInfo req)
{
	RequestResult result;
	try
	{
		//calling func that deserialize the buffer from client
		LoginRequest logReq = JsonRequestPacketDeserializer::deserializeLoginRequest(req.buffer);
		LoginResponse logRes;
		//getting instance of LoginManager class from the class that creates handlers
		LoginManager& manager = this->_handlerFactory.getLoginManager();
		if (manager.login(logReq.username, logReq.password))   //checking if login request is valid
		{
			logRes.status = SUCCESS;		//setting result status to success
			//serializing the response buffer base on the result status
			result.response = JsonResponsePacketSerializer::serializeResponse(logRes);
			//setting next handler as MenuRequestHandler
			LoggedUser user = LoggedUser(logReq.username);
			IRequestHandler* newHandler = this->_handlerFactory.createMenuRequestHandler(user);
			result.newHandler = newHandler;
		}
		else  //if login didnt succeded
		{
			//setting status to fail
			logRes.status = FAIL;
			//serializing error response
			result.response = JsonResponsePacketSerializer::serializeResponse(logRes);
			result.newHandler = _handlerFactory.createLoginRequestHandler();
		}

		return result;   //returning the RequestResult
	}
	catch (const std::exception& e)   //catching exceptions
	{
		//creating error response with the exception cought
		ErrorResponse error;
		error.message = e.what();
		//return seriallized error response
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createLoginRequestHandler() };
	}

}
RequestResult LoginRequestHandler::signup(RequestInfo req)
{
	try
	{
		RequestResult result;
		//deserializing the signup request with the buffer from client
		SignupRequest signupReq = JsonRequestPacketDeserializer::deserializeSignupRequest(req.buffer);
		SignupResponse signupRes;
		//getting instance of LoginManager
		LoginManager& manager = this->_handlerFactory.getLoginManager();
		if (manager.signup(signupReq.username, signupReq.password, signupReq.email))  //checking if signup succeded
		{
			//setting status to success
			signupRes.status = SUCCESS;
			//getting serialized response
			result.response = JsonResponsePacketSerializer::serializeResponse(signupRes);
			LoggedUser user = LoggedUser(signupReq.username);
			IRequestHandler* newHandler = this->_handlerFactory.createMenuRequestHandler(user);
			result.newHandler = newHandler;
		}
		else
		{
			//setting status as fail
			signupRes.status = FAIL;
			//getting serialized buffer for the server response
			result.response = JsonResponsePacketSerializer::serializeResponse(signupRes);
			result.newHandler = _handlerFactory.createLoginRequestHandler();
		}

		return result;  //returning the RequestResult
	}
	catch (const std::exception& e)  //catching exception
	{
		//creating error response with the exception cought
		ErrorResponse error;
		error.message = e.what();
		//returning RequestResult of the serialized error response
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createLoginRequestHandler() };
	}

}

