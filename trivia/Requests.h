#pragma once
#include <string>
#include <vector>
#include <ctime> 
/*
this file is for all the requests structs
*/

class IRequestHandler; // Forward declaration to prevent circular dependency
struct RequestResult
{
    std::vector<unsigned char> response;    //the buffer of the response
    IRequestHandler* newHandler;  //the next handler
};

//enum for the request codes
enum RequestId
{
    LOGIN_REQUEST = 1,
    SIGNUP_REQUEST = 2,
    LOGOUT_REQUEST = 3,
    GET_ROOMS_REQUEST = 4,
    GET_PLAYERS_IN_ROOMS_REQUEST = 5,
    JOIN_ROOM_REQUEST = 6,
    CREATE_ROOM_REQUEST = 7,
    GET_HIGH_SCORE_REQUEST = 8,
    GET_PERSONAL_STATS_REQUEST = 9,
    CLOSE_ROOM_REQUEST = 10,
    START_GAME_REQUEST = 11,
    GET_ROOM_STATE_REQUEST = 12,
    LEAVE_ROOM_REQUEST = 13,
    SUBMIT_ANSWER_REQUEST = 14,
    GET_GAME_RESULTS_REQUEST = 15,
    GET_QUESTION_REQUEST = 16,
    LEAVE_GAME_REQUEST = 17
};

//struct for the info
struct RequestInfo
{
    int id;                      
    time_t receivalTime;               
    std::vector<unsigned char> buffer; 
};

//struct for login request
struct LoginRequest
{
    std::string username;
    std::string password;
};

//struct for signup request
struct SignupRequest
{
    std::string username;
    std::string password;
    std::string email;
};

//struct for get players in room request
struct GetPlayersInRoomRequest
{
    unsigned int roomId;
};

//struct for join room request
struct JoinRoomRequest
{
    unsigned int roomId;
};

//struct for create room request
struct CreateRoomRequest
{
    std::string roomName;
    unsigned int maxUsers;   
    unsigned int questionCount;    
    unsigned int answerTimeout;
};

// struct for submit answer request
struct SubmitAnswerRequest
{
    unsigned int answerId;
};