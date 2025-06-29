#pragma once
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "RoomData.h"
/*
file for all response structs
*/

//enum for all the codes
enum ResponseCodes
{
	LOGIN_CODE = 1,
	SIGNUP_CODE = 2,
	LOGOUT_CODE = 3,
	GET_ROOMS_CODE = 4,
	GET_PLAYERS_IN_ROOMS_CODE = 5,
	JOIN_ROOM_CODE = 6,
	CREATE_ROOM_CODE = 7,
	GET_HIGH_SCORE_CODE = 8,
	GET_PERSONAL_STATS_CODE = 9,
	CLOSE_ROOM_CODE = 10,
	START_GAME_CODE = 11,
	GET_ROOM_STATE_CODE = 12,
	LEAVE_ROOM_CODE = 13,
	SUBMIT_ANSWER_CODE = 14,
	GET_GAME_RESULTS_CODE = 15,
	GET_QUESTION_CODE = 16,
	LEAVE_GAME_CODE = 17
};
//stract for login response
struct LoginResponse
{
	unsigned int status;
};

//stract for signup response
struct SignupResponse
{
	unsigned int status;
};

//struct for error response
struct ErrorResponse
{
	std::string message;
};

//stract for join room response
struct JoinRoomResponse
{
	unsigned int status;
};

//stract for create room response
struct CreateRoomResponse
{
	unsigned int status;
	unsigned int roomID;
};

//stract for logout response
struct LogoutResponse
{
	unsigned int status;
};

//stract for get room response
struct GetRoomsResponse
{
	unsigned int status;
	std::vector<RoomData> rooms;
};

//stract for get players in room response
struct GetPlayersInRoomResponse
{
	std::vector<std::string> players;
};

//stract for get high score response
struct GetHighScoreResponse
{
	unsigned int status;
	std::vector<std::string> statistics;
};

//stract for get personal stats response
struct GetPersonalStatsResponse
{
	unsigned int status;
	std::vector<std::string> statistics;
};

//stract for close room response
struct CloseRoomResponse
{
	unsigned int status;
};
//stract for start game response
struct StartGameResponse
{
	unsigned int status;
};
//stract for get room state response
struct GetRoomStateResponse
{
	unsigned int status;
	bool hasGameBegun;
	std::vector<std::string> players;
	unsigned int questionsCount;
	unsigned int answerTimeout;

};
//stract for leave room response
struct LeaveRoomResponse
{
	unsigned int status;
};

//stract for PlayerResult response
struct PlayerResult 
{
	std::string username;
	unsigned int correctAnswerCount;
	unsigned int wrongAnswerCount;
	float averageAnswerTime;
};

//stract for GetGameResultsResponse response
struct GetGameResultsResponse
{
	unsigned int status;
	std::vector<PlayerResult> results;
};

//stract for SubmitAnswerResponse response
struct SubmitAnswerResponse
{
	unsigned int status;
	unsigned int correctAnswerId;
};

//stract for GetQuestionResponse response
struct GetQuestionResponse
{
	unsigned int status;
	std::string question;
	std::vector<std::string> answers;
	unsigned int correctAnswerID;
};

//stract for LeaveGameResponse response
struct LeaveGameResponse
{
	unsigned int status;
};
