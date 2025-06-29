#pragma once
#include "IDataBase.h"
#include "GameData.h"
#include "sqlite3.h"
#include <exception>
#include <io.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <mutex>
#include <algorithm>
//class that implements the IDatabase with sqlite
class SqliteDatabase : public IDataBase
{
public:
	//constractor
	SqliteDatabase();
	//distractor
	~SqliteDatabase();
	//this func open the db
	virtual bool open() override;
	//this func close the db
	virtual bool close() override;
	/*
	this func check if user exist
	input: username
	output: if user exist or no
	*/
	virtual bool doesUserExist(const std::string& username) override;
	/*
	this func check if a password match the user's password
	input: the password,, the user to check if it matches
	output: if password match or no
	*/
	virtual bool doesPasswordMatch(const std::string& password, const std::string& username) override;
	/*
	this func add new user to the db
	input: password, username, email - all data of new user
	output: if adding worked or not
	*/
	virtual bool addNewUser(const std::string& password, const std::string& username, const std::string& email) override;
	// func to get avg time for answer
	virtual float getPlayerAvarageAnswerTime(const std::string& username) override;
	// func to get num of correct answers of the user
	virtual int getNumOfCorrectAnswers(const std::string& username) override;
	// func to get num of total answers of the user
	virtual int gerNumOfTotalAnswers(const std::string& username) override;
	// func to get num of games played by the user
	virtual int getNumOfPlayerGames(const std::string& username) override;
	// this func calc the score of the player based on all the info in the statistics table
	virtual int getPlayerScore(const std::string& username) override;
	// this func return vector of all the scores of the users from highest to lowest
	virtual std::vector<std::string> getHighScores() override;
	// this func return vector of question in the given size
	virtual std::vector<Question> getQuestions(int numOfQuestions) override;
	//this function updates the game data table with a given game data struct
	virtual int SubmitGameStatistics(GameData& data) override;
	//this functionn returns the num of correct answers of user from the game data table
	virtual unsigned int correctAnswerCount(const std::string& username) override;
	//this functionn returns the num of wtong answers of user from the game data table
	virtual unsigned int wrongAnswerCount(const std::string& username) override;
	//this functionn returns the avg answer time of user from the game data table
	virtual float averageAnswerTime(const std::string& username) override;
	//this func updates the statistics table at the end of a game with a given game data
	virtual void updateUserStatistics(const GameData& data) override;
private:
	mutable std::mutex mtx;
	sqlite3* _db;
	std::string _dbFileName = "triviaDB.sqlite";  //the name of the db
	//this func creates the tables in the db
	bool createTables();
	//this func runs a python script that insert questions to the db
	void insertQuestions();
};