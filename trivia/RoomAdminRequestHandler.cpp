#include "RoomAdminRequestHandler.h"
#include "Communicator.h"
#include "MenuRequestHandler.h"
#include "GameRequestHandler.h"
#include "GameManager.h"
#include "RoomMemberRequestHandler.h"
RoomAdminRequestHandler::RoomAdminRequestHandler(RoomData metadata, const LoggedUser& user,
	RoomManager& roomManager,
	RequestHandlerFactory& handlerFactory,
	Communicator& communicator)
	: _room(roomManager.getRoom(metadata.id)), //getting the room by its id
	_user(user),
	_roomManager(roomManager),
	_handlerFactory(handlerFactory),
	_communicator(communicator)
{
}
bool RoomAdminRequestHandler::isRequestRelevant(RequestInfo req)
{
	//checking if the request code is 1 of the codes that are allowed for the handler
	if (req.id ==  CLOSE_ROOM_REQUEST
		|| req.id == START_GAME_REQUEST
		|| req.id ==  GET_ROOM_STATE_REQUEST)
	{
		return true;
	}
	return false;
}
RequestResult RoomAdminRequestHandler::handleRequest(RequestInfo req)
{
	//for each case calling func that handle the request
	switch (req.id)
	{
	case CLOSE_ROOM_REQUEST:
		return closeRoom(req);
		break;
	case START_GAME_REQUEST:
		return startGame(req);
		break;
	case GET_ROOM_STATE_REQUEST:
		return getRoomState(req);
		break;
	default:
		ErrorResponse error = { "Invalid request!" };
		//creating ErrorResponse as the RequestResult 
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createRoomAdminRequestHandler(_user, _roomManager.getRoom(_room->getRoomData().id)) };
		break;
	}
}
RequestResult RoomAdminRequestHandler::closeRoom(RequestInfo req)
{
	RequestResult result;
	//initializing the structs with status success
	LeaveRoomResponse leaveRes{ SUCCESS };
	CloseRoomResponse closeRes{ SUCCESS };

	try
	{
		//serializing reponse for leave room
		auto buffer = JsonResponsePacketSerializer::serializeResponse(leaveRes);
		/*
		calling func that gets ref to the clients vector
		pair: first element - socket, second - the clients handler
		*/
		auto& clientsVec = _communicator.getClientHandlersRef();

		{
			std::lock_guard<std::mutex> lock(_communicator.getClientsMutex());

			for (auto& pair : clientsVec)  //looppin through all clients
			{
				SOCKET sock = pair.first;  //getting the socket
				//getting the handler of the client
				std::unique_ptr<IRequestHandler>& handlerPtr = pair.second;

				if (!handlerPtr)  //skeeping client if hundler null
					continue;
				//trying to cast the clients handler to RoomMemberRequestHandler
				auto* memberHandler = dynamic_cast<RoomMemberRequestHandler*>(handlerPtr.get());
				//checking if casting succeded and the client is in the curr room
				if (memberHandler && memberHandler->getRoomID() == _room->getRoomData().id)
				{
					//if the client is not the curr user
					if (memberHandler->getUsername() != _user.getUsername())
					{
						//sending leave room response to the client
						send(sock, reinterpret_cast<const char*>(buffer.data()), buffer.size(), 0);
					}
					//creating a LoggedUser with the clients username
					LoggedUser targetUser(memberHandler->getUsername());
					//creating new menuRequestHandler for the client
					IRequestHandler* newMenu = _handlerFactory.createMenuRequestHandler(targetUser);
					//calling func that will update the client handler by the socket
					_communicator.updateHandler(sock, newMenu);
				}
			}
		}
		//deleting the room by its id
		_roomManager.deleteRoom(_room->getRoomData().id);
		//serializing the server response for close room
		result.response = JsonResponsePacketSerializer::serializeResponse(closeRes);
		//setting next handler for the user as menuRequestHandler
		result.newHandler = _handlerFactory.createMenuRequestHandler(_user);
		return result;
	}
	catch (const std::exception& e)
	{
		ErrorResponse error{ e.what() };
		return { JsonResponsePacketSerializer::serializeResponse(error),
				 _handlerFactory.createRoomAdminRequestHandler(_user, _roomManager.getRoom(_room->getRoomData().id)) };
	}
}

RequestResult RoomAdminRequestHandler::startGame(RequestInfo req)
{
	RequestResult result;
	//setting status as success
	StartGameResponse gameRes{ SUCCESS };
	try
	{
		//creating a game for the players in the room
		_handlerFactory.getGameManager().createGame(_room, this->_user.getUsername());
		auto playersInRoom = _room->getAllUsers();  //getting all the players in the room
		//serializing the server response for start game
		auto buffer = JsonResponsePacketSerializer::serializeResponse(gameRes);

		{
			std::lock_guard<std::mutex> lock(_communicator.getClientsMutex());
			//getting the clients vector
			auto& clientsVec = _communicator.getClientHandlersRef();

			for (auto& pair : clientsVec)  //loopping through each client
			{
				SOCKET sock = pair.first;  //getting the client socket
				//getting client handler
				std::unique_ptr<IRequestHandler>& handler = pair.second;
				//if handler is null skeeping the client
				if (!handler)
					continue;
				//casting the handler as RoomMemberRequestHandler
				auto* memberHandler = dynamic_cast<RoomMemberRequestHandler*>(handler.get());
				//if casting succeeded and client is in the curr room
				if (memberHandler && memberHandler->getRoomID() == _room->getRoomData().id)
				{
					//and client is not the curr user
					if (memberHandler->getUsername() != _user.getUsername())
					{
						//sending start game msg to the client by his socket
						send(sock, reinterpret_cast<const char*>(buffer.data()), buffer.size(), 0);
					}
					//creating LoggedUser instance with the client username
					LoggedUser targetUser(memberHandler->getUsername());
					//setting the client's next handler as gameRequestHandler
					IRequestHandler* newGameHandler = _handlerFactory.createGameRequestHandler(targetUser);
					//updating the clients handler in the clients vector in the communicator
					_communicator.updateHandler(sock, newGameHandler);
				}
			}
		}

		result.response = buffer;  //setting response as the start game buffer
		//setting the user's new handler as gameRequestHandler
		result.newHandler = _handlerFactory.createGameRequestHandler(_user);
		return result;
	}
	catch (const std::exception& e)
	{
		ErrorResponse error{ e.what() };
		return { JsonResponsePacketSerializer::serializeResponse(error),
				 _handlerFactory.createRoomAdminRequestHandler(_user, _roomManager.getRoom(_room->getRoomData().id)) };
	}
}

RequestResult RoomAdminRequestHandler::getRoomState(RequestInfo req)
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
		result.newHandler = _handlerFactory.createRoomAdminRequestHandler(_user, _roomManager.getRoom(_room->getRoomData().id));
		return result;
	}
	catch (const std::exception& e)
	{
		ErrorResponse error;
		error.message = e.what();
		//return seriallized error response
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createRoomAdminRequestHandler(_user, _roomManager.getRoom(_room->getRoomData().id)) };
	}
}
