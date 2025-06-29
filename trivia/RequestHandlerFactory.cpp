#include "RequestHandlerFactory.h"
#include "LoginRequestHandler.h"
#include "MenuRequestHandler.h"
#include "RoomAdminRequestHandler.h"
#include "RoomMemberRequestHandler.h"
#include "GameRequestHandler.h"
#include "Communicator.h"
RequestHandlerFactory::RequestHandlerFactory(IDataBase* db, Communicator& communicator) 
    : _database(db), _loginManager(db) , _statisticsManager(db), _communicator(communicator), _gameManager(db){}
RequestHandlerFactory::~RequestHandlerFactory()
{
}


LoginRequestHandler* RequestHandlerFactory::createLoginRequestHandler()
{
    return new LoginRequestHandler(*this);  //returning pointer to LoginRequestHandler
}

LoginManager& RequestHandlerFactory::getLoginManager()
{
    return _loginManager;  //returnnig ref to instance of LoginManager
}
MenuRequestHandler* RequestHandlerFactory::createMenuRequestHandler(LoggedUser user)
{
    return new MenuRequestHandler(user, *this);  //returning pointer to MenuRequestHandler
}
StatisticsManager& RequestHandlerFactory::getStatisticsManager()
{
    return _statisticsManager;   //returnnig ref to instance of StatisticsManager
}
RoomManager& RequestHandlerFactory::getRoomManager()
{
    return _roomManager;   //returnnig ref to instance of RoomManager
}
RoomAdminRequestHandler* RequestHandlerFactory::createRoomAdminRequestHandler(const LoggedUser& user, std::shared_ptr<Room> room)
{
    //returning pointer to RoomAdminRequestHandler
    return new RoomAdminRequestHandler(room->getRoomData(), user, this->_roomManager, *this, _communicator);
}

RoomMemberRequestHandler* RequestHandlerFactory::createRoomMemberRequestHandler(const LoggedUser& user, std::shared_ptr<Room> room)
{
    //returning pointer to RoomMemberRequestHandler
    return new RoomMemberRequestHandler(room->getRoomData(), user, this->_roomManager, *this, _communicator);
}

GameManager& RequestHandlerFactory::getGameManager()
{
    return _gameManager;  //returnnig ref to instance of GameManager
}
GameRequestHandler* RequestHandlerFactory::createGameRequestHandler(const LoggedUser& user)
{
    //getting the game of the user
    std::shared_ptr<Game> game = _gameManager.getGameByUser(user);
    //returning pointer to GameRequestHandler
    return new GameRequestHandler(game, user, _gameManager, *this);
}