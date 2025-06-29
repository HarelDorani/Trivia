#include "LoginManager.h"
LoginManager::LoginManager(IDataBase* db) : _database(db)
{
}
LoginManager::~LoginManager()
{
}
bool LoginManager::signup(const std::string& username, const std::string& password, const std::string& email)
{
	if (this->_database->doesUserExist(username)) //checking if user exist in the db
	{
		throw std::runtime_error("User already exist! login instead.");
	}
	else
	{
		//if not adding new user to db
		if(this->_database->addNewUser(password, username, email))
		{
			this->_loggedUsers.push_back(LoggedUser(username));
			return true;
		}
		return false;
	}
}
bool LoginManager::login(const std::string& username, const std::string& password)
{
	//checking if the user exist in the database
	if (!(this->_database->doesUserExist(username)))
	{
		throw std::runtime_error("User doesnt exist!");
	}
	//checking if user is already logged
	else if (isUserLogged(username))
	{
		throw std::runtime_error("User Already logged!");
	}
	//checking if the password match the password in the db
	else if (!(this->_database->doesPasswordMatch(password, username)))
	{
		throw std::runtime_error("Wrong password!");
	}
	else
	{
		//adding the user to logged users
		std::lock_guard<std::mutex> lock(mtx);
		this->_loggedUsers.push_back(LoggedUser(username));
		return true;
	}
}
void LoginManager::logout(const std::string& username)
{
	std::lock_guard<std::mutex> lock(mtx);
	std::vector<LoggedUser> temp;  //creating temp vector of logged users
	//adding each user except of curr user
	for (const auto& user : _loggedUsers)
	{
		if (user.getUsername() != username)
			temp.push_back(user);
	}
	_loggedUsers = std::move(temp);
}
bool LoginManager::isUserLogged(const std::string& username)
{
	std::lock_guard<std::mutex> lock(mtx);
	//loop through all users in the loged_users vector
	for (const auto& user : this->_loggedUsers)
	{
		if (user.getUsername() == username)  //if user found
		{
			return true;
		}
	}
	return false;  //if user wasnt found
}