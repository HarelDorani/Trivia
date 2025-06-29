#pragma once

#include <string>
//this struct holds room data
struct RoomData
{
    unsigned int id;
    std::string name;
    unsigned int maxPlayers;
    unsigned int numOfQuestionsInGame;
    unsigned int timePerQuestion;
    bool status;
};
