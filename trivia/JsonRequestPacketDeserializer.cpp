#include "JsonRequestPacketDeserializer.h"
LoginRequest JsonRequestPacketDeserializer::deserializeLoginRequest(const Buffer& buffer)
{
	std::string jsonStr(buffer.begin(), buffer.end()); //convert bytes to string
	json j = json::parse(jsonStr); //parse into JSON
	//check if the str have the right fields for login
	if (!j.contains("username") || !j.contains("password"))
	{
		throw std::runtime_error("Invalid login request");
	}

	//adding fields to the struct
	LoginRequest req;
	req.username = j["username"];
	req.password = j["password"];
	return req;
}
SignupRequest JsonRequestPacketDeserializer::deserializeSignupRequest(const Buffer& buffer)
{
	std::string jsonStr(buffer.begin(), buffer.end());  //convert bytes to string
	json j = json::parse(jsonStr); //parse into JSON	
	//check if the str have the right fields for signup
	if (!j.contains("username") || !j.contains("password") || !j.contains("email"))
	{
		throw std::runtime_error("Invalid signup request");
	}

	//adding fields to the struct
	SignupRequest req;
	req.username = j["username"];
	req.password = j["password"];
	req.email = j["email"];
	return req;
}

GetPlayersInRoomRequest JsonRequestPacketDeserializer::deserializeGetPlayersRequest(const Buffer& buffer)
{
	std::string jsonStr(buffer.begin(), buffer.end());  //convert bytes to string
	json j = json::parse(jsonStr); //parse into JSON
	//check if the str have the right fields for the request
	if (!j.contains("roomId"))
	{
		throw std::runtime_error("Invalid request");
	}

	//adding fields to the struct
	GetPlayersInRoomRequest req;
	req.roomId = j["roomId"];
	return req;
}
JoinRoomRequest JsonRequestPacketDeserializer::deserializeJoinRoomRequest(const Buffer& buffer)
{
	std::string jsonStr(buffer.begin(), buffer.end());  //convert bytes to string
	json j = json::parse(jsonStr); //parse into JSON
	//check if the str have the right fields for the request
	if (!j.contains("roomId"))
	{
		throw std::runtime_error("Invalid request");
	}

	//adding fields to the struct
	JoinRoomRequest req;
	req.roomId = j["roomId"];
	return req;
}
CreateRoomRequest JsonRequestPacketDeserializer::deserializeCreateRoomRequest(const Buffer& buffer)
{
	std::string jsonStr(buffer.begin(), buffer.end());  //convert bytes to string
	json j = json::parse(jsonStr); //parse into JSON
	//check if the str have the right fields for the request
	if (!j.contains("answerTimeout") || !j.contains("maxUsers") || !j.contains("questionCount") || !j.contains("roomName"))
	{
		throw std::runtime_error("Invalid request");
	}

	//adding fields to the struct
	CreateRoomRequest req;
	req.answerTimeout = j["answerTimeout"];
	req.maxUsers = j["maxUsers"];
	req.questionCount = j["questionCount"];
	req.roomName = j["roomName"];
	return req;
}

SubmitAnswerRequest JsonRequestPacketDeserializer::deserializeSubmitAnswerRequest(const Buffer& buffer)
{
	std::string jsonStr(buffer.begin(), buffer.end());  //convert bytes to string
	json j = json::parse(jsonStr);  //parse into JSON
	//check if the str have the right fields for the request
	if (!j.contains("answerId"))
	{
		throw std::runtime_error("Invalid submit answer request");
	}

	//adding fields to the struct
	SubmitAnswerRequest req;
	req.answerId = j["answerId"];

	return req;
}