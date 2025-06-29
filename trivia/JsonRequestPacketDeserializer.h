#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include "Requests.h"
#include "json.hpp"
using json = nlohmann::json;   //json class
typedef std::vector<unsigned char> Buffer;  //the buffer of bytes
//class for deserializing requests from client
class JsonRequestPacketDeserializer
{
public:
	/*
	this function deserialize a buffer from clien request and gets from that a login request.
	input: the buffer from the client
	output: LoginRequest struct that have req info.
	*/
	static LoginRequest deserializeLoginRequest(const Buffer& buffer);
	/*
	this function deserialize a buffer from clien request and gets from that a signup request.
	input: the buffer from the client
	output: SignupRequest struct that have req info.
	*/
	static SignupRequest deserializeSignupRequest(const Buffer& buffer);
	/*
	this function deserialize a buffer from clien request and gets from that a get players in room request.
	input: the buffer from the client
	output: GetPlayersInRoomRequest struct that have req info.
	*/
	static GetPlayersInRoomRequest deserializeGetPlayersRequest(const Buffer& buffer);
	/*
	this function deserialize a buffer from clien request and gets from that a join room request.
	input: the buffer from the client
	output: JoinRoomRequest struct that have req info.
	*/
	static JoinRoomRequest deserializeJoinRoomRequest(const Buffer& buffer);
	/*
	this function deserialize a buffer from clien request and gets from that a create room request.
	input: the buffer from the client
	output: CreateRoomRequest struct that have req info.
	*/
	static CreateRoomRequest deserializeCreateRoomRequest(const Buffer& buffer);
	/*
	this function deserialize a buffer from clien request and gets from that a submit answer request.
	input: the buffer from the client
	output: SubmitAnswerRequest struct that have req info.
	*/
	static SubmitAnswerRequest deserializeSubmitAnswerRequest(const Buffer& buffer);
};
