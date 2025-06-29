#include "Question.h"
Question::Question(const std::string& question, const std::vector<std::string>& possibleAnswers, int correctAnswerId)
    : m_question(question), m_possibleAnswers(possibleAnswers), m_correctAnswerId(correctAnswerId) {}
Question::Question()
{}
Question::~Question()
{
}
std::string Question::getQuestion() const
{
    return this->m_question;  //getting the question
}
std::vector<std::string> Question::getPossibleAnswers() const
{
    return this->m_possibleAnswers;  //getting the answers vector
}
int Question::getCorrectAnswerID() const
{
    return this->m_correctAnswerId;  //getting the id of the answer
}