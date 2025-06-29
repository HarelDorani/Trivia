#pragma once
#include <vector>
#include <mutex>
#include <iostream>
#include <string>
#include "Game.h"
#include "Room.h"
#include "IDataBase.h"
//a manager of games
class GameManager
{
public:
    /*
    constractor
    input: pointer to db
    */
    GameManager(IDataBase* database);

    /*
    this function creare a game for players in a room and set the admin in of the game
    input: a pointer to a room, and name of the admin
    output: pointer to a game instance
    */
    std::shared_ptr<Game> createGame(const std::shared_ptr<Room> room, const std::string& admin);

    /*
    this function deletes a game from the games vector by its id
    input: the game id to delete
    output: none
    */
    void deleteGame(unsigned int gameId);

    /*
    this function is a getter to a game by its id
    input: the games id
    output: pointer to a game instance
    */
    std::shared_ptr<Game> getGame(int id);

    /*
    this function is a getter to a game by user in it
    input: user to get his game
    output: pointer to a game instance
    */
    std::shared_ptr<Game> getGameByUser(const LoggedUser& user);
    
    /*
    this function is a getter to the admin of the game
    input: none
    output: the name of the games admin
    */
    std::string getAdmin() const;
private:
    std::string _admin;   //the admin of the game
    mutable std::mutex _mutex;
    IDataBase* _database;   //pointer to the db
    std::vector<std::shared_ptr<Game>> _games;   //vector of all the games
};
