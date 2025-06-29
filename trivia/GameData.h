#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Question.h"
//struct that represent a game data of a user in a game
struct GameData
{
	std::string username;
	Question currentQuestion;
	unsigned int correctAnswerCount;
	unsigned int wrongAnswerCount;
	float averageAnswerTime;
};