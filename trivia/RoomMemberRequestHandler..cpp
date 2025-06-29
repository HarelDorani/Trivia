#include "RoomMemberRequestHandler.h"
#include "MenuRequestHandler.h"
RoomMemberRequestHandler::RoomMemberRequestHandler(RoomData metadata, const LoggedUser& user,
	RoomManager& roomManager,
	RequestHandlerFactory& handlerFactory,
	Communicator& communicator)
	: _room(roomManager.getRoom(metadata.id)),  //getting the room by its id
	_user(user),
	_roomManager(roomManager),
	_handlerFactory(handlerFactory),
	_communicator(communicator)
{
}

bool RoomMemberRequestHandler::isRequestRelevant(RequestInfo req)
{
	//checking if the request code is 1 of the codes that are allowed for the handler
	if (req.id == LEAVE_ROOM_REQUEST
		|| req.id == GET_ROOM_STATE_REQUEST)
	{
		return true;
	}
	return false;
}
RequestResult RoomMemberRequestHandler::handleRequest(RequestInfo req)
{
	//for each case calling func that handle the request
	switch (req.id)
	{
	case LEAVE_ROOM_REQUEST:
		return leaveRoom(req);
		break;
	case GET_ROOM_STATE_REQUEST:
		return getRoomState(req);
		break;
	default:
		ErrorResponse error = { "Invalid request!" };
		//creating ErrorResponse as the RequestResult 
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createRoomMemberRequestHandler(_user, _roomManager.getRoom(_room->getRoomData().id)) };
		break;
	}
}
RequestResult RoomMemberRequestHandler::leaveRoom(RequestInfo req)
{
	RequestResult result;
	LeaveRoomResponse leaveRes;
	try
	{
		_room->removeUser(_user);  //removing user from the room
		leaveRes.status = SUCCESS;  //setting status as success
		//serializing the server response for leave room
		result.response = JsonResponsePacketSerializer::serializeResponse(leaveRes);
		//setting next handler as MenuRequestHandler
		result.newHandler = _handlerFactory.createMenuRequestHandler(_user);
		return result;
	}
	catch (const std::exception& e)
	{
		ErrorResponse error;
		error.message = e.what();
		//return seriallized error response
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createRoomMemberRequestHandler(_user, _roomManager.getRoom(_room->getRoomData().id)) };
	}
	return result;
}
RequestResult RoomMemberRequestHandler::getRoomState(RequestInfo req)
{
	RequestResult result;
	GetRoomStateResponse getRes;
	try
	{
		//getting the time per question
		getRes.answerTimeout = this->_room->getRoomData().timePerQuestion;
		//getting room status - game begun or not
		getRes.hasGameBegun = this->_room->getRoomData().status;
		//getting all players in the room
		getRes.players = this->_room->getAllUsers();
		//getting the number of questions
		getRes.questionsCount = this->_room->getRoomData().numOfQuestionsInGame;
		//setting status to success
		getRes.status = SUCCESS;
		//serializing the server response
		result.response = JsonResponsePacketSerializer::serializeResponse(getRes);
		//setting next handler as curr handler
		result.newHandler = _handlerFactory.createRoomMemberRequestHandler(_user, _roomManager.getRoom(_room->getRoomData().id));
		return result;
	}
	catch (const std::exception& e)
	{
		ErrorResponse error;
		error.message = e.what();
		//return seriallized error response
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createRoomMemberRequestHandler(_user, _roomManager.getRoom(_room->getRoomData().id)) };
	}
}
unsigned int RoomMemberRequestHandler::getRoomID()
{
	return this->_room->getRoomData().id;  //getter to the room id
}
std::string RoomMemberRequestHandler::getUsername()
{
	return this->_user.getUsername();  //getter to the username
}