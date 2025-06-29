#pragma once
#include <iostream>
#include <string>
#include <mutex>
//class that represents a loggef user
class LoggedUser
{
public:
	//constractor
	LoggedUser(const std::string& username);
	//distractor
	~LoggedUser();
	//getter for the username
	std::string getUsername() const;
	/*
	the < operator function
	input: other ref to instance of Logged user
	output: if curr username is < than other username
	*/
	bool operator<(const LoggedUser& other) const;
private:
	std::string _username;
};