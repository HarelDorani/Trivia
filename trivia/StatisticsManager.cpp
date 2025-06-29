#include "StatisticsManager.h"
StatisticsManager::StatisticsManager(IDataBase* db) : m_database(db) {};
StatisticsManager::~StatisticsManager(){}
std::vector<std::string> StatisticsManager::getHighScore()
{
    //returning top 5 highest scores of all players
    return m_database->getHighScores();
}
std::vector<std::string> StatisticsManager::getUserStatistics(const std::string& username)
{
    std::vector<std::string> stats; //vector to hold all stats of the user
    //getting all the statistics from the db
    float avgTime = m_database->getPlayerAvarageAnswerTime(username);
    int correct = m_database->getNumOfCorrectAnswers(username);
    int total = m_database->gerNumOfTotalAnswers(username);
    int games = m_database->getNumOfPlayerGames(username);
    int score = m_database->getPlayerScore(username);

    //pushing back all stats to vector + converting to string
    stats.push_back("Average Answer Time: " + std::to_string(avgTime));
    stats.push_back("Correct Answers: " + std::to_string(correct));
    stats.push_back("Total Answers: " + std::to_string(total));
    stats.push_back("Games Played: " + std::to_string(games));
    stats.push_back("The player score: " + std::to_string(score));

    return stats;
}