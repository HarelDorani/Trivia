#pragma once
#include <vector>
#include <iostream>
#include <string>
#include "IDataBase.h"
#include "LoginManager.h" 
#include "RoomManager.h"
#include "StatisticsManager.h"
#include "LoggedUser.h"
#include "GameManager.h"
// first defenitions of classes to prevent circular dependency
class MenuRequestHandler;
class LoginRequestHandler;
class RoomAdminRequestHandler;
class RoomMemberRequestHandler;
class Communicator;
class GameRequestHandler;

//this class creates all types of handler and stores all managers
class RequestHandlerFactory
{
public:
    /*
    constractor
    input: pointer to db, ref to the communicator
    */
    RequestHandlerFactory(IDataBase* db, Communicator& communicator); 
    ~RequestHandlerFactory();
    /*
    this func create a LoginRequestHandler
    output: pointer to instance of LoginRequestHandler class
    */
    LoginRequestHandler* createLoginRequestHandler();
    //this func returns a ref to LoginManager instance
    LoginManager& getLoginManager(); 
    /*
    this func create a MenuRequestHandler
    input: the user
    output: pointer to instance of LoginRequestHandler class
    */
    MenuRequestHandler* createMenuRequestHandler(LoggedUser user);
    //this func returns a ref to StatisticsManager instance
    StatisticsManager& getStatisticsManager();
    //this func returns a ref to RoomManager instance
    RoomManager& getRoomManager();
    //this func returns a ref to GameManager instance
    GameManager& getGameManager();
    /*
    this func create a RoomAdminRequestHandler
    input: the user, pointer to room
    output: pointer to instance of LoginRequestHandler class
    */
    RoomAdminRequestHandler* createRoomAdminRequestHandler(const LoggedUser& user, std::shared_ptr<Room> room);
    /*
    this func create a RoomMemberRequestHandler
    input: the user, pointer to room
    output: pointer to instance of LoginRequestHandler class
    */
    RoomMemberRequestHandler* createRoomMemberRequestHandler(const LoggedUser& user, std::shared_ptr<Room> room);
    /*
    this func create a GameRequestHandler
    input: the user
    output: pointer to instance of LoginRequestHandler class
    */
    GameRequestHandler* createGameRequestHandler(const LoggedUser& user);
private:
    Communicator& _communicator;  //ref to Communicator class
    LoginManager _loginManager;  //instance of LoginManager
    IDataBase* _database;   //pointer to db
    RoomManager _roomManager;  
    StatisticsManager _statisticsManager;
    GameManager _gameManager;
};
