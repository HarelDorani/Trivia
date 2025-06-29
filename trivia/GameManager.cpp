#include "GameManager.h"
#include <exception>
GameManager::GameManager(IDataBase* database) : _database(database)
{
}

std::shared_ptr<Game> GameManager::createGame(const std::shared_ptr<Room> room, const std::string& admin)
{
    std::lock_guard<std::mutex> lock(_mutex);
    this->_admin = admin;   //setting the admin
    unsigned int newGameId = 1;    //game id is 1 if its the first game

    if (!_games.empty())   //if there are games in vector
    {
        newGameId = _games.back()->getGameId() + 1;   //getting the last games id and adding 1 to set the new game id
    }

    //creating a new game with the game id and pointer to the db
    std::shared_ptr<Game> newGame = std::make_shared<Game>(newGameId, _database); 
    //setting questions for the new game by calling the db func that gets random questions from the questions table
    // with the number of questions that are set in the room
    newGame->setQuestions(_database->getQuestions(room->getRoomData().numOfQuestionsInGame));
    //looping through all the users in the room and adding them to the game
    for (const LoggedUser& user : room->getAllUsers())  
    {
        newGame->addPlayer(user);  //calling func that adds users to a game
    }
    _games.push_back(newGame);  //adding the new game to the games vector

    return newGame;   //returning the new game
}

void GameManager::deleteGame(unsigned int gameId)
{
    std::lock_guard<std::mutex> lock(_mutex);  //locking

    // move games with matching id to the end of the vector
    auto it = std::remove_if(_games.begin(), _games.end(),
        [gameId](const std::shared_ptr<Game>& game)
        {
            return game->getGameId() == gameId;
        });

    if (it == _games.end())  //if end is reached and no game with the id found
    {
        throw std::runtime_error("Game not found with ID: " + std::to_string(gameId));
    }

    _games.erase(it, _games.end());  //erasing the game that matched the id
}

std::shared_ptr<Game> GameManager::getGame(int id)
{
    std::lock_guard<std::mutex> lock(_mutex);

    for (const auto& game : _games)  //looping through the games vector
    {
        if (game->getGameId() == id)  //if IDs match
        {
            return game;  //returning the curr game
        }
    }

    throw std::runtime_error("Game not found with ID: " + std::to_string(id));  //no game found
}
std::shared_ptr<Game> GameManager::getGameByUser(const LoggedUser& user)
{
    std::lock_guard<std::mutex> lock(_mutex);

    for (const auto& game : _games)  //loopping through all games
    {
        if (game->hasUser(user)) //calling func that checks if user is inside a game
        {
            return game;  //returning the game if user was found in it
        }
    }

    throw std::runtime_error("Game not found for user: " + user.getUsername());  //the user wasnt found in any game
}

std::string GameManager::getAdmin() const
{
    return this->_admin;  //getting the admin
}
