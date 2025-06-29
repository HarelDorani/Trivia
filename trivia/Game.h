#pragma once
#include <vector>
#include <map>
#include <mutex>
#include "Question.h"
#include "LoggedUser.h"
#include "IDataBase.h"
#include "GameData.h"
//class that represents a game
class Game
{
public:
    /*
    constractor
    input: the game id, pointer to the database
    */
    Game(unsigned int gameId, IDataBase* db);

    /*
    This function gets a question for a given user
    input: the user to give a question to
    output: instance of the Question class
    */
    Question getQuestionForUser(const LoggedUser& user);
    
    /*
    This function gets the answer from the user and submit the answer -
    update the game data pf the user by their answer and then update the game data in the db
    input: the user to submit the question
    output: none
    */
    void submitAnswer(const unsigned int answerID, LoggedUser& user);

    /*
    This function remove player from the game
    input: the user to remove
    output: none
    */
    void removePlayer(const LoggedUser& user);

    /*
    this function is a getter to the game id
    input: none
    output: the game id
    */
    unsigned int getGameId();

    /*
    This function sets the questions of the game
    input: vector of questions to set
    output: none
    */
    void setQuestions(const std::vector<Question> questions);

    /*
    this function checks if user exist in the game
    input: the user to check
    output: bool - exist in game or not
    */
    bool hasUser(const LoggedUser& user) const;

    /*
    this function is a getter for the game data of a given user
    input: user to gets his game data
    output: the game data of the user
    */
    GameData& getGameDataForUser(const LoggedUser& user);

    /*
    this function is a getter to the vector of the all players that are in the game
    input: none
    output: the vector of all the players in the game
    */
    std::vector<LoggedUser> getAllPlayers() const;

    /*
    this function gets the number of correct answers a user have from his game data table in the db
    input: the name of the user
    output: num of correct answers
    */
    unsigned int getCorrectAnswerCountFromDB(const std::string& username) const;

    /*
    this function gets the num of wrong answers a user has from his game data table in the db
    input: the name of the user
    output: the num of wrong answers
    */
    unsigned int getWrongAnswerCountFromDB(const std::string& username) const;

    /*
    this function gets the avg time it took a user to answer the questions from his game data table in the db
    input: the name of the user
    output: the avg time
    */
    float getAverageAnswerTimeFromDB(const std::string& username) const;

    /*
    this function adds a player to the game
    input: the user to add
    output: none
    */
    void addPlayer(const LoggedUser& user);

    /*
    this function submit the game data stats to the statistics table in the db
    input: a game data to submit
    outout: none
    */
    void submitGameStatsToDB(const GameData& gameData);
private:
    //map that holds a username as key and the time user got a question from the server
    std::map<std::string, std::chrono::steady_clock::time_point> _questionStartTimes;
    /*
    this function updates the avg answer time it took for a user
    input: the user to update
    output: none
    */
    void updateAnswerTime(const LoggedUser& user);
    mutable std::mutex _mutex;
    std::vector<Question> _questions;  //the questions of the game
    std::map<LoggedUser, GameData> _players;   //the map of players and their game data as values
    unsigned int _gameId;
    IDataBase* _database;  //pointer to db
};
