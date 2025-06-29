#pragma once
#include "IDataBase.h"
#include <iostream>
#include <string>
#include <vector>
class StatisticsManager
{
public:
	//constracotor
	StatisticsManager(IDataBase* db);
	//distracotr
	~StatisticsManager();
	//func to get top 5 high scores
	std::vector<std::string> getHighScore();
	//func to get all the user statistics
	std::vector<std::string> getUserStatistics(const std::string& username);
private:
	IDataBase* m_database;
};