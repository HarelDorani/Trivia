#include "SqliteDatabase.h"
#include <string>
SqliteDatabase::SqliteDatabase()
{
    open();  //calling func that open the db
}
SqliteDatabase::~SqliteDatabase()
{
    close();  //calling func that close the db
}
bool SqliteDatabase::open()
{   
    int file_exist = _access(_dbFileName.c_str(), 0);  //checking if db already exist
    int res = sqlite3_open(_dbFileName.c_str(), &_db);  //opening db
    if (res != SQLITE_OK) {  //if error happend when opening
        _db = nullptr;
        throw std::runtime_error("Failed to open DB");
    }
    if (!createTables()) {  //creating tables and checking if ok
        throw std::runtime_error("Failed to create tables.");
    }
    insertQuestions();  //calling func that insert questions to the db
    return true;
}
bool SqliteDatabase::close()
{
    std::lock_guard<std::mutex> lock(mtx);
    //closing the db
    if (_db) {
        sqlite3_close(_db);
        _db = nullptr;
        return true;
    }
    return false;
}
bool SqliteDatabase::doesUserExist(const std::string& username)
{
    std::lock_guard<std::mutex> lock(mtx);
   const char* sql = "SELECT 1 FROM USERS WHERE USERNAME = ?;";  //stmt to get user with the username
    sqlite3_stmt* stmt = nullptr;
    bool exists = false;

    if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);  //adding username
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            exists = true;
        }
    }

    sqlite3_finalize(stmt);
    return exists;
}
bool SqliteDatabase::doesPasswordMatch(const std::string& password, const std::string& username)
{
    std::lock_guard<std::mutex> lock(mtx);
    const char* sql = "SELECT PASSWORD FROM USERS WHERE USERNAME = ?;";  //getting password based on the username
    sqlite3_stmt* stmt = nullptr;
    bool match = false;

    if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);   //adding username

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            std::string dbPassword = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            if (dbPassword == password) //checking if its the same password
            {
                match = true;
            }
        }
    }

    sqlite3_finalize(stmt);
    return match;
}
bool SqliteDatabase::addNewUser(const std::string& password, const std::string& username, const std::string& email)
{
    std::lock_guard<std::mutex> lock(mtx);
    //stmt for adding new user
    const char* sql = "INSERT INTO USERS (USERNAME, PASSWORD, EMAIL) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    bool success = false;

    if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);  //adding username
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);  //adding passowrd
        sqlite3_bind_text(stmt, 3, email.c_str(), -1, SQLITE_STATIC);     //adding email

        if (sqlite3_step(stmt) == SQLITE_DONE)
        {
            success = true;
        }
    }

    sqlite3_finalize(stmt);
    return success;
}

void SqliteDatabase::insertQuestions()
{
    std::lock_guard<std::mutex> lock(mtx);
    //running the scrip that insert questions to the db
    int result = std::system("python questions.py");

    // checking if error happened while runnig the script
    if (result != 0) {
        throw std::runtime_error("Failed to run Python seed script.");
    }

}
bool SqliteDatabase::createTables()
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        //the tables of the db
        const char* sql = R"(
    CREATE TABLE IF NOT EXISTS USERS (
        USERNAME TEXT PRIMARY KEY,
        PASSWORD TEXT NOT NULL,
        EMAIL TEXT NOT NULL
    );

    CREATE TABLE IF NOT EXISTS STATISTICS (
        USERNAME TEXT PRIMARY KEY,
        AVG_TIME REAL,
        CORRECT_ANSWERS INTEGER,
        TOTAL_ANSWERS INTEGER,
        NUM_OF_GAMES INTEGER,
        FOREIGN KEY(USERNAME) REFERENCES USERS(USERNAME) ON DELETE CASCADE
    );

    CREATE TABLE IF NOT EXISTS GAME_DATA (
        USERNAME TEXT PRIMARY KEY,
        CORRECT_ANSWERS_COUNT INTEGER,
        WRONG_ANSWERS_COUNT INTEGER,
        AVG_ANSWER_TIME REAL,
        FOREIGN KEY(USERNAME) REFERENCES USERS(USERNAME) ON DELETE CASCADE
    );
)";


        char* errMsg = nullptr;
        if (sqlite3_exec(_db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {  //sending to sqlite and checking if its okay
            std::cerr << "Error creating tables: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }
    }
    return true;
}
float SqliteDatabase::getPlayerAvarageAnswerTime(const std::string& username)
{
    std::lock_guard<std::mutex> lock(mtx);
    sqlite3_stmt* stmt;
    float avgTime = 0.0f;
    //stmt for getting avg time
    const std::string sql = "SELECT AVG_TIME FROM STATISTICS WHERE USERNAME = ?";

    if (sqlite3_prepare_v2(_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);  //adding the username

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            avgTime = static_cast<float>(sqlite3_column_double(stmt, 0));  //getting the avg time
        }
    }
    sqlite3_finalize(stmt);
    return avgTime;
}
// func to get num of correct answers of the user
int SqliteDatabase::getNumOfCorrectAnswers(const std::string& username)
{
    std::lock_guard<std::mutex> lock(mtx);
    sqlite3_stmt* stmt;
    int correct = 0;
    //stmt to get correct answers
    const std::string sql = "SELECT CORRECT_ANSWERS FROM STATISTICS WHERE USERNAME = ?";

    if (sqlite3_prepare_v2(_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);  //adding username
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            correct = sqlite3_column_int(stmt, 0);  //getting the num of correct answers
        }
    }
    sqlite3_finalize(stmt);
    return correct;
}
// func to get num of total answers of the user
int SqliteDatabase::gerNumOfTotalAnswers(const std::string& username)
{
    std::lock_guard<std::mutex> lock(mtx);
    sqlite3_stmt* stmt;
    int total = 0;
    //stmt to get total of answers
    const std::string sql = "SELECT TOTAL_ANSWERS FROM STATISTICS WHERE USERNAME = ?";

    if (sqlite3_prepare_v2(_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);  //adding username
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            total = sqlite3_column_int(stmt, 0);  //getting the total
        }
    }
    sqlite3_finalize(stmt);
    return total;
}
// func to get num of games played by the user
int SqliteDatabase::getNumOfPlayerGames(const std::string& username)
{
    std::lock_guard<std::mutex> lock(mtx);
    sqlite3_stmt* stmt;
    int games = 0;
    //stsmt to get num of games
    const std::string sql = "SELECT NUM_OF_GAMES FROM STATISTICS WHERE USERNAME = ?";

    if (sqlite3_prepare_v2(_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);   //adding the username
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            games = sqlite3_column_int(stmt, 0);  //getting the num of games
        }
    }
    sqlite3_finalize(stmt);
    return games;
}
// this func calc the score of the player based on all the info in the statistics table
int SqliteDatabase::getPlayerScore(const std::string& username)
{
    int correct = getNumOfCorrectAnswers(username);  //getting num of correct answers of the user
    int total = gerNumOfTotalAnswers(username);  //get num of total answers of the user
    float avgTime = getPlayerAvarageAnswerTime(username);  //getting the ang time for answer of the user

    int baseScore = correct * 10;  //each correct answer give 10 points
    int penalty = (total - correct) * 2;  //each wrong answer remove 2 points - adding it to the penalty
    if (avgTime > 10.0f)  //if avg time for answer if bigger than 10 seconds
    {
        //removing half pointt for each second late - adding it to the penalty
        penalty += static_cast<int>((avgTime - 10.0f) * 0.5f);
    }
    //returning the base score minus the penalty - the score, of 0 if its bigger to avoid negative score
    return std::max(baseScore - penalty, 0);
}
// this func return vector of all the scores of the users from highest to lowest
std::vector<std::string> SqliteDatabase::getHighScores()
{
    std::vector<std::pair<std::string, int>> scores;  //vector of pairs to hold the scores and their owners
    sqlite3_stmt* stmt;
    //stmt to get all usernames
    const std::string sql = "SELECT USERNAME FROM USERS";

    if (sqlite3_prepare_v2(_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        //loop to get each score of players
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            //getting curr username
            std::string username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            int score = getPlayerScore(username);  //getting the score of the curr player
            {
                std::lock_guard<std::mutex> lock(mtx);
                scores.push_back({ username, score });  ///pushing score with the username
            }
        }
    }
    sqlite3_finalize(stmt);

    //sorting players by their scores
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::sort(scores.begin(), scores.end(), [](auto& a, auto& b) {
            return a.second > b.second;
            });
    }

    std::vector<std::string> result;
    //the limit is the top 5 scores, if no 5 scores its the curr max scores exist
    int limit = std::min(5, static_cast<int>(scores.size()));
    for (int i = 0; i < limit; ++i)
    {
        //pushing the username and score as string
        std::lock_guard<std::mutex> lock(mtx);
        result.push_back(scores[i].first + ": " + std::to_string(scores[i].second));
    }

    return result;
}
std::vector<Question> SqliteDatabase::getQuestions(int numOfQuestions)
{
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<Question> questions; //vector of questions
    sqlite3_stmt* stmt = nullptr;

    //getting questions from QUESTIONS table with the answers and correct answer in random order
    std::string sql = "SELECT QUESTION, ANSWER1, ANSWER2, ANSWER3, ANSWER4, CORRECT_ANSWER "
        "FROM QUESTIONS "
        "ORDER BY RANDOM() "
        "LIMIT ?;";

    if (sqlite3_prepare_v2(_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(_db)));
    }

    // Bind the limit value (number of questions to retrieve)
    if (sqlite3_bind_int(stmt, 1, numOfQuestions) != SQLITE_OK) {  //limit is the num of questions requested
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to bind parameter: " + std::string(sqlite3_errmsg(_db)));
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {  //loop through all rows
        //getting the question text
        std::string question = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

        std::vector<std::string> answers = { //adding all the answers to a vector
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))
        };

        int correctAnswer = sqlite3_column_int(stmt, 5) - 1;  //getting correct answer id like index

        //adding a Question instance to the questions vector
        questions.emplace_back(question, answers, correctAnswer);
    }

    sqlite3_finalize(stmt);
    return questions;  //returning the questions
}
int SqliteDatabase::SubmitGameStatistics(GameData& data)
{
    std::lock_guard<std::mutex> lock(mtx);
    //updating the game data of a user with the given data
    const char* sql = R"(
        INSERT INTO GAME_DATA (USERNAME, CORRECT_ANSWERS_COUNT, WRONG_ANSWERS_COUNT, AVG_ANSWER_TIME)
        VALUES (?, ?, ?, ?)
        ON CONFLICT(USERNAME) DO UPDATE SET
            CORRECT_ANSWERS_COUNT = excluded.CORRECT_ANSWERS_COUNT,
            WRONG_ANSWERS_COUNT = excluded.WRONG_ANSWERS_COUNT,
            AVG_ANSWER_TIME = excluded.AVG_ANSWER_TIME;
    )";
    sqlite3_stmt* stmt = nullptr;
    bool success = false;

    if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, data.username.c_str(), -1, SQLITE_STATIC);  //the username
        sqlite3_bind_int(stmt, 2, data.correctAnswerCount);   //the num of correct answers
        sqlite3_bind_int(stmt, 3, data.wrongAnswerCount);  //num of wrong answers
        sqlite3_bind_double(stmt, 4, data.averageAnswerTime);  //the avg answer time

        if (sqlite3_step(stmt) == SQLITE_DONE)   //if updating data succeded
        {
            success = true;  //returning true
        }
    }
    sqlite3_finalize(stmt);
    return success;
}
unsigned int SqliteDatabase::correctAnswerCount(const std::string& username)
{
    std::lock_guard<std::mutex> lock(mtx);
    //getting the num of correct answers by the username
    const char* sql = "SELECT CORRECT_ANSWERS_COUNT FROM GAME_DATA WHERE USERNAME = ?;";
    sqlite3_stmt* stmt = nullptr;
    unsigned int count = 0;

    if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);  //adding the username

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            count = sqlite3_column_int(stmt, 0);  //getting the num
        }
    }
    else
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(_db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return count;
}
unsigned int SqliteDatabase::wrongAnswerCount(const std::string& username)
{
    std::lock_guard<std::mutex> lock(mtx);
    //getting num of wrong answers by username from the GAME_DATA table
    const char* sql = "SELECT WRONG_ANSWERS_COUNT FROM GAME_DATA WHERE USERNAME = ?;";
    sqlite3_stmt* stmt = nullptr;
    unsigned int count = 0;

    if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);  //adding username

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            count = sqlite3_column_int(stmt, 0);  //getting the num
        }
    }

    sqlite3_finalize(stmt);
    return count;
}
float SqliteDatabase::averageAnswerTime(const std::string& username)
{
    std::lock_guard<std::mutex> lock(mtx);
    //getting avg answer time by username
    const char* sql = "SELECT AVG_ANSWER_TIME FROM GAME_DATA WHERE USERNAME = ?;";
    sqlite3_stmt* stmt = nullptr;
    float time = 0.0f;

    if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);  //adding username

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            time = static_cast<float>(sqlite3_column_double(stmt, 0));  //getting the avg
        }
    }

    sqlite3_finalize(stmt);
    return time;
}
void SqliteDatabase::updateUserStatistics(const GameData& data)
{
    std::lock_guard<std::mutex> lock(mtx);
    //updating the statistics of a user
    const char* sql = R"(
        INSERT INTO STATISTICS (USERNAME, AVG_TIME, CORRECT_ANSWERS, TOTAL_ANSWERS, NUM_OF_GAMES)
        VALUES (?, ?, ?, ?, 1)
        ON CONFLICT(USERNAME) DO UPDATE SET
            AVG_TIME = ((AVG_TIME * NUM_OF_GAMES) + excluded.AVG_TIME) / (NUM_OF_GAMES + 1),
            CORRECT_ANSWERS = CORRECT_ANSWERS + excluded.CORRECT_ANSWERS,
            TOTAL_ANSWERS = TOTAL_ANSWERS + excluded.TOTAL_ANSWERS,
            NUM_OF_GAMES = NUM_OF_GAMES + 1;
    )";

    sqlite3_stmt* stmt = nullptr;
    int totalAnswers = data.correctAnswerCount + data.wrongAnswerCount;  //getting total answers of the user

    if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, data.username.c_str(), -1, SQLITE_STATIC);  //adding username
        sqlite3_bind_double(stmt, 2, data.averageAnswerTime);  //adding averageAnswerTime
        sqlite3_bind_int(stmt, 3, data.correctAnswerCount);  //adding correctAnswerCount
        sqlite3_bind_int(stmt, 4, totalAnswers);  //adding totalAnswers

        sqlite3_step(stmt);
    }
    else
    {
        std::cerr << "SQL error (updateUserStatistics): " << sqlite3_errmsg(_db) << std::endl;
    }

    sqlite3_finalize(stmt);
    //deleting the game data table becouse updating the statistics ufter a game is done
    const char* dropSql = "DROP TABLE IF EXISTS GAME_DATA;";
    sqlite3_stmt* dropStmt = nullptr;

    if (sqlite3_prepare_v2(_db, dropSql, -1, &dropStmt, nullptr) == SQLITE_OK)
    {
        sqlite3_step(dropStmt);
    }
    else
    {
        std::cerr << "SQL error (drop GAME_DATA): " << sqlite3_errmsg(_db) << std::endl;
    }

    sqlite3_finalize(dropStmt);
}
