#include "MenuRequestHandler.h"
#include "StatisticsManager.h"
#include "RoomData.h"
#include "LoginManager.h"
#include "LoginRequestHandler.h"
#include "RoomAdminRequestHandler.h"
#include "RoomMemberRequestHandler.h"
MenuRequestHandler::MenuRequestHandler(LoggedUser user, RequestHandlerFactory& factory)
	: _user(user), _handlerFactory(factory)
{
}
bool MenuRequestHandler::isRequestRelevant(RequestInfo req)
{
	//checking if the request code is 1 of the codes that are allowed for the handler
	if (req.id == GET_ROOMS_REQUEST || req.id == GET_PLAYERS_IN_ROOMS_REQUEST
		|| req.id == JOIN_ROOM_REQUEST ||
		req.id == CREATE_ROOM_REQUEST || req.id == GET_HIGH_SCORE_REQUEST
		|| req.id == GET_PERSONAL_STATS_REQUEST || req.id == LOGOUT_REQUEST)
	{
		return true;
	}
	return false;
}
RequestResult MenuRequestHandler::handleRequest(RequestInfo req)
{
	//for each case calling func that handle the request
	switch(req.id)
	{
	case LOGOUT_REQUEST:
		return signout(req);
		break;
	case GET_ROOMS_REQUEST:
		return getRooms(req);
		break;
	case GET_PLAYERS_IN_ROOMS_REQUEST:
		return getPlayersInRoom(req);
		break;
	case JOIN_ROOM_REQUEST:
		return joinRoom(req);
		break;
	case CREATE_ROOM_REQUEST:
		return createRoom(req);
		break;
	case GET_HIGH_SCORE_REQUEST:
		return getHighScore(req);
		break;
	case GET_PERSONAL_STATS_REQUEST:
		return getPersonalStats(req);
		break;
	default:
		ErrorResponse error = { "Invalid request!" };
		//creating ErrorResponse as the RequestResult 
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createMenuRequestHandler(_user) };
		break;
	}
}
RequestResult MenuRequestHandler::signout(RequestInfo req)
{
	RequestResult result;
	LogoutResponse logRes;
	try
	{
		//calling function that handles the logout
		this->_handlerFactory.getLoginManager().logout(this->_user.getUsername());
		logRes.status = SUCCESS;  //setting status to success
		//calling func that serialize reponse of logout request
		result.response = JsonResponsePacketSerializer::serializeResponse(logRes);
		//setting next handler to login handler
		IRequestHandler* newHandler = this->_handlerFactory.createLoginRequestHandler();
		result.newHandler = newHandler;	
		return result;
	}
	catch (const std::exception& e)
	{
		ErrorResponse error;
		error.message = e.what();
		//return seriallized error response
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createMenuRequestHandler(_user) };
	}

}
RequestResult MenuRequestHandler::getRooms(RequestInfo req)
{
	RequestResult result;
	GetRoomsResponse roomsRes;
	try
	{
		//getting the room manager
		RoomManager& roomManager = this->_handlerFactory.getRoomManager();
		roomsRes.rooms = roomManager.getRooms();  //getting all rooms
		roomsRes.status = SUCCESS;  //setting status to success
		//serializing the response
		result.response = JsonResponsePacketSerializer::serializeResponse(roomsRes);
		//setting next handler to curr handler
		result.newHandler = _handlerFactory.createMenuRequestHandler(_user);
		return result;
	}
	catch (const std::exception& e)
	{
		ErrorResponse error;
		error.message = e.what();
		//return seriallized error response
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createMenuRequestHandler(_user) };
	}
}
RequestResult MenuRequestHandler::getPlayersInRoom(RequestInfo req)
{
	RequestResult result;
	try
	{
		//diserializing the request from the client and creating GetPlayersInRoomRequest struct
		GetPlayersInRoomRequest getReq = JsonRequestPacketDeserializer::deserializeGetPlayersRequest(req.buffer);
		GetPlayersInRoomResponse getRes;
		//getting the room manager
		RoomManager& roomManager = this->_handlerFactory.getRoomManager();
		//calling func that gets all players in the room and getting the room by the id from the request from the client
		getRes.players = roomManager.getRoom(getReq.roomId)->getAllUsers();
		//serializing the server response
		result.response = JsonResponsePacketSerializer::serializeResponse(getRes);
		//setting next handler to curr handler
		result.newHandler = _handlerFactory.createMenuRequestHandler(_user);
		return result;
	}
	catch (const std::exception& e)
	{
		ErrorResponse error;
		error.message = e.what();
		//return seriallized error response
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createMenuRequestHandler(_user) };
	}
}
RequestResult MenuRequestHandler::getPersonalStats(RequestInfo req)
{
	RequestResult result;
	try
	{
		GetPersonalStatsResponse getRes;
		//getting the statistics manager
		StatisticsManager manager = this->_handlerFactory.getStatisticsManager();
		//calling func that gets the user statistics
		getRes.statistics = manager.getUserStatistics(this->_user.getUsername());
		getRes.status = SUCCESS;  //setting status to success
		//serializing the server response
		result.response = JsonResponsePacketSerializer::serializeResponse(getRes);
		//setting next handler to curr handler
		result.newHandler = _handlerFactory.createMenuRequestHandler(_user);
		return result;
	}
	catch (const std::exception& e)
	{
		ErrorResponse error;
		error.message = e.what();
		//return seriallized error response
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createMenuRequestHandler(_user) };
	}
}
RequestResult MenuRequestHandler::getHighScore(RequestInfo req)
{
	RequestResult result;
	try
	{
		GetHighScoreResponse getRes;
		//getting the statistics manager
		StatisticsManager manager = this->_handlerFactory.getStatisticsManager();
		//calling func that gets the top 5 high score from the statistics table
		getRes.statistics = manager.getHighScore();
		getRes.status = SUCCESS;  //setting status to success
		//serializing the server response
		result.response = JsonResponsePacketSerializer::serializeResponse(getRes);
		//setting next handler to curr handler
		result.newHandler = _handlerFactory.createMenuRequestHandler(_user);
		return result;
	}
	catch (const std::exception& e)
	{
		ErrorResponse error;
		error.message = e.what();
		//return seriallized error response
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createMenuRequestHandler(_user) };
	}
}
RequestResult MenuRequestHandler::joinRoom(RequestInfo req)
{
	RequestResult result;
	try
	{
		//calling the func that deserialize the request from the clients and creates JoinRoomRequest struct
		JoinRoomRequest joinReq = JsonRequestPacketDeserializer::deserializeJoinRoomRequest(req.buffer);
		JoinRoomResponse joinRes;
		//getting the room manager
		RoomManager& roomManager = this->_handlerFactory.getRoomManager();
		if (roomManager.getRoomState(joinReq.roomId) != false)  //if game already begun cant join the room
		{
			throw std::runtime_error("cant join room: game already begun");
		}
		else
		{
			joinRes.status = SUCCESS;  //setting status to success
			roomManager.getRoom(joinReq.roomId)->addUser(_user);  //adding user to the room
			result.response = JsonResponsePacketSerializer::serializeResponse(joinRes);  //serializing the server response
			//setting next handler as member request handler because not admin
			result.newHandler = _handlerFactory.createRoomMemberRequestHandler(this->_user, roomManager.getRoom(joinReq.roomId));
		}
		return result;
	}
	catch (const std::exception& e)
	{
		ErrorResponse error;
		error.message = e.what();
		//return seriallized error response
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createMenuRequestHandler(_user) };
	}
}
RequestResult MenuRequestHandler::createRoom(RequestInfo req)
{
	RequestResult result;
	try
	{
		//calling func that deserialize the client request and creates CreateRoomRequest struct
		CreateRoomRequest createReq = JsonRequestPacketDeserializer::deserializeCreateRoomRequest(req.buffer);
		CreateRoomResponse createRes;
		//getting the room manager
		RoomManager& roomManager = this->_handlerFactory.getRoomManager();
		int roomID;
		{
			std::lock_guard<std::mutex> lock(mtx);
			roomID = roomManager.getRoomID() + 1;  //getting the last room id from the server and updating by 1
		}
		//create an instance of the room data struct with all the info from the client request and with the roomID
		RoomData roomData = { roomID, createReq.roomName, createReq.maxUsers, createReq.questionCount, createReq.answerTimeout, false };
		//calling func that create a room for the user with the new data
		roomManager.createRoom(this->_user, roomData);  
		createRes.roomID = roomID;  //setting the roomID
		createRes.status = SUCCESS;   //setting status to success
		//serializing the response of the server
		result.response = JsonResponsePacketSerializer::serializeResponse(createRes);
		//setting the next handler to be room admin request handler because user is the admin of the room
		result.newHandler = _handlerFactory.createRoomAdminRequestHandler(this->_user, roomManager.getRoom(roomID));
		return result;
	}
	catch (const std::exception& e)
	{
		ErrorResponse error;
		error.message = e.what();
		//return seriallized error response
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createMenuRequestHandler(_user) };
	}
}