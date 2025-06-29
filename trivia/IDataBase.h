#pragma once
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include "Question.h"
#include "GameData.h"
//interface for database inplement
class IDataBase
{
public:
	//pure virtual function for opening database
	virtual bool open() = 0;
	//pure virtual function for closing database
	virtual bool close() = 0;
	//pure virtual function that checks if user exist in database based on input username
	virtual bool doesUserExist(const std::string& username) = 0;
	//pure virtual function that checks if input password match the input username password
	virtual bool doesPasswordMatch(const std::string& password, const std::string& username) = 0;
	//pure virtual function for adding new user to the database
	virtual bool addNewUser(const std::string& password, const std::string& username, const std::string& email) = 0;
	// func to get avg time for answer
	virtual float getPlayerAvarageAnswerTime(const std::string& username) = 0;
	// func to get num of correct answers of the user
	virtual int getNumOfCorrectAnswers(const std::string& username) = 0;
	// func to get num of total answers of the user
	virtual int gerNumOfTotalAnswers(const std::string& username) = 0;
	// func to get num of games played by the user
	virtual int getNumOfPlayerGames(const std::string& username) = 0;
	// this func calc the score of the player based on all the info in the statistics table
	virtual int getPlayerScore(const std::string& username) = 0;
	// this func return vector of all the scores of the users from highest to lowest
	virtual std::vector<std::string> getHighScores() = 0;
	// this function gets a given num of questions from the db
	virtual std::vector<Question> getQuestions(int numOfQuestions) = 0;
	//  this function submit game data statistics to the game data table
	virtual int SubmitGameStatistics(GameData& data) = 0;
	// this function gets the number of correct answers from the game data table
	virtual unsigned int correctAnswerCount(const std::string& username) = 0;
	// this function gets the number of wrong answers from the game data table
	virtual unsigned int wrongAnswerCount(const std::string& username) = 0;
	// this function gets the avg answer time from the game data table
	virtual float averageAnswerTime(const std::string& username) = 0;
	// this function udates user statistics at the end of a game
	virtual void updateUserStatistics(const GameData& data) = 0;
};