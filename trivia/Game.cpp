#include "Game.h"
#include <stdexcept>
#include <algorithm>

// Constructor
Game::Game(unsigned int gameId, IDataBase* database)
    : _gameId(gameId), _database(database)
{
}

unsigned int Game::getGameId()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _gameId;   //getting gameId
}

Question Game::getQuestionForUser(const LoggedUser& user)
{
    std::lock_guard<std::mutex> lock(_mutex);  //locking
    auto it = _players.find(user);   //finding the user in the map
    if (it == _players.end())  //if user wasnt fount
    {
        throw std::runtime_error("User not found in game.");
    }

    GameData& data = it->second;   //getting the game data of the user

    //if curr question of the user is equal to the last question in the questions vector
    if (data.currentQuestion.getQuestion() == this->_questions.back().getQuestion())
    {
        throw std::runtime_error("No more questions left.");
    }
    else if (data.currentQuestion.getQuestion().empty())
    {
        data.currentQuestion = _questions[0];   //no questions yet so getting the first
    }
    else
    {
        //looping through all the questions vector - 1 to prevent violation access
        for (size_t i = 0; i < _questions.size() - 1; ++i)
        {
            //if curr question in the vector is equal to the curr question of the user
            if (_questions[i].getQuestion() == data.currentQuestion.getQuestion())
            {
                //getting the next question in the vector for the user and updating it to be the curr question for the user
                data.currentQuestion = _questions[i + 1]; 
                break;  //exiting loop
            }
        }
    }
    //keeping the curr time - time user got the question
    _questionStartTimes[user.getUsername()] = std::chrono::steady_clock::now();
    return data.currentQuestion;   //returning the curr question of the user
}

void Game::submitAnswer(const unsigned int answerID, LoggedUser& user)
{
    std::lock_guard<std::mutex> lock(_mutex);  //locking
    updateAnswerTime(user);   //calling func that update the avg answer time and stops the clock of the user
    auto it = _players.find(user);  //finding user in the map
    GameData& data = it->second;   //getting user game data
    //if the answer id is the correct answer id
    if (answerID == data.currentQuestion.getCorrectAnswerID())
    {
        data.correctAnswerCount++;   //updating the num of the correct answers
    }
    else
    {
        data.wrongAnswerCount++;   //updating the num of the wrong answers
    }
    data.username = user.getUsername();   //setting the username in the game data
    //updating the game data table of the user with the new updated game data
    _database->SubmitGameStatistics(data);
}

void Game::removePlayer(const LoggedUser& user)
{
    std::lock_guard<std::mutex> lock(_mutex);   //locking
    auto it = _players.find(user);   //getting player from the map
    if (it != _players.end())   //if exist in map
    {
        GameData data = _players[user];   //getting the game data of the user
        submitGameStatsToDB(data);   //updating the statistics table with the game data of the player
        _players.erase(it);   //removing player
    }
    else
    {
        throw std::runtime_error("User not found in game.");
    }
}

void Game::submitGameStatsToDB(const GameData& gameData)
{
    //calling func that updates the user statistics table with the game data
    _database->updateUserStatistics(gameData);
}

void Game::setQuestions(const std::vector<Question> questions)
{
    this->_questions = questions;   //setting the questions of the game
}

bool Game::hasUser(const LoggedUser& user) const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _players.find(user) != _players.end();   //checking if user exist in the users map
}
GameData& Game::getGameDataForUser(const LoggedUser& user)
{
    std::lock_guard<std::mutex> lock(_mutex);  //locking

    auto it = _players.find(user);  //getting user
    if (it == _players.end())
    {
        throw std::runtime_error("User not found in this game");
    }
    return it->second;  //getting the users game data
}
std::vector<LoggedUser> Game::getAllPlayers() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    std::vector<LoggedUser> users;   //creating a vector of users

    for (const auto& pair : _players)  //looping through all map
    {
        users.push_back(pair.first);   //pushing the key value of the map - the users
    }

    return users;  //returning the vector of users
}

unsigned int Game::getCorrectAnswerCountFromDB(const std::string& username) const
{
    return _database->correctAnswerCount(username);   //getting the num of correct answers of user from the db
}

unsigned int Game::getWrongAnswerCountFromDB(const std::string& username) const
{
    return _database->wrongAnswerCount(username);   //getting the num of wrong answers of the user from the db
}

float Game::getAverageAnswerTimeFromDB(const std::string& username) const
{
    return _database->averageAnswerTime(username);   //getting the avg answer time of the user from the db
}

void Game::addPlayer(const LoggedUser& user)
{
    //adding player to the map
    std::lock_guard<std::mutex> lock(_mutex);
    _players[user] = GameData{ user.getUsername(), Question(), 0, 0, 0};
}
void Game::updateAnswerTime(const LoggedUser& user)
{
    auto now = std::chrono::steady_clock::now();   //getting the curr time
    std::string username = user.getUsername();  //getting username

    if (_questionStartTimes.count(username))   //if time keeped for user in the map
    {
        auto startTime = _questionStartTimes[username];   //the time user got the question
        // time taken is the curr time - time user submit the answer minus the time the user got the question divided to get time as seconds and not millieseconds
        float timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count() / 1000.0f;

        GameData& data = _players[user];  //getting the game data of the user

        int prevAnswers = data.correctAnswerCount + data.wrongAnswerCount;  //getting total answers count before this curr answer
        int totalAnswers = prevAnswers + 1;  //total answers including curr answer

        //calc the new ang answer time and apdating the game data
        data.averageAnswerTime = (data.averageAnswerTime * prevAnswers + timeTaken) / totalAnswers;

        _questionStartTimes.erase(username);   //removing the time user got the curr question
    }
}

