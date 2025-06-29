#pragma once
#include <vector>
#include <iostream>
#include <string>
#include "IDataBase.h"
#include "LoggedUser.h"
//class for managing logins
class LoginManager
{
public:
	//constractor
	LoginManager(IDataBase* db);
	//distractor
	~LoginManager();
	/*
	this function managing signup request
	input: username, password, email. details of a new user
	output: bool - if signup succeded or not
	*/
	bool signup(const std::string& username, const std::string& password, const std::string& email);
	/*
	this function managing login request
	input: the username and the password of the user
	output: bool - if login succeded or not
	*/
	bool login(const std::string& username, const std::string& password);
	/*
	this function is loggin out a user based on username
	*/
	void logout(const std::string& username);;
private:
	mutable std::mutex mtx;
	IDataBase* _database;  //pointer to the db
	std::vector<LoggedUser> _loggedUsers;  //vector of all the logged users
	//this func check if user is logged in based on username
	bool isUserLogged(const std::string& username);
};