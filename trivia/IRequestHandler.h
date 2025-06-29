#pragma once
#include <iostream>
#include <string.h>
#include <vector>
#include "Requests.h"
#include "Responses.h"
#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"
//inteface for handlers implement
class IRequestHandler
{
public:
	//pure virtual function to check if request is relevant for the curr handler
	virtual bool isRequestRelevant(RequestInfo req) = 0;
	//pure virtual function to handle a client request
	virtual RequestResult handleRequest(RequestInfo req) = 0;
};

