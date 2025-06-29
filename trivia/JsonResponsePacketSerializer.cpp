#include "JsonResponsePacketSerializer.h"
Buffer JsonResponsePacketSerializer::serializeResponse(LoginResponse res)
{
    json j;
    int loginCode = LOGIN_CODE;  //code for login
    if (res.status == SUCCESS)
    {
        j = { {"status", "SUCCESS"} };  //the json string
    }
    else
    {
        j = { {"status", "FAILED"} };
    }
    return buildBuffer(loginCode, j.dump());  //call for func that build the buffer of bytes
}
Buffer JsonResponsePacketSerializer::serializeResponse(SignupResponse res)
{
    json j;
    int signupCode = SIGNUP_CODE;   //code for signup
    if (res.status == SUCCESS)
    {
        j = { {"status", "SUCCESS"} };  //the json string
    }
    else
    {
        j = { {"status", "FAILED"} };
    }
    return buildBuffer(signupCode, j.dump());  //call func that build the buffer of bytes
}
Buffer JsonResponsePacketSerializer::serializeResponse(LogoutResponse res)
{
    json j;
    int logoutCode = LOGOUT_CODE;  //code for logout
    if (res.status == SUCCESS)
    {
        j = { {"status", "SUCCESS"} };  //the json string
    }
    else
    {
        j = { {"status", "FAILED"} };
    }
    return buildBuffer(logoutCode, j.dump());  //call for func that build the buffer of bytes
}
Buffer JsonResponsePacketSerializer::serializeResponse(GetRoomsResponse res)
{
    json j;
    int getRoomsCode = GET_ROOMS_CODE;  //code for get rooms
    if (res.status == SUCCESS)
    {
        j["status"] = "SUCCESS";
        std::vector<json> roomsJson;  //vector of jsons
        for (const auto& room : res.rooms)  //looping through all rooms
        {
            //addinng each rooms as json
            roomsJson.push_back({
                {"id", room.id},
                {"maxPlayers", room.maxPlayers},
                {"name", room.name},
                {"numOfQuestionsInGame", room.numOfQuestionsInGame},
                {"state", room.status},
                {"timePerQuestion", room.timePerQuestion}
                });
        }
        // the results val is the vector of jsons that represents the rooms
        j["results"] = roomsJson;
    }
    else
    {
        j = { {"status", "FAILED"} };
    }
    return buildBuffer(getRoomsCode, j.dump());  //call for func that build the buffer of bytes
}
Buffer JsonResponsePacketSerializer::serializeResponse(GetPlayersInRoomResponse res)
{
    json j;
    int getPlayersInRoomCode = GET_PLAYERS_IN_ROOMS_CODE;  //code for get Players In Room
    j = { {"PlayersInRoom", res.players}, {"status", "SUCCESS"} };  //getting all players

    return buildBuffer(getPlayersInRoomCode, j.dump());  //call for func that build the buffer of bytes
}
Buffer JsonResponsePacketSerializer::serializeResponse(JoinRoomResponse res)
{
    json j;
    int joinRoomCode = JOIN_ROOM_CODE;  //status code for join room
    if (res.status == SUCCESS)
    {
        j = { {"status", "SUCCESS"} };  //the json string
    }
    else
    {
        j = { {"status", "FAILED"} };
    }
    return buildBuffer(joinRoomCode, j.dump());  //call for func that build the buffer of bytes
}
Buffer JsonResponsePacketSerializer::serializeResponse(CreateRoomResponse res)
{
    json j;
    int createRoomCode = CREATE_ROOM_CODE;  //status code for create room
    if (res.status == SUCCESS)
    {
        j = { {"status", "SUCCESS"} , { "roomID", res.roomID } };  //the json string
    }
    else
    {
        j = { {"status", "FAILED"} };
    }
    return buildBuffer(createRoomCode, j.dump());  //call for func that build the buffer of bytes
}
Buffer JsonResponsePacketSerializer::serializeResponse(GetHighScoreResponse res)
{
    json j;
    int getHighScoresCode = GET_HIGH_SCORE_CODE; //status code for high score
    if (res.status == SUCCESS)
    {
        j = { {"HighScore", res.statistics} , {"status", "SUCCESS"} };  //getting all highscores stats
    }
    else
    {
        j = { {"status", "FAILED"} };
    }
    return buildBuffer(getHighScoresCode, j.dump());  //call for func that build the buffer of bytes
}
Buffer JsonResponsePacketSerializer::serializeResponse(GetPersonalStatsResponse res)
{
    json j;
    int getPersonalStatsCode = GET_PERSONAL_STATS_CODE; //status code for personal stats
    if (res.status == SUCCESS)
    {
       j = { {"PersonalStats", res.statistics} , {"status", "SUCCESS"} };  //getting all person stats stats
    }
    else
    {
        j = { {"status", "FAILED"} };
    }

    return buildBuffer(getPersonalStatsCode, j.dump());  //call for func that build the buffer of bytes
}

Buffer JsonResponsePacketSerializer::serializeResponse(CloseRoomResponse res)
{
    json j;
    int closeRoomCode = CLOSE_ROOM_CODE;  //status code for close room
    if (res.status == SUCCESS)
    {
        j = { {"status", "SUCCESS"} };  //the json string
    }
    else
    {
        j = { {"status", "FAILED"} };
    }
    return buildBuffer(closeRoomCode, j.dump());  //call for func that build the buffer of bytes
}
Buffer JsonResponsePacketSerializer::serializeResponse(StartGameResponse res)
{
    json j;
    int startGameCode = START_GAME_CODE;  //status code for start game
    if (res.status == SUCCESS)
    {
        j = { {"status", "SUCCESS"} };  //the json string
    }
    else
    {
        j = { {"status", "FAILED"} };
    }
    return buildBuffer(startGameCode, j.dump());  //call for func that build the buffer of bytes
}
Buffer JsonResponsePacketSerializer::serializeResponse(GetRoomStateResponse res)
{
    json j;
    int getRoomStateCode = GET_ROOM_STATE_CODE;  //status code for get room state
    if (res.status == SUCCESS)
    {
        //the json with all feilds from struct
        j = {
                {"hasGameBegan", res.hasGameBegun},
                {"players", res.players},
                {"questionCount", res.questionsCount},
                {"answerTimeout", res.answerTimeout},
                {"status", "SUCCESS"}
        };
    }
    else
    {
        j = { {"status", "FAILED"} };
    }
    return buildBuffer(getRoomStateCode, j.dump());  //call for func that build the buffer of bytes
}
Buffer JsonResponsePacketSerializer::serializeResponse(LeaveRoomResponse res)
{
    json j;
    int leaveRoomCode = LEAVE_ROOM_CODE;  //status code for leave room
    if (res.status == SUCCESS)
    {
        j = { {"status", "SUCCESS"} };  //the json string
    }
    else
    {
        j = { {"status", "FAILED"} };
    }
    return buildBuffer(leaveRoomCode, j.dump());  //call for func that build the buffer of bytes
}

Buffer JsonResponsePacketSerializer::serializeResponse(LeaveGameResponse res)
{
    json j;
    if (res.status == SUCCESS)
    {
        j = { {"status", "SUCCESS"} };  //the json string
    }
    else
    {
        j = { {"status", "FAILED"} };
    }

    //call for func that build the buffer of bytes with the leave game code
    return buildBuffer(LEAVE_GAME_CODE, j.dump());
}

Buffer JsonResponsePacketSerializer::serializeResponse(SubmitAnswerResponse res)
{
    json j;
    if (res.status == SUCCESS)
    {
        j = { {"status", "SUCCESS"},
        {"correctAnswerId", res.correctAnswerId} };  //the json string
    }
    else
    {
        j = { {"status", "FAILED"} };
    }
    //call for func that build the buffer of bytes with the submit answer code
    return buildBuffer(SUBMIT_ANSWER_CODE, j.dump());
}


Buffer JsonResponsePacketSerializer::serializeResponse(GetGameResultsResponse res)
{
    json j;
    if (res.status == SUCCESS)
    {
        j["status"] = "SUCCESS"; 
        // convert each PlayerResult to JSON and store in a vector
        std::vector<json> resultsJson;
        for (const auto& result : res.results)
        {
            resultsJson.push_back({
                {"username", result.username},
                {"correctAnswerCount", result.correctAnswerCount},
                {"wrongAnswerCount", result.wrongAnswerCount},
                {"averageAnswerTime", result.averageAnswerTime}
                });
        }
        // assign the array of player results to the JSON object
        j["results"] = resultsJson;
    }
    else
    {
        j = { {"status", "FAILED"} };
    }
    //call for func that build the buffer of bytes with the get game results code
    return buildBuffer(GET_GAME_RESULTS_CODE, j.dump());
}

Buffer JsonResponsePacketSerializer::serializeResponse(GetQuestionResponse res)
{
    json j;

    if (res.status == SUCCESS)
    {
        //converting the whole struct feilds to JSON
        j["status"] = "SUCCESS";
        j["question"] = res.question;
        j["answers"] = res.answers;
        j["correctAnswerId"] = res.correctAnswerID;
    }
    else
    {
        j = { {"status", "FAILED"} };
    }
    //call for func that build the buffer of bytes with the get question code
    return buildBuffer(GET_QUESTION_CODE, j.dump());
}


Buffer JsonResponsePacketSerializer::serializeResponse(ErrorResponse res)
{
    int errorCode = 100;   //statuc code for error
    json j = { {"message", res.message} };  //the json string
    return buildBuffer(errorCode, j.dump());  //call func that build the buffer of bytes
}
Buffer JsonResponsePacketSerializer::buildBuffer(unsigned char code, const std::string& jsonStr)
{
    Buffer buffer;   //creating the buffer
    buffer.push_back(code);    //pushing the status code

    int len = jsonStr.size();   //getting the len of the string
    //4 byte for string len
    buffer.push_back((len >> 24) & 0xFF);  //pushing first byte
    buffer.push_back((len >> 16) & 0xFF);  //pushing second byte
    buffer.push_back((len >> 8) & 0xFF);   //pushing third byte
    buffer.push_back(len & 0xFF);          //pushing last byte

    // pushing the string to the buffer
    buffer.insert(buffer.end(), jsonStr.begin(), jsonStr.end());
    return buffer;
}