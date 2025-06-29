#include "GameRequestHandler.h"
#include "Question.h"
#include "MenuRequestHandler.h"
#include "RoomAdminRequestHandler.h"
#include "RoomMemberRequestHandler.h"
GameRequestHandler::GameRequestHandler(std::shared_ptr<Game> game, LoggedUser user, GameManager& gameManager, RequestHandlerFactory& handlerFactory)
	: _game(game), _user(user), _gameManager(gameManager), _handlerFactory(handlerFactory)
{
}
bool GameRequestHandler::isRequestRelevant(RequestInfo req)
{
	//checking if the request code is 1 of the codes that are allowed for the handler
	if (req.id == SUBMIT_ANSWER_REQUEST
		|| req.id == GET_GAME_RESULTS_REQUEST
		|| req.id == GET_QUESTION_REQUEST
		|| req.id == LEAVE_GAME_REQUEST)
	{
		return true;
	}
	return false;
}
RequestResult GameRequestHandler::handleRequest(RequestInfo req)
{
	//for each case calling func that handle the request
	switch (req.id)
	{
	case SUBMIT_ANSWER_REQUEST:
		return submitAnswer(req);  
		break;
	case GET_GAME_RESULTS_REQUEST:
		return getGameResults(req);
		break;
	case GET_QUESTION_REQUEST:
		return getQuestion(req);
		break;
	case LEAVE_GAME_REQUEST:
		return leaveGame(req);
		break;
	default:
		//if request is not 1 of the cases - sending error msg
		ErrorResponse error = { "Invalid request!" };
		//creating ErrorResponse as the RequestResult with the next handler
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createGameRequestHandler(_user) };
		break;
	}
}
RequestResult GameRequestHandler::getQuestion(RequestInfo request)
{
	RequestResult result;  //intializing the RequestResult struct
	GetQuestionResponse res;  //intializing the GetQuestionResponse struct
	try
	{
		Question question = _game->getQuestionForUser(_user);  //calling func that gets question for user
		res.question = question.getQuestion();  //adding the question string to the response struct
		res.answers = question.getPossibleAnswers();  //adding the possible answers to the response struct
		res.correctAnswerID = question.getCorrectAnswerID();  //adding the correct answer id to the response struct
		res.status = SUCCESS;  //setting status to success
		//seriallizing the GetQuestionResponse struct and setting the response buffer
		result.response = JsonResponsePacketSerializer::serializeResponse(res);
		//setting next handler as curr handler, calling funct that creats the handler from the handler factory
		result.newHandler = _handlerFactory.createGameRequestHandler(_user);
		return result;  //returning the RequestResult struct
	}
	catch (const std::exception& e) //if error occured
	{
		//sending error msg with the exception that cought
		ErrorResponse error;
		error.message = e.what();
		//return seriallized error response
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createGameRequestHandler(_user) };
	}

}
RequestResult GameRequestHandler::submitAnswer(RequestInfo request)
{
	RequestResult result;   //intializing the RequestResult struct
	SubmitAnswerResponse answerRes;  //intializing the SubmitAnswerResponse struct
	try
	{
		//calling func that deserialize the request buffer and initializing new SubmitAnswerRequest struct with the result
		SubmitAnswerRequest answerReq = JsonRequestPacketDeserializer::deserializeSubmitAnswerRequest(request.buffer);
		//calling func that submit the answer, id from the SubmitAnswerRequest struct
		_game->submitAnswer(answerReq.answerId, _user); 
		//calling func that gets the correct answer id of the question and adding data to the struct
		answerRes.correctAnswerId = _game->getGameDataForUser(_user).currentQuestion.getCorrectAnswerID();
		answerRes.status = SUCCESS;  //setting status as success
		result.response = JsonResponsePacketSerializer::serializeResponse(answerRes);  //serializing the response
		result.newHandler = _handlerFactory.createGameRequestHandler(_user);  //setting next handler as curr handler
	}
	catch (const std::exception& e)
	{
		ErrorResponse error;
		error.message = e.what();
		//return seriallized error response
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createGameRequestHandler(_user) };
	}

	return result;  //returning the result
}
RequestResult GameRequestHandler::getGameResults(RequestInfo request)
{
	RequestResult result;  //intializing the RequestResult struct
	GetGameResultsResponse res;   //intializing the GetGameResultsResponse struct
	try
	{
		//getting all players in the game
		std::vector<LoggedUser> allPlayers = _game->getAllPlayers();
		for (auto& user : allPlayers)  //looping through each player
		{
			PlayerResult playerResult;  //creating PlayerResult struct for each player
			//calling func that gets the avg answer time of the player from the db
			playerResult.averageAnswerTime = _game->getAverageAnswerTimeFromDB(user.getUsername());
			//getting func that gets the num of correcct answers of player from the db
			playerResult.correctAnswerCount = _game->getCorrectAnswerCountFromDB(user.getUsername());
			playerResult.username = user.getUsername();  //setting the username
			//calling func that gets the num of wrong answers of the player from the db
			playerResult.wrongAnswerCount = _game->getWrongAnswerCountFromDB(user.getUsername());
			res.results.push_back(playerResult);  //pushing the PlayerResult struct to the results
		}
		res.status = SUCCESS;  //setting status as success
		//sorting the vector of the results of the players by num of correct answers and the avg answer time
		std::sort(res.results.begin(), res.results.end(), [](const PlayerResult& a, const PlayerResult& b) {
			if (a.correctAnswerCount != b.correctAnswerCount)
				return a.correctAnswerCount > b.correctAnswerCount;
			return a.averageAnswerTime < b.averageAnswerTime;
			});
		//looping through all users in the game
		for (auto& user : _game->getAllPlayers())
		{
			//calling function that updates their statistics table in the db
			_game->submitGameStatsToDB(_game->getGameDataForUser(user));
		}
		//new handler is curr handler, maybe ill change it later
		result.newHandler = _handlerFactory.createGameRequestHandler(_user);
		//seriallizing the response
		result.response = JsonResponsePacketSerializer::serializeResponse(res);
	}
	catch (const std::exception& e)
	{
		ErrorResponse error;
		error.message = e.what();
		//return seriallized error response
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createGameRequestHandler(_user) };
	}
	return result;  //returning result
}
RequestResult GameRequestHandler::leaveGame(RequestInfo request)
{
	RequestResult result;
	LeaveGameResponse res;
	try
	{
		res.status = SUCCESS;  //setting statis to success
		//serializing the response
		result.response = JsonResponsePacketSerializer::serializeResponse(res);
		//setting next handler to the menu handler
		IRequestHandler* newHandler = this->_handlerFactory.createMenuRequestHandler(_user);
		result.newHandler = newHandler;
		return result;  //returning result
		//not removing the player from the game so its data will be kept for the game results
	}
	catch (const std::exception& e)
	{
		ErrorResponse error;
		error.message = e.what();
		//return seriallized error response
		return { JsonResponsePacketSerializer::serializeResponse(error), _handlerFactory.createGameRequestHandler(_user) };
	}

	return result;
}