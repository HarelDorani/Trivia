#include "LoggedUser.h"
LoggedUser::LoggedUser(const std::string& username)
{
	this->_username = username;  //setting the username
}
LoggedUser::~LoggedUser()
{
}
std::string LoggedUser::getUsername() const
{
	return this->_username;  //returning username
}
bool LoggedUser::operator<(const LoggedUser& other) const {
    return _username < other._username;  
}