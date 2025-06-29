#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Responses.h"
#include "json.hpp"
#define SUCCESS 1
#define FAIL 0
using json = nlohmann::json;   //json class
typedef std::vector<unsigned char> Buffer;  //the buffer of bytes
//class for serialaizing responses
class JsonResponsePacketSerializer
{
public:
	//func that serialize response for login request
	static Buffer serializeResponse(LoginResponse res);
	//func that serialize response for signup request
	static Buffer serializeResponse(SignupResponse res);
	//func that serialize response for error response
	static Buffer serializeResponse(ErrorResponse res);
	//func that serialize response for logout response
	static Buffer serializeResponse(LogoutResponse res);
	//func that serialize response for get room response
	static Buffer serializeResponse(GetRoomsResponse res);
	//func that serialize response for get players in room response
	static Buffer serializeResponse(GetPlayersInRoomResponse res);
	//func that serialize response for join room response
	static Buffer serializeResponse(JoinRoomResponse res);
	//func that serialize response for create room response
	static Buffer serializeResponse(CreateRoomResponse res);
	//func that serialize response for get high score response
	static Buffer serializeResponse(GetHighScoreResponse res);
	//func that serialize response for get personal stats response
	static Buffer serializeResponse(GetPersonalStatsResponse res);
	//func that serialize response for close room response
	static Buffer serializeResponse(CloseRoomResponse res);
	//func that serialize response for start game response
	static Buffer serializeResponse(StartGameResponse res);
	//func that serialize response for get room state response
	static Buffer serializeResponse(GetRoomStateResponse res);
	//func that serialize response for leave room response
	static Buffer serializeResponse(LeaveRoomResponse res);
	//func that serialize response for get game results response
	static Buffer serializeResponse(GetGameResultsResponse res);
	//func that serialize response for submit answer response
	static Buffer serializeResponse(SubmitAnswerResponse res);
	//func that serialize response for get question response
	static Buffer serializeResponse(GetQuestionResponse res);
	//func that serialize response for leave game response
	static Buffer serializeResponse(LeaveGameResponse res);


private:
	/*
	this func bield the buffer thet is sent to the client
	input: the statuc code, the json string
	output: vector of chars - bytes, each element represent diff msg:
			first byte: the status code.
			4 bytes: the length of the msg.
			the rest: the msg.
	*/
	static Buffer buildBuffer(unsigned char code, const std::string& jsonStr);
};