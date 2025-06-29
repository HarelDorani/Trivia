#pragma once
#include <iostream>
#include <string>
#include <vector>
// this class represents a question in the trivia game
class Question
{
public:
	/*
	constractor
	*/
	Question();
	/*
	constractor
	input: the question string, vector of possible answers, index of correcr answer
	*/
	Question(const std::string& question, const std::vector<std::string>& possibleAnswers, int correctAnswerId);
	//distractor
	~Question();
	//getter to the question string
	std::string getQuestion() const;
	//getter to the possible answers vector
	std::vector<std::string> getPossibleAnswers() const;
	//getter to the correct answer id
	int getCorrectAnswerID() const;
private:
	int m_correctAnswerId;
	std::string m_question;
	std::vector<std::string> m_possibleAnswers;  //vector that holds the possible answers of the question
};